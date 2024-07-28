# Wifi Examples

These examples are for the Arduino WiFi libraries. These examples will work with:  the Arduino [MKR1000](https://store.arduino.cc/usa/arduino-mkr1000) board; the [MKR1010](https://store.arduino.cc/usa/mkr-wifi-1010); [Nano 33 IoT](https://store.arduino.cc/usa/nano-33-iot) board; [Uno WiFi rev 2](https://store.arduino.cc/usa/arduino-uno-wifi-rev2) boards; the [Uno R4 WiFi](https://docs.arduino.cc/hardware/uno-r4-wifi) board; and the [Nano ESP32](https://docs.arduino.cc/hardware/nano-esp32) board.

 The libraries for these boards should be API-compatible with each other in these examples, though some will have features that others do not. Any library-specific examples in this repository are noted as such.
 
Many of these examples are also compatible with the ESP8266WiFi library, which has a similar API to these libraries. 

Many of these examples also use the [ArduinoHttpClient](https://github.com/arduino-libraries/ArduinoHttpClient) and [Arduino_JSON](https://github.com/arduino-libraries/Arduino_JSON) libraries.

## Library Include line

All of these examples require you to include the WiFi library for their board at the top of the code. Not all of the examples have been updated for the newest boards, however. Here's a guide to including the right library:

| Board | Library | #include line |
| --- | --- | --- |
| MKR1000 | [WiFi101](https://www.arduino.cc/en/Reference/WiFi101) library | `#include <WiFi101.h>` |
| MKR1010, Nano 33 IoT, and Uno WiFi rev 2 | [WiFiNINA](https://www.arduino.cc/en/Reference/WiFiNINA) library | `#include <WiFiNINA.h>` |
| Uno R4 WiFi | [WiFiS3](https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples) library | `#include <WiFiS3.h>` |
| Nano ESP32 | WiFi library for the [Nano ESP32](https://docs.arduino.cc/hardware/nano-esp32) | `#include <WiFi.h>` |

## Credentials Header file

All of these examples expect you to add an `arduino_secrets.h` header file to the sketch with the following information: 
````arduino
#define SECRET_SSID "" // your WiFi access point name
#define SECRET_PASS "" // your WiFi password
````

Some sketches will add other constants to that file as well. These files are ignored by the git repo using `.gitignore`.

## WiFi Connection
* [WiFi_Reconnect](https://github.com/tigoe/Wifi_examples/tree/main/WiFi_Connection_Examples/WiFi_Reconnect) -  Shows how to reconnect to the network in the main loop. Also prints out network signal strength (RSSI).
* [WiFi_RssiTest](https://github.com/tigoe/Wifi_examples/tree/main/WiFi_Connection_Examples/WiFi_RssiTest) - Prints the following to the serial monitor: SSID, BSSID, RSSI, WiFi status, reconnect count.
* [WiFi_ConnectionTimeLogger](https://github.com/tigoe/Wifi_examples/tree/main/WiFi_Connection_Examples/WiFi_ConnectionTimeLogger) - Logs the health of the network connection to an SD card. Periodically logs: date,time, MAC address, SSID, BSSID, IP Address, signal strength (RSSI), WiFi status, reconnect count, and uptime.
* [WiFI_ipAddress](https://github.com/tigoe/Wifi_examples/tree/main/WiFi_Connection_Examples/WiFi_ipAddress) - The IP address in the WiFi libraries is a 4-byte array. This example shows how to get it as a String.
* [Connecting to Eduroam](eduroam.md) Can't connect easily to your school's WiFi? Try Eduroam. 
* [WiFI_ESP_eduroam_connection](https://github.com/tigoe/Wifi_examples/blob/main/WiFi_Connection_Examples/WIFi_ESP_eduroam_connection/WIFi_ESP_eduroam_connection.ino) - This example shows how to connect to [eduroam](eduroam.md) networks using the ESP32 WiFi library. 

## Arduino HTTP Clients
* [HttpClientSimple](https://github.com/tigoe/Wifi_examples/tree/main/ArduinoHttpClient_Examples/HttpClientSimple) - a simple example of an HTTP client. 
* [HttpClientGetJSON](https://github.com/tigoe/Wifi_examples/tree/main/ArduinoHttpClient_Examples/HttpClientGetJSON) - makes an HTTP GET request and parses a JSON response using the Arduino_JSON library. Reads from [this server](https://glitch.com/edit/#!/arduino-to-json).            
* [HttpClientRemoteStatusCheck](https://github.com/tigoe/Wifi_examples/tree/main/ArduinoHttpClient_Examples/HttpClientRemoteStatusCheck) - sends an HTTP request once a minute. Shows how to use the RTC alarm functions to set a timed event. Also listens on a UDP port so you can check when the last successful request was made. Designed to log sensor data over HTTP, while letting you check that it's still working over UDP, so you don't have to disconnect it from the network to see how it's doing. 
* [HttpSSLClient_OLED](https://github.com/tigoe/Wifi_examples/tree/main/ArduinoHttpClient_Examples/HttpSSLClient_OLED) - makes an HTTPS call and displays the result on an SSD1306 OLED. Shows some String manipulation techniques as well.
* [ConnDevClient](https://github.com/tigoe/Wifi_examples/tree/main/ArduinoHttpClient_Examples/ConnDevClient) - an HTTPS client for the the  [Connected Devices server](https://github.com/don/itp-connected-devices`), written by Mathura Govindarajan and Don Coleman

## HTTP Servers
* [SimplerWebServer](https://github.com/tigoe/Wifi_examples/tree/main/Servers/SimplerWebServer) - a stripped-down example of how to respond to HTTP requests
* [NetworkClientLogger](https://github.com/tigoe/Wifi_examples/tree/main/Servers/NetworkClientLogger) - listens on a number of popular ports (HTTP, SMTP, SSH, etc) and logs to an SD card all client connnection attempts
* [UptimeServer](https://github.com/tigoe/Wifi_examples/tree/main/Servers/UptimeServer) - a web server that reports the device's uptime when it receives an HTTP request. 
* [SensorWebServerWithCSSandJS](https://github.com/tigoe/Wifi_examples/tree/main/Servers/SensorWebServerWithCSSandJS) - a web server that reports reads sensors and delivers them as JSON when it receives an HTTP request. This example also shows how to serve an index.html, styles.css, and script.js file separately. 

## TCP and UDP Examples
* [simpleTCPClient](https://github.com/tigoe/Wifi_examples/tree/main/simpleTCPClient) - makes a TCP connection to a remote * host and sends a message every ten seconds
* [simpleTCPServer](https://github.com/tigoe/Wifi_examples/tree/main/simpleTCPServer) - listens for TCP connections and responds.
* [simpleUDPClient](https://github.com/tigoe/Wifi_examples/tree/main/simpleUDPClient) - sends and listens for UDP packets
* [UDPEcho](https://github.com/tigoe/Wifi_examples/tree/main/UDPEcho) - listens for UDP packets and echoes them back
* [UDPRelayTimeTest](https://github.com/tigoe/Wifi_examples/tree/main/UDPRelayTimeTest) - sends UDP packets, waits for a reply from the remote host, and notes the round trip time. 
* [TCPRelay](https://github.com/tigoe/Wifi_examples/tree/main/TCPRelay) - listens for TCP connections and echoes what comes in to a second remote host. 

## OSC Examples
* [WifiUDP_OSC_MIDI_Client](https://github.com/tigoe/Wifi_examples/tree/main/OSC_Examples/WifiUDP_OSC_MIDI_Client) - sends MIDI noteon and noteoff messages over OSC
* [OSC_MIDI_oneKey_improviser](https://github.com/tigoe/Wifi_examples/tree/main/OSC_Examples/OSC_MIDI_oneKey_improviser) - sends MIDI noteon and noteoff messages over OSC, constrained to a particular key, and on the press of a pushbutton


