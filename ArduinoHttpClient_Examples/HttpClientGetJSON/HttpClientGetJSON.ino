/*
   GET client for ArduinoHttpClient library
  Connects to a server once every ten seconds,
  sends a GET request and parses the resulting JSON response.
  Uses SSL and the following libraries:
  http://librarymanager/All#ArduinoHttpClient
  http://librarymanager/All#WiFiNINA
  http://librarymanager/All#Arduino_JSON

  created 15 Feb 2016
  updated 11 Jan 2021
  by Tom Igoe
*/
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include <Arduino_JSON.h>
#include "arduino_secrets.h"

const char serverAddress[] = "arduino-to-json.glitch.me";  // server address
int port = 443;

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);     // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);
  }

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void loop() {
  // assemble the path for the GET message:
  String path = "/data/";

  // send the GET request
  Serial.println("making GET request");
  client.get(path);

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  // parse the string into a JSONVar object:
  JSONVar myObject = JSON.parse(response);

  // if you know the object's keys, you can ask for their values:
  Serial.print("temperature:\t");
  Serial.println(myObject["temperature"]);
  Serial.print("humidity:\t");
  Serial.println(myObject["humidity"]);

  // myObject.keys() can be used to get an array
  // of all the keys in the object
  JSONVar keys = myObject.keys();
  // read over the object, one key at a time:
  for (int i = 0; i < keys.length(); i++) {
    JSONVar value = myObject[keys[i]];
    // what's the key:
    Serial.print(keys[i]);
    Serial.print("\t");
    // and what's the value of the key:
    Serial.print(value);
    // and what's the type of the value:
    Serial.print("\ttype: ");
    Serial.println(JSON.typeof(value));
  }

  Serial.println("Wait ten seconds\n");
  delay(10000);
}
