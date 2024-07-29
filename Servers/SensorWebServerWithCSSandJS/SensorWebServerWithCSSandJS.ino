/*
  Web Server to serve temperature and humidity data

  This example uses the MCP9808 precision temperature sensor
  and the DHT22 temperature and humidity sensor
  to serve temperature and humidity data. It also shows how
  to parse client requests for the GET line, and to 
  separate server responses for multiple documents.
  Each document to be served (index.html, styles.css, script.js)
  is defined in a different #define, and stored in a separate
  file to make editing convenient. 

  MCP9808 sensor is attached to the I2C pins
  DHT22 sensor is attached to digital pin 2

  created 28 Jul 2024
  by Tom Igoe
*/

// include the sensor libraries and the WiFI library:
#include "Adafruit_MCP9808.h"
#include "DHT.h"
// this is the Nano 33 IoT library. Change to match your board:
#include <WiFiNINA.h>
// include the WiFi network SSID and password:
#include "arduino_secrets.h"
// include the content documents:
#include "index.h"
#include "styles.h"
#include "script.h"

// initialize the web server:
WiFiServer server(80);
// set the DHT22 sensor input pin:
const int DHTPin = 2;
// initialize the sensors:
DHT dhtSensor(DHTPin, DHT22);
Adafruit_MCP9808 MCPSensor = Adafruit_MCP9808();

void setup() {
  // initialize serial communications:
  Serial.begin(9600);
  // attempt to connect to WiFi:
  while (WiFi.status() != WL_CONNECTED) {
    if (Serial) Serial.print("Attempting to connect to Network named: ");
    if (Serial) Serial.println(SECRET_SSID);  // print the network name (SSID)
    WiFi.begin(SECRET_SSID, SECRET_PASS);     // try to connect
    delay(2000);
  }
  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  if (Serial) {
    Serial.print("IP Address: ");
    Serial.println(ip);
  }
  // start the server:
  server.begin();
  // start the MCP sensor:
  while (!MCPSensor.begin(0x18)) {
    if (Serial) Serial.println("sensor not found. Check connections.");
    delay(1000);
  }
  // Set the MCS sensor to 0.5°C resolution:
  MCPSensor.setResolution(0);
  MCPSensor.wake();
  // start the DHT22 sensor:
  dhtSensor.begin();
}

void loop() {
  // wait for a new client:
  WiFiClient client = server.available();

  // when you get a new client:
  if (client) {
    if (Serial) Serial.println("new client");
    // prepare a response string and content type:
    String response = "";
    String contentType = "text/";
    // while the client is connected, read the strings it sends:
    while (client.connected()) {
      if (client.available()) {
        // read until you get a newline character
        String input = client.readStringUntil('\n');
        if (Serial) Serial.println(input);
        // if you only get a return character, then you've reached the end
        // of the request. You can send a response:
        if (input == "\r") {
          if (Serial) Serial.println("request finished");
          // send a standard HTTP response header
          client.println("HTTP/1.1 200 OK");
          // set the content type (HTML, CSS, or JS):
          client.println("Content-Type: " + contentType);
          // the connection will be closed after completion of the response
          client.println("Connection: close");
          client.println();
          // send the response and a blank line after:
          client.println(response);
          client.println();
          break;
        }
        // trim any whitespace from the response:
        input.trim();
        // check what the GET request is (always ends with /HTTP/1.1):
        // the index page request:
        if (input.endsWith("/ HTTP/1.1") || input.endsWith("/index.html HTTP/1.1")) {
          response = String(INDEXHTML);
          contentType += "html";
        }
        // the stylesheet request:
        if (input.endsWith("/styles.css HTTP/1.1")) {
          response = String(STYLESCSS);
          contentType += "css";
        }
        // the javascript response:
        if (input.endsWith("/script.js HTTP/1.1")) {
          response = String(SCRIPTJS);
          contentType += "js";
        }
        // a custom response that sends the readings as JSON:
        if (input.endsWith("/readings HTTP/1.1")) {
          response = getReadings();
          contentType += "json";
        }
      }
    }
    // if the client is diconnected, close the connection:
    client.stop();
    if (Serial) Serial.println("client disconnected");
  }
}

// read all the sensors and send them as a JSON string:
String getReadings() {
  // get the MCP9808 reading:
  String text = "{\"mcpTemp\":";
  text += String(MCPSensor.readTempF(), 4);
  // get the DHT22 readings:
  text += ",\"humidity\":";
  float humidity = dhtSensor.readHumidity();
  text += String(humidity, 4);
  text += ",\"dhtTemp\":";
  float DhtTempF = dhtSensor.readTemperature(true);
  text += String(DhtTempF, 4);
  text += "}";
  return text;
}