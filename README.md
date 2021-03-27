# Wifi101_examples

These examples are for the Arduino [WiFi101](https://www.arduino.cc/en/Reference/WiFi101) and [WiFiNINA](https://www.arduino.cc/en/Reference/WiFiNINA) libraries. The former works with the Arduino [MKR1000](https://store.arduino.cc/usa/arduino-mkr1000) board, and the latter work with the [MKR1010](https://store.arduino.cc/usa/mkr-wifi-1010), [Nano 33 IoT](https://store.arduino.cc/usa/nano-33-iot), and [Uno WiFi rev 2](https://store.arduino.cc/usa/arduino-uno-wifi-rev2) boards. Many of these examples are also compatible with the ESP8266WiFi library, which has a similar API to the WIFi101 and WiFiNINA libraries. 

Many of these examples also use the [ArduinoHttpClient](https://github.com/arduino-libraries/ArduinoHttpClient) and [Arduino_JSON](https://github.com/arduino-libraries/Arduino_JSON) libraries.

## Credentials Header file

All of these examples expect you to add an `arduino_secrets.h` header file to the sketch with the following information: 
````arduino
#define SECRET_SSID "" // your WiFi access point name
#define SECRET_PASS "" // your WiFi password
````

Some sketches will add other constants to that file as well. These files are ignored by the git repo using `.gitignore`.

## WiFi Connection
* [WiFi_Reconnect](https://github.com/tigoe/Wifi101_examples/tree/master/WiFi_Connection_Examples/WiFi_Reconnect) -  Shows how to reconnect to the network in the main loop. Also prints out network signal strength (RSSI).
* [WiFi_RssiTest](https://github.com/tigoe/Wifi101_examples/tree/master/WiFi_Connection_Examples/WiFi_RssiTest) - Prints the following to the serial monitor: SSID, BSSID, RSSI, WiFi status, reconnect count.
* [WiFi_ConnectionTimeLogger](https://github.com/tigoe/Wifi101_examples/tree/master/WiFi_Connection_Examples/WiFi_ConnectionTimeLogger) - Logs the health of the network connection to an SD card. Periodically logs: date,time, MAC address, SSID, BSSID, IP Address, signal strength (RSSI), WiFi status, reconnect count, and uptime.
* [WiFI_ipAddress](https://github.com/tigoe/Wifi101_examples/tree/master/WiFi_Connection_Examples/WiFi_ipAddress) - The IP address in the WiFi libraries is a 4-byte array. This example shows how to get it as a String.

## Arduino HTTP Clients
* [HttpClientSimple](https://github.com/tigoe/Wifi101_examples/tree/master/ArduinoHttpClient_Examples/HttpClientSimple) - a simple example of an HTTP client. 
* [HttpClientGetJSON](https://github.com/tigoe/Wifi101_examples/tree/master/ArduinoHttpClient_Examples/HttpClientGetJSON) - makes an HTTP GET request and parses a JSON response using the Arduino_JSON library             
* [HttpClientRemoteStatusCheck](https://github.com/tigoe/Wifi101_examples/tree/master/ArduinoHttpClient_Examples/HttpClientRemoteStatusCheck) - sends an HTTP request once a minute. Shows how to use the RTC alarm functions to set a timed event. Also listens on a UDP port so you can check when the last successful request was made. Designed to log sensor data over HTTP, while letting you check that it's still working over UDP, so you don't have to disconnect it from the network to see how it's doing. 
* [HttpSSLClient_OLED](https://github.com/tigoe/Wifi101_examples/tree/master/ArduinoHttpClient_Examples/HttpSSLClient_OLED) - makes an HTTPS call and displays the result on an SSD1306 OLED. Shows some String manipulation techniques as well.
* [ConnDevClient](https://github.com/tigoe/Wifi101_examples/tree/master/ArduinoHttpClient_Examples/ConnDevClient) - an HTTPS client for the the  [Connected Devices server](https://github.com/don/itp-connected-devices`), written by Mathura Govindarajan and Don Coleman

## HTTP Servers
* [SimplerWebServer](https://github.com/tigoe/Wifi101_examples/tree/master/Servers/SimplerWebServer) - a stripped-down example of how to respond to HTTP requests
* [NetworkClientLogger](https://github.com/tigoe/Wifi101_examples/tree/master/Servers/NetworkClientLogger) - listens on a number of popular ports (HTTP, SMTP, SSH, etc) and logs to an SD card all client connnection attempts
* [UptimeServer](https://github.com/tigoe/Wifi101_examples/tree/master/Servers/UptimeServer) - a web server that reports the device's uptime when it receives an HTTP request. 

## TCP and UDP Examples
* [simpleTCPClient](https://github.com/tigoe/Wifi101_examples/tree/master/simpleTCPClient) - makes a TCP connection to a remote * host and sends a message every ten seconds
* [simpleTCPServer](https://github.com/tigoe/Wifi101_examples/tree/master/simpleTCPServer) - listens for TCP connections and responds.
* [simpleUDPClient](https://github.com/tigoe/Wifi101_examples/tree/master/simpleUDPClient) - sends and listens for UDP packets
* [UDPEcho](https://github.com/tigoe/Wifi101_examples/tree/master/UDPEcho) - listens for UDP packets and echoes them back
* [UDPRelayTimeTest](https://github.com/tigoe/Wifi101_examples/tree/master/UDPRelayTimeTest) - sends UDP packets, waits for a reply from the remote host, and notes the round trip time. 
* [TCPRelay](https://github.com/tigoe/Wifi101_examples/tree/master/TCPRelay) - listens for TCP connections and echoes what comes in to a second remote host. 

## OSC Examples
* [WifiUDP_OSC_MIDI_Client](https://github.com/tigoe/Wifi101_examples/tree/master/OSC_Examples/WifiUDP_OSC_MIDI_Client) - sends MIDI noteon and noteoff messages over OSC
* [OSC_MIDI_oneKey_improviser](https://github.com/tigoe/Wifi101_examples/tree/master/OSC_Examples/OSC_MIDI_oneKey_improviser) - sends MIDI noteon and noteoff messages over OSC, constrained to a particular key, and on the press of a pushbutton


