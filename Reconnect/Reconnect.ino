/*
  WiFi reconnect for WiFi101 library

  This example shows how to check for connection
  continually and re-connect to a network 
  if you lose the connection.  When connected, 
  the example makes a HTTP request to http://www.example.com

  created 1 Mar 2018
  by Tom Igoe
*/
#include <WiFi101.h>
 // put your network SSID and password in 
 // a tab called arduino_secrets.h:
#include "arduino_secrets.h"   

WiFiClient http;       // client for http connection

void setup() {
  Serial.begin(9600); 
  connectToNetwork();  
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
  } else {
    if (!http.connected()) {
      makeHttpRequest();
    }
    // when the server responds, 
    while (http.available()) {          // if there is data available,
      String input = http.readString(); // read a line
      Serial.println(input);            // print it
    }
  }
  delay(10000);
}

void makeHttpRequest() {
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (http.connect("www.example.com", 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    http.println("GET / HTTP/1.1");
    http.println("Host: www.example.com");
    http.println("Connection: close");
    http.println();
  }
}
void connectToNetwork() {
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);       // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
  }

  // print the SSID of the network you're attached to:
  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

