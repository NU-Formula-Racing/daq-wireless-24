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

#include "schema.hpp"


#define VERSION_ARGS(major, minor, patch) major, minor, patch
#define TIMEOUT 1000
#define REPORTS_PER_SECOND 5
#define DATA_REQUESTS_PER_SECOND 5

#define LED_PIN 8

VirtualTimerGroup g_initalizationTimer;
VirtualTimerGroup g_reportingTimer;
wircom::ComInterface g_comInterface{};
std::string g_schemaName;
int g_version[3]; // major, minor, patch
bool ledState = false;

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
        std::cout << "Connection established! Set through the metdata file" << std::endl;
        g_state = ApplicationState::CONNECTION_ESTABLISHED;
    }
    else
    {
        std::cout << "Schema not found in daqser registry" << std::endl;
        std::cout << "Sending a request for the DRIVE" << std::endl;
        g_comInterface.sendMessage(wircom::MessageBuilder::createDriveMessageRequest());
    }
}

void onRecieveDriveResponse(wircom::Message msg)
{
    // std::vector<std::uint8_t>& data = msg.data;
    // std::cout << "Received message of length: " << data.size() << std::endl;

    // for (int i = 0; i < data.size(); i++)
    // {
    //     std::cout << data[i];
    // }

    // wircom::ContentResult<wircom::DriveContent> res = wircom::MessageParser::parseDriveContent(data);

    // std::cout << "Parsed drive content: " << res.content.driveContent << std::endl;
    
    // if (!res.success)
    // {
    //     std::cout << "Failed to parse drive content" << std::endl;
    //     return;
    // }
    // // now we can set the application state to CONNECTION_ESTABLISHED
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

void reportData()
{
    if (g_state != ApplicationState::CONNECTION_ESTABLISHED)
    {
        return;
    }

    std::cout << "Reporting data..." << std::endl;
    std::vector<std::uint8_t> data = daqser::serializeFrame();
    for (int i = 0; i < data.size(); i++)
    {
        std::cout << data[i];
    }
    std::cout << std::endl;
}

void requestData()
{
    if (g_state != ApplicationState::CONNECTION_ESTABLISHED)
    {
        return;
    }

    std::cout << "Requesting data..." << std::endl;
    g_comInterface.sendMessage(wircom::MessageBuilder::createDataTransferRequest(), false);

}

void blinkLED()
{
    if (ledState)
    {
        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
    }
    ledState = !ledState;
}


void setup()
{
    Serial.begin(9600);
    std::cout << "Initializing daqser..." << std::endl;
    daqser::initialize(false);
    daqser::setSchema(SCHEMA_CONTENTS);

    std::cout << "Initializing wircom..." << std::endl;
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
    g_comInterface.addRXCallback(
        wircom::MessageType::MSG_RESPONSE,
        wircom::MessageContentType::MSG_CON_DATA_TRANSFER, 
        onDataTransfer
    );

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    threads.addThread(listenForMessages);
    g_reportingTimer.AddTimer(1000 / REPORTS_PER_SECOND, reportData);
    g_reportingTimer.AddTimer(1000 / DATA_REQUESTS_PER_SECOND, requestData);
    
    g_initalizationTimer.AddTimer(200, blinkLED);

    std::cout << "Attempting to establish connection with the car..." << std::endl;
    // g_comInterface.sendMessage(wircom::MessageBuilder::createMetaMessageRequest());
    g_state = ApplicationState::CONNECTION_ESTABLISHED;
}

void loop()
{
    // listenForMessages();

    if (g_state != ApplicationState::CONNECTION_ESTABLISHED)
    {
        g_initalizationTimer.Tick(millis());
        return;
    }

    // turn on the LED
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    g_reportingTimer.Tick(millis());
}