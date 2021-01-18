/*
  Connected Devices client

  Connects to a server using HTTPS and uploads data. This
  client works with the connected devices server API at
  https://github.com/don/itp-connected-devices

  For details, check connected devices class website.
  Works with MKR1010, MKR1000, ESP8266
  Uses the following libraries:
   http://librarymanager/All#WiFi101  // use this for MKR1000
   http://librarymanager/All#WiFiNINA  // use this for MKR1010, Nano 33 IoT
   http://librarymanager/All#ESP8266WiFi  // use this with ESP8266
   http://librarymanager/All#ArduinoHttpClient
   http://librarymanager/All#Arduino_JSON

  created 18 Feb 2019
  by Tom Igoe
*/
// include required libraries and config files
#include <SPI.h>
//#include <WiFi101.h>        // for MKR1000 modules
#include <WiFiNINA.h>         // for MKR1010 modules
//#include <ESP8266WiFi.h>    // for ESP8266 modules
#include <ArduinoHttpClient.h>
#include <Arduino_JSON.h>
#include "arduino_secrets.h"

byte mac[6];                    // mac address of your device
WiFiSSLClient netSocket;        // network socket to server
const char server[] = SECRET_SERVER;  // server name
String route = "/data";         // API route

// the HTTP client is global so you can use it in multiple functions below:
HttpClient client(netSocket, server, 443);
// a JSON variable for the body of your requests:
JSONVar body;

// request timestamp in ms:
long lastRequest = 0;
// interval between requests:
int interval = 10000;

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

  // add the mac address and session key to the body,
  // since they won't change:
  body["macAddress"] = macToString(mac);
  body["sessionKey"] = SECRET_SESSIONKEY;
}

void loop() {
  if (millis() - lastRequest > interval ) {

    // this is your data. It's a JSON object that will go
    // in the other JSON object (the body):
    JSONVar sensorData;
    // read sensor (in this case, a TMP36 temperature sensor):
    int sensorValue = analogRead(A0);
    // convert reading to voltage level:
    float voltage = sensorValue *  (3.3 / 1024.0);
    // convert the millivolts to temperature celsius:
    float temperature = (voltage - 0.5) / 0.01;
    sensorData["temperature"] = temperature;

    // use this for POST:
    postData(JSON.stringify(sensorData));
    //  or use this for GET:
    //  getData();
    int transactionID = 539;
    //  or use this to GET one datum:
    //  getDatum(transactionID);
    //  or use this to DELETE one datum:
    //deleteDatum(transactionID);

    // read the status code and body of the response
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

    // print out the response:
    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: " );
    Serial.println(response);
    // when there's nothing left to the response,
    client.stop();           // close the request
    // timestamp the time of request:
    lastRequest = millis();
  }
}

void postData(String newData) {
  // set the content type and fill in the POST data:
  String contentType = "application/json";
  // the template for the body of the POST request:
  body["data"] = newData;
  Serial.println(body);
  // make the request
  client.post(route, contentType, JSON.stringify(body));
}

void getData() {
  // set the content type and fill in the body:
  String contentType = "application/json";

  // make the request:
  client.beginRequest();
  client.get(route);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", body.length());
  client.beginBody();
  client.println(body);
  client.endRequest();
}

void getDatum(int newData) {
  // set the content type and fill in the body:
  String contentType = "application/json";
  // add the query to the route:
  String request = route + "/";
  request += String(newData);
  // make the request:
  client.beginRequest();
  client.get(request);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", body.length());
  client.beginBody();
  client.print(body);
  client.endRequest();
}



void deleteDatum(int newData) {
  // set the content type and fill in the body:
  String contentType = "application/json";
  // the template for the body of the POST request:
  String body = " {\"macAddress\":\"MAC\",\"sessionKey\":\"KEY\"}";
  // replace the template placeholders with actual values:
  body.replace("MAC", macToString(mac));
  body.replace("KEY", SECRET_SESSIONKEY);

  // add the query to the route:
  String request = route + "/";
  request += String(newData);
  // make the request:
  client.beginRequest();
  client.del(request);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", body.length());
  client.beginBody();
  client.print(body);
  client.endRequest();
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
