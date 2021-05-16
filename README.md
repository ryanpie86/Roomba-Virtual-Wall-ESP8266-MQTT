# Roomba-Virtual-Wall-ESP8266-MQTT
An MQTT-based Virtual Wall for ESP8266 Devices

I made this based off of the IRSend, IRremoteESP8266, and EspMQTTClient libraries.  While https://github.com/MKme/Roomba was a great idea,
I wanted mine to be smart and able to be controlled through Home Assistant and Alexa.  Thus, I ventured out to make this.

I am using a Wemos D1 Mini, Everlight IR333-A infrared emitter, a 10 ohm resistor for power reduction from 5v to 4v, and a 1.2K resistor
for the drain to a 2N2222A NPN transistor. I pulled the raw IR from my existing Virtual Wall using Tasmota and then copied it into this
code.  In this configuration, it will stop my Roomba 960 from half way across a large room.

I'm new to Arduino and C* type programming, so please feel free to make any improvements.

The box STL was originally made by matsekberg who can be found on Thingiverse:
https://www.thingiverse.com/thing:1995963
https://www.thingiverse.com/matsekberg
