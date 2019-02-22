/*
  Connected Devices client

  Connects to a server using HTTPS and uploads data.
  For details, check connected devices class website.
  Works with MKR1010, MKR1000, ESP8266

  created 18 Feb 2019
  by Tom Igoe
*/
// include required libraries and config files
#include <SPI.h>
#include <WiFi101.h>        // for MKR1000 modules
//#include <WiFiNINA.h>     // for MKR1010 modules
//#include <ESP8266WiFi.h>  // for ESP8266 modules
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"

byte mac[6];                       // mac address of your device
WiFiSSLClient netSocket;           // network socket to server
const char server[] = SECRET_SERVER;  // server name
String route = "/data";         // API route

// the HTTP client is global so you can use it in multiple functions below:
HttpClient client(netSocket, server, 443);

void setup() {
  Serial.begin(9600);              // initialize serial communication

  // while you're not connected to a WiFi AP, attempt to connect:
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);           // print the network name (SSID)
    WiFi.begin(SECRET_SSID, SECRET_PASS);  // try to connect
    delay(2000);
  }
  // get your MAC address:
  WiFi.macAddress(mac);

  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.print("MAC Address: ");
  Serial.println(macToString(mac));
}

void loop() {
  // read sensor (in this case, a TMP36 temperature sensor):
  int sensorValue = analogRead(A0);
  // convert reading to voltage level:
  float voltage = sensorValue *  (3.3 / 1024.0);
  // convert the millivolts to temperature celsius:
  float temperature = (voltage - 0.5) / 0.01;
  String sensorData = "{\"temperature\":";
  sensorData += String(temperature);
  sensorData += "}";
  // use this for POST:
  postData(sensorData);
  //  or use this for GET:
  //getData();

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: " );
  Serial.println(response);
  // when there's nothing left to the response,
  client.stop();           // close the request
  // stop forever
  while (true);
}

void postData(String newData) {
  // have to insert extra \ before " in data JSON,
  // since it's a JSON string within a JSON string:
  newData.replace("\"", "\\\"");
  // set the content type and fill in the POST data:
  String contentType = "application/json";
  // the template for the body of the POST request:
  String body = " {\"macAddress\":\"MAC\",\"sessionKey\":\"KEY\",\"data\":\"DATA\"}";
  // replace the template placeholders with actual values:
  body.replace("MAC", macToString(mac));
  body.replace("KEY", SECRET_SESSIONKEY);
  body.replace("DATA", newData);
  Serial.println(body);
  // make the request
  client.post(route, contentType, body);
}

void getData() {
  // the template for the query string of the GET request:
  String queryString = "?macAddress=MAC&sessionKey=KEY";
  // replace the template placeholders with actual values
  queryString.replace("MAC", macToString(mac));
  queryString.replace("KEY", SECRET_SESSIONKEY);

  // add the query to the route:
  String request = route + queryString;
  // make the request:
  client.get(request);
}

// convert the MAC address to a String:
String macToString(byte mac[]) {
  String result;
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      result += "0";
    }
    result += String(mac[i], HEX);
    if (i > 0)  result += ":";
  }
  return result;
}
