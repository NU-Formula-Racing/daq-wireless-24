#include <Arduino.h>
#include <daqser.hpp>
#include <daqser_can.hpp>
#include <virtualTimer.h>
#include <message.hpp>
#include <builder.hpp>
#include <com_interface.hpp>
#include <RH_RF95.h> // dependency of com_interface.hpp, for some reason, it isn't included in the main.cpp file
#include <TeensyThreads.h>

#include <vector>

#define VERSION_ARGS(major, minor, patch) major, minor, patch
#define SCHEMA_NAME "daq-schema"
#define SCHEMA_VERSION VERSION_ARGS(1, 0, 0)
#define LISTEN_TIMEOUT 1000
#define DT_PER_SECOND 5

VirtualTimer g_dataTransferTimer;
wircom::ComInterface g_comInterface{};

void listenForMessages()
{
    while (true)
    {
        g_comInterface.listen(LISTEN_TIMEOUT);
        g_comInterface.tick();
    }
}

void onMetaRequest(wircom::Message message)
{
    Serial.println("Meta request received");
    wircom::Message response = wircom::MessageBuilder::createMetaMessageResponse(message.messageID, SCHEMA_NAME, SCHEMA_VERSION);
    g_comInterface.sendMessage(response);
}

void onDriveRequest(wircom::Message message)
{
    // get the contents of the drive file from daqser
    std::string driveFileContents = daqser::getDriveContents();
    // create a message with the drive file contents
    wircom::Message response = wircom::MessageBuilder::createDriveMessageResponse(message.messageID, driveFileContents);
    // send the message
    g_comInterface.sendMessage(response);
}

void setup()
{
    Serial.begin(9600);
    daqser::initialize();
    daqser::initializeCAN();
    g_comInterface.initialize();

    threads.addThread(listenForMessages);

    g_comInterface.addRXCallback(
        wircom::MessageType::MSG_REQUEST,
        wircom::MessageContentType::MSG_CON_META, 
        onMetaRequest
    );

    g_comInterface.addRXCallback(
        wircom::MessageType::MSG_REQUEST,
        wircom::MessageContentType::MSG_CON_DRIVE,
        onDriveRequest
    );
}

void loop()
{
    while (true)
    {
        unsigned long start = millis();
        unsigned long end = start + 1000 / DT_PER_SECOND;

        if (millis() >= end)
        {
            std::cout << "Sending data..." << std::endl;
            daqser::tickCAN();
            // send daqser data over serial
            std::vector<std::uint8_t> data = daqser::serializeFrame();
            wircom::Message message = wircom::MessageBuilder::createDataTransferMessage(data);
            g_comInterface.sendMessage(message);

            start = millis();
            end = start + 1000 / DT_PER_SECOND;
        }
    }
}