/*
  Web  Server with client display
  Shows the client address in the Serial Monitor and returns
  the time UTC. Checks the client request path, and
  if the request is for a particular string, responds accordingly

  created 22 Mar 2019
  by Tom Igoe
*/

#include <SPI.h>
#include <WiFi101.h>
#include <RTCZero.h>
#include "arduino_secrets.h"

WiFiServer server(80);
RTCZero rtc;

void setup() {
  rtc.begin();
  Serial.begin(9600);// initialize serial communications
  // while you're not connected to a WiFi AP:
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);
    WiFi.begin(SECRET_SSID, SECRET_PASS); //   try to connect
    delay(2000);       // wait 2 seconds before trying again
  }

  // print out the device's network status
  Serial.print("To see this device's web interface, go to http://");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // get the time from the network:
  unsigned long epoch;
  do {
    Serial.println("Trying to get time...");
    epoch = WiFi.getTime();
    delay(500);
  } while (epoch == 0);

  // print the epoch once you have it:
  Serial.print("Epoch received: ");
  Serial.println(epoch);
  rtc.setEpoch(epoch);

  // print the time:
  Serial.println(getTimeString());

  server.begin();      // When you're connected, start the server
}

void loop() {
  String response;
  
  // listen for incoming clients
  WiFiClient client = server.available();
  
  // if there's a client, get the IP address and set up a response string:
  if (client) {
    response = getTimeString();
    Serial.print("request from: ");
    Serial.println(client.remoteIP());
  }
  
  while (client.connected()) {    // while the client is connected,
    if (client.available()) {     // and there are incoming bytes to read,
      // read the incoming line by line:
      String request = client.readStringUntil('\n');
      Serial.println(request);            // print the line

      // if it's a GET request for rubeGoldberg:
      if (request.startsWith("GET /rubeGoldberg")) {
        response += "\nHi Rube";
      }
      // if the request is a blank line (\n or \r\n)
      // then you're in the body of the request:
      if (request.length() <= 2) {
        client.println("HTTP 200 OK\n"); // send an HTTP response
        client.println("\n"); // send an HTTP response
        client.println(response);

        delay(10);
        // give the client time to get the data
        if (client.connected()) {   // if the client's still connected
          client.stop();            // disconnect the client
        }
      }
    }
  }
}


// get the time as an ISO8601 string e.g. 2019-03-22T12:25:16+0000):
String getTimeString() {
  String now = "20";
  if (rtc.getYear() < 9) now += "0";
  now += rtc.getYear();
  now += "-";
  if (rtc.getMonth() < 9) now += "0";
  now += rtc.getMonth();
  now += "-";
  if (rtc.getDay() < 9) now += "0";
  now += rtc.getDay();
  now += "T";
  if (rtc.getHours() < 9) now += "0";
  now += rtc.getHours();
  now += ":";
  if (rtc.getMinutes() < 9) now += "0";
  now += rtc.getMinutes();
  now += ":";
  if (rtc.getSeconds() < 9) now += "0";
  now += rtc.getSeconds();
  now += "+0000";
  return now;
}
