#include <Arduino.h>
#include <virtualTimer.h>
#include <vector>
#include <TeensyThreads.h>

// daqser
#include <daqser.hpp>
#include <daqser_can.hpp>

// wircom
#include <message.hpp>
#include <builder.hpp>
#include <com_interface.hpp>
#include <RH_RF95.h> // dependency of com_interface.hpp, for some reason, it isn't included in the main.cpp file

#include "schema.hpp"

#define VERSION_ARGS(major, minor, patch) major, minor, patch
#define SCHEMA_NAME "daq-schema"
#define SCHEMA_VERSION VERSION_ARGS(1, 0, 0)
#define LISTEN_TIMEOUT 1000
#define DT_PER_SECOND 5

VirtualTimerGroup g_dataTransferTimer;
wircom::ComInterface g_comInterface{};

unsigned long g_startTransmissionTime = millis();
unsigned long g_endTransmissionTime = g_startTransmissionTime + 1000 / DT_PER_SECOND;


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

    std::cout << "Initializing daqser..." << std::endl;
    daqser::initialize();
    daqser::initializeCAN();
    daqser::setSchema(SCHEMA_CONTENTS); // work around cause I just learned that you can't flash files to the teensy

    std::cout << "Initializing wircom..." << std::endl;
    g_comInterface.initialize();
    threads.addThread(listenForMessages);
    g_comInterface.addRXCallback(
        wircom::MessageType::MSG_REQUEST,
        wircom::MessageContentType::MSG_CON_META,
        onMetaRequest);
    g_comInterface.addRXCallback(
        wircom::MessageType::MSG_REQUEST,
        wircom::MessageContentType::MSG_CON_DRIVE,
        onDriveRequest);

    g_dataTransferTimer.AddTimer(10, []()
                                 { daqser::tickCAN(); });
    std::cout << "Finished setup" << std::endl;
}

void loop()
{
    g_dataTransferTimer.Tick(millis());

    if (millis() >= g_endTransmissionTime)
    {
        std::cout << "Sending data..." << std::endl;
        daqser::updateSignals();
        std::cout << "CAN Updated" << std::endl;
        // send daqser data over serial
        std::vector<std::uint8_t> data = daqser::serializeFrame();
        for (std::uint8_t byte : data)
        {
            std::cout << std::hex << (int)byte << " ";
        }
        std::cout << std::endl;

        wircom::Message message = wircom::MessageBuilder::createDataTransferMessage(data);
        g_comInterface.sendMessage(message);

        g_startTransmissionTime = millis();
        g_startTransmissionTime = g_startTransmissionTime + 1000 / DT_PER_SECOND;
    }
}