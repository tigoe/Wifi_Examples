
/*
  WiFi getTime example

  Uses the WiFi.getTime() command to get the time from the network.

  For the libraries which have no getTime() function yet 
  (ESP32, MKR1000, Uno R4 WiFi), a getTime function is included.
  It's a modification of Michael Margolis' original UDP NTP example
  with a more minimal request and a return in the same function, 
  to make the process more portable. 

 Uses the following libraries:
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA  // use this for MKR1010 or Nano 33 IoT
  http://librarymanager/All#WiFiUDP
  
  For the Arduino Uno R4 WiFi, use
  #include <WiFiS3.h>
  For the Nano ESP32 use 
  #include <WiFi.h>

  created 13 Dec 2023
  by Tom Igoe
*/

// #include <WiFiNINA.h>  // use this for MKR1010 board and Nano 33 IoT
#include <WiFi101.h>  // use this for the MKR1000 board
// #include <WiFiS3.h>  // use this for the Uno R4 WiFi board
// #include <WiFi.h>  // use this for the Nano ESP32 board
#include <WiFiUdp.h>
#include "arduino_secrets.h"  // put SECRET_SSID and SECRET_PASS here

WiFiUDP Udp;
// local port to listen for UDP packets
const int localPort = 2390;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  connectToNetwork();
  Udp.begin(localPort);
}

void loop() {
  // if not connected, attempt to connect:
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
    return;
  }
  // get the time. Use this for the WiFiNINA lib:
  // epoch = WiFi.getTime();
  // get the time. Use this for the other libs:
  unsigned long epoch = getTime();
  Serial.println(epoch);
  delay(5000);
}

void connectToNetwork() {
  Serial.print("Attempting to connect to network ");
  Serial.println(SECRET_SSID);
  // try to connect to the network:
  while (WiFi.status() != WL_CONNECTED) {
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  // print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // print the signal strength:
  Serial.print("Signal Strength: ");
  Serial.println(WiFi.RSSI());
}

// send an NTP request and wait for the result:
unsigned long getTime() {
  // NTP timestamp packet is 48 bytes:
  const int NTP_PACKET_SIZE = 48;
  // diff between NTP result and current UTC epoch is 70 years in seconds:
  const unsigned long SEVENTY_YEARS = 2208988800UL;
  // byte buffer for NTP packet:
  byte packetBuffer[NTP_PACKET_SIZE];
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // simplest NTP request needs only the leap indicator, NTP version, and mode.
  // see RFC5905: https://datatracker.ietf.org/doc/html/rfc5905
  packetBuffer[0] = 0b00100011;  // Leap indicator 0, version 4, mode 3

  // send request to pool.ntp.org, port 123:
  Udp.beginPacket("pool.ntp.org", 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
  // wait for return packet:
  while (!Udp.parsePacket());
  // Parse the return packet:
  Udp.read(packetBuffer, NTP_PACKET_SIZE);
  //the timestamp  (32 bit integer) starts at byte 40
  // of the received packet:
  unsigned long result = 0;
  for (byte b = 40; b < 44; b++) {
    // move the current value left 8 bits, and add the next byte:
    result = (result << 8) | packetBuffer[b];
  }
  // subtract the time since 1900:
  result = result - SEVENTY_YEARS;
  return result;
}
