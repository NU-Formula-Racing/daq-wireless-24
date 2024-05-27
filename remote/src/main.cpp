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


void setup()
{
    Serial.begin(9600);
    g_comInterface.initialize();
}

void loop()
{
    g_comInterface.sendMessage(
        wircom::Message::createMetaMessageResponse(
            SCHEMA_NAME,
            SCHEMA_VERSION
    ));

    delay(1000);
}