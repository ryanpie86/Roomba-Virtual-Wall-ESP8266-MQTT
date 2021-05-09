#include <Arduino.h>
#include "IRsend.h"
#include "IRremoteESP8266.h"
#include "EspMQTTClient.h"

const int ledPin = 2;
const uint16_t infraredPin = 12;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
unsigned long previousMillisIR = 0;
unsigned long previousMillisLED = 0;
const long infraredInterval = 100; // milliseconds between IR transmits... this is needed or the receiver will take the rawData as one giant transmit and it will fail
const long ledInterval = 1000; //milliseconds to flash LED
char infraredSwitchNumber = 0; // boot up in off mode.  state 1 starts the flashing loop.
char ledSwitchNumber = 0; //boot up in off mode. states 1 or 2 starts the flashing loop.

IRsend irsend(infraredPin);  // Set the GPIO to be used to sending the message.

uint16_t rawData[67] = {450, 7570, 425, 7565, 450}; // this is the raw IR code captured from my Roomba 960 virtual wall.  This may work with other Roomba models.

EspMQTTClient client(
  "WIFI SSID GOES HERE",
  "WIFI PASSWORD GOES HERE",
  "IP OF MQTT BROKER GOES HERE",     // MQTT Broker IP
//  "virtualwall",                   // Can be omitted if not needed
//  "somepass",                      // Can be omitted if not needed
  "OfficeVirtualWall",               // Client name that uniquely identify your device
  1883                               // The MQTT port, default to 1883. this line can be omitted
);

void setup()
{
  irsend.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266
  pinMode(2, OUTPUT);
  digitalWrite(2, 0); // turn off the LED on boot before connection

  
  // Optional functionnalities of EspMQTTClient :
  //client.enableDebuggingMessages();                            // Enable debugging messages sent to serial output
  //client.enableHTTPWebUpdater();                               // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("virtualwall/state", "offline");  // You can activate the retain flag by setting the third parameter to true

}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient

void onConnectionEstablished()
{
  digitalWrite(2, 1); // turn on the LED to show it is connected
  client.subscribe("virtualwall/command", onCommandReceived);

  // Publish that device is online
  client.publish("virtualwall/state", "online"); // You can activate the retain flag by setting the third parameter to true

  // Report the current state of pin to MQTT after connection
  if (char(infraredSwitchNumber) == char('1')) {
    client.publish("virtualwall/state/power", "ON");
  }
  else {
    client.publish("virtualwall/state/power", "OFF");
  }
}

void onCommandReceived(const String& message) {
  Serial.print("message received from virtualwall/command: " + message);
  client.publish("virtualwall/state/power", message);
  if ((String)message == String("ON")) {
    char(infraredSwitchNumber = char('1'));
    char(ledSwitchNumber = char('1')); // set this to 0 to disable flashing when loop is running.
  }
  else {
    char(infraredSwitchNumber = char('0'));
    char(ledSwitchNumber = char('0'));
    digitalWrite(2, 1); // turn the LED back to just 'on' if loop is not running.  this can be commented out.
  }
}


void loop()
{
  // loop for IR

  unsigned long currentMillisIR = millis();
  if (currentMillisIR - previousMillisIR >= infraredInterval) {
    previousMillisIR = currentMillisIR;
    if (char(infraredSwitchNumber) == char('1')) {
      irsend.sendRaw(rawData, 67, 38);
    }
  }

  // loop for LED

  unsigned long currentMillisLED = millis();
  if (currentMillisLED - previousMillisLED >= ledInterval) {
    previousMillisLED = currentMillisLED;
    if (char(ledSwitchNumber) == char('1')) {
      digitalWrite(2, 1);
      char(ledSwitchNumber = char('2'));
    }
    else if (char(ledSwitchNumber) == char('2')) {
      digitalWrite(2, 0);
      char(ledSwitchNumber = char('1'));
    }
  }
  client.loop();
}
