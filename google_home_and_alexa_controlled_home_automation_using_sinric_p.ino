//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 
#define device_ID_1   "***********************"
#define device_ID_2   "***********************"
#define WIFI_SSID         "********************"    
#define WIFI_PASS         "**********************"
#define APP_KEY           "************************"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "*****************************************"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define RelayPin1 23  //D23
#define RelayPin2 22  //D22
#define BAUD_RATE  9600              // Change baudrate to your need


#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>

/*****************
 * Configuration *
 *****************/

struct RelayInfo {
  String deviceId;
  String name;
  int pin;
};

std::vector<RelayInfo> relays = {
    {device_ID_1, "Relay 1", RelayPin1},
    {device_ID_2, "Relay 2", RelayPin2}
    };


bool onPowerState(const String &deviceId, bool &state) {
  for (auto &relay : relays) {                                                            // for each relay configuration
    if (deviceId == relay.deviceId) {                                                       // check if deviceId matches
      Serial.printf("Device %s turned %s\r\n", relay.name.c_str(), state ? "on" : "off");     // print relay name and state to serial
      digitalWrite(relay.pin, state);                                                         // set state to digital pin / gpio
      return true;                                                                            // return with success true
    }
  }
  return false; // if no relay configuration was found, return false
}

void setupRelayPins() {
  for (auto &relay : relays) {    // for each relay configuration
    pinMode(relay.pin, OUTPUT);     // set pinMode to OUTPUT
  }
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
  for (auto &relay : relays) {                             // for each relay configuration
    SinricProSwitch &mySwitch = SinricPro[relay.deviceId];   // create a new device with deviceId from relay configuration
    mySwitch.onPowerState(onPowerState);                     // attach onPowerState callback to the new device
  }

  SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(BAUD_RATE);
  setupRelayPins();
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}
