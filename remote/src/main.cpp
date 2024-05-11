#include <Arduino.h>
#include <daqser.hpp>
// #include <daqser_can.hpp>
#include <virtualTimer.h>
#include <teensy_can.h>

#define VERSION_ARGS(major, minor, patch) major, minor, patch

#define SCHEMA_NAME "test_schema_v1.0.0.drive"
#define SCHEMA_VERSION VERSION_ARGS(1, 0, 0)

// Listen to boards
// #define RX_BMS
// #define RX_IMD

VirtualTimerGroup timerGroup;
// Function to record and send data
// Called every 100ms
void recordData()
{
  //  Prepare data from the CAN bus for serialization
  // daqser::updateSignals();
  // Serialize the data, and get the raw bytes
  std::vector<std::uint8_t> byteData = daqser::serializeFrame();
  // send the data over lora or something
  // send(byteData);
}

void setup()
{
  Serial.begin(115200);
  // Initialize the daqser library
  daqser::initialize();
  // Tell daqser what schema we are using to serialize the data
  daqser::setSchema(SCHEMA_NAME, SCHEMA_VERSION);
  // Tell daqser to record/send data from these boards
  timerGroup.AddTimer(100, recordData);
}

void loop()
{
  timerGroup.Tick(millis());
}