/*
  Dweet.io GET client for ArduinoHttpClient library
  Connects to dweet.io once every ten seconds,
  sends a GET request and a request body. Uses SSL

  Shows how to use Strings to assemble path and parse content
  from response. dweet.io expects:
  https://dweet.io/get/latest/dweet/for/thingName

  For more on dweet.io, see https://dweet.io/play/

  created 15 Feb 2016
  updated 22 Jan 2019
  by Tom Igoe

  this example is in the public domain
*/
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include <Arduino_JSON.h>

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
/////// Wifi Settings ///////
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char serverAddress[] = "192.168.0.100";  // server address
int port = 8080;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);     // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
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
  String path = "/temperature/";

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
