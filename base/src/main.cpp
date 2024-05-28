#include <Arduino.h>
#include <daqser.hpp>
#include <virtualTimer.h>
#include <message.hpp>
#include <com_interface.hpp>
#include <RH_RF95.h> // dependency of com_interface.hpp, for some reason, it isn't included in the main.cpp file

#include <vector>

#define VERSION_ARGS(major, minor, patch) major, minor, patch
#define SCHEMA_NAME "esp32dev_test"
#define SCHEMA_VERSION VERSION_ARGS(1, 0, 0)

wircom::ComInterface g_comInterface{};

void onReceiveAnyMessage(std::vector<std::uint8_t> data)
{
    std::cout << "Received message of length: " << data.size() << std::endl;
    for (std::uint8_t byte : data)
    {
        Serial.print(byte, HEX);
        Serial.print(" ");
    }

    Serial.println();
}

void onRecieveMetaMessage(std::vector<std::uint8_t> data)
{
    std::cout << "Received meta message of length: " << data.size() << std::endl;
    for (std::uint8_t byte : data)
    {
        Serial.print(byte, HEX);
        Serial.print(" ");
    }

    Serial.println();
}

void setup()
{
    Serial.begin(9600);
    g_comInterface.initialize();
    g_comInterface.addRXCallbackToAny(onReceiveAnyMessage);
    g_comInterface.addRXCallback(wircom::MessageContentType::MSG_CON_META, onRecieveMetaMessage);
}

void loop()
{
    g_comInterface.listen();
}