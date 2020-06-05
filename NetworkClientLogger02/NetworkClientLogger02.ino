/*
  Network Client Logger

  Listens for clients on popular web ports and logs their IP
  addresses

  created 6 Sept 2019
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFi101.h>
//#include <WiFiNINA.h>
#include "arduino_secrets.h"

WiFiServer http(80);
WiFiServer https(443);
WiFiServer httpAlt(8080);
WiFiServer ssh(22);
WiFiServer smtp(25);
WiFiServer smtps(587);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  connectToNetwork();
  http.begin();
  https.begin();
  httpAlt.begin();
  ssh.begin();
  smtp.begin();
  smtps.begin();
}

void loop() {
  getClient(http, 80);
  getClient(https, 443);
  getClient(httpAlt, 8080);
  getClient(ssh, 22);
  getClient(smtp, 25);
  getClient(smtps, 587);
}

void getClient(WiFiServer server, int myPort) {
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    String message = client.readString();
    Serial.println("incoming port: " + String(myPort));
    Serial.print("ipAddr: ");
    Serial.print(client.remoteIP());
    Serial.print(":");
    Serial.println(client.remotePort());
    Serial.print("message: ");
    Serial.println(message);
    Serial.println();

    client.print("Hello, ");
    client.print(client.remoteIP());
    client.println();
    // close the connection:
    client.stop();
  }
}


void connectToNetwork() {
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  Serial.println("connected to: " + String(SECRET_SSID));
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}
