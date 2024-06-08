#include <Arduino.h>
#include <daqser.hpp>
#include <virtualTimer.h>
#include <message.hpp>
#include <builder.hpp>
#include <com_interface.hpp>
#include <RH_RF95.h> // dependency of com_interface.hpp, for some reason, it isn't included in the main.cpp file
#include <TeensyThreads.h>

#include <vector>

#define VERSION_ARGS(major, minor, patch) major, minor, patch
#define SCHEMA_NAME "esp32dev_test"
#define SCHEMA_VERSION VERSION_ARGS(1, 0, 0)
#define TIMEOUT 1000

wircom::ComInterface g_comInterface{};

void listenForMessages()
{
    while (true)
    {
        // std::cout << "Listening for messages" << std::endl;
        g_comInterface.listen(TIMEOUT);
        g_comInterface.tick();
    }
}

void onMetaRequest(wircom::Message message)
{
    Serial.println("Meta request received");
    wircom::Message response = wircom::MessageBuilder::createMetaMessageResponse(message.messageID, SCHEMA_NAME, SCHEMA_VERSION);
    g_comInterface.sendMessage(response);
}

void setup()
{
    Serial.begin(9600);
    g_comInterface.initialize();
    threads.addThread(listenForMessages);

    g_comInterface.addRXCallback(
        wircom::MessageType::MSG_REQUEST,
        wircom::MessageContentType::MSG_CON_META, 
        onMetaRequest
    );
}

void loop()
{
    std::cout << "Looping" << std::endl;
}