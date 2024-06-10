#include <Arduino.h>
#include <vector>
#include <virtualTimer.h>

// daqser
#include <daqser.hpp>

// wireless communication
#include <message.hpp>
#include <builder.hpp>
#include <com_interface.hpp>
#include <RH_RF95.h> // dependency of com_interface.hpp, for some reason, it isn't included in the main.cpp file
#include <TeensyThreads.h>


#define VERSION_ARGS(major, minor, patch) major, minor, patch
#define TIMEOUT 1000
#define REPORTS_PER_SECOND 5

wircom::ComInterface g_comInterface{};
std::string g_schemaName;
int g_version[3]; // major, minor, patch

enum ApplicationState
{
    INITIALIZING_CONNECTION = 0,
    CONNECTION_ESTABLISHED = 1,
};


ApplicationState g_state = ApplicationState::INITIALIZING_CONNECTION;

void listenForMessages()
{
    while (true)
    {
        g_comInterface.listen(TIMEOUT);
        g_comInterface.tick();
    }
}

void onRecieveMetaResponse(wircom::Message msg)
{
    std::vector<std::uint8_t> data = msg.data;
    std::cout << "Received message of length: " << data.size() << std::endl;
    wircom::ContentResult<wircom::MetaContent> res = wircom::MessageParser::parseMetaContent(data);
    if (res.success)
    {
        std::cout << "Parsed meta content: " << res.content.schemaName << " " << res.content.major << "." << res.content.minor << "." << res.content.patch << std::endl;
    }
    else
    {
        std::cout << "Failed to parse meta content" << std::endl;
    }
    Serial.println();

    g_schemaName = res.content.schemaName;
    g_version[0] = res.content.major;
    g_version[1] = res.content.minor;
    g_version[2] = res.content.patch;

    // lets see if daqser knows about this schema
    if (daqser::hasSchema(g_schemaName, res.content.major, res.content.minor, res.content.patch))
    {
        std::cout << "Schema found in daqser registry" << std::endl;
        daqser::setSchema(g_schemaName, res.content.major, res.content.minor, res.content.patch);
        g_state = ApplicationState::CONNECTION_ESTABLISHED;
    }
    else
    {
        std::cout << "Schema not found in daqser registry" << std::endl;
        std::cout << "Sending a request for the DRIVE" << std::endl;
        g_comInterface.sendMessage(wircom::MessageBuilder::createMetaMessageRequest());
    }
}

void onRecieveDriveResponse(wircom::Message msg)
{
    std::vector<std::uint8_t> data = msg.data;
    std::cout << "Received message of length: " << data.size() << std::endl;
    wircom::ContentResult<wircom::DriveContent> res = wircom::MessageParser::parseDriveContent(data);
    if (res.success)
    {
        std::cout << "Parsed drive content: " << res.content.driveContent << std::endl;
    }
    else
    {
        std::cout << "Failed to parse drive content" << std::endl;
    }
    Serial.println();

    // now set the schema from the drive file
    // daqser::setSchemaFromDrive(res.content.driveContent);

    // now we can set the application state to CONNECTION_ESTABLISHED
    g_state = ApplicationState::CONNECTION_ESTABLISHED;
}

void onDataTransfer(wircom::Message msg)
{
    if (g_state != ApplicationState::CONNECTION_ESTABLISHED)
    {
        return; // we don't know enough about the schema to do anything
    }

    std::vector<std::uint8_t> data = msg.data;
    std::cout << "Received message of length: " << data.size() << std::endl;
    std::cout << "Parsing message..." << std::endl;
    daqser::deserializeFrame(data);
}


void setup()
{
    Serial.begin(9600);
    daqser::initialize();
    g_comInterface.initialize();
    g_comInterface.addRXCallback(
        wircom::MessageType::MSG_RESPONSE,
        wircom::MessageContentType::MSG_CON_META, 
        onRecieveMetaResponse
    );

    g_comInterface.addRXCallback(
        wircom::MessageType::MSG_RESPONSE,
        wircom::MessageContentType::MSG_CON_DRIVE, 
        onRecieveDriveResponse
    );

    threads.addThread(listenForMessages);
    g_comInterface.sendMessage(wircom::MessageBuilder::createMetaMessageRequest());
}

void loop()
{
    if (g_state != ApplicationState::CONNECTION_ESTABLISHED)
    {
        return;
    }

    while (true)
    {
        unsigned long start = millis();
        unsigned long end = start + 1000 / REPORTS_PER_SECOND;

        if (millis() >= end)
        {
            // send daqser data over serial
            std::string json = daqser::serializeFrameToJson();
            std::cout << json << std::endl;

            start = millis();
            end = start + 1000 / REPORTS_PER_SECOND;
        }
    }
}