/*
  Test HTTPS Client
  Based on the Test HTTP Client from Making Things Talk, 3rd ed.

  makes an HTTPS call and displays the result on an SSD1306 OLED
  Uses the following libraries:
  http://librarymanager/All#WiFiNINA
  http://librarymanager/All#ArduinoHttpClient
  http://librarymanager/All#Adafruit_SSD1306

  created 12 Feb 2018
  modified 11 Jan 2021
  by Tom Igoe
*/

// include required libraries and config files
#include <WiFiNINA.h>           // use this  for MKR1010/Nano 33 IoT modules
//#include <WiFi101.h>          // use this  for MKR1000/WINC1500 modules
//#include <ESP8266WiFi.h>      // use this for ESP8266 modules
#include <ArduinoHttpClient.h>
#include <Adafruit_SSD1306.h>
#include "arduino_secrets.h"    // tab for the SSID and password

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    0  // Reset pin for display (0 or -1 if no reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiSSLClient netSocket;                  // network socket to server
const char server[] = "www.example.com";  // server name
String route = "/";                       // API route
int portNumber = 443;

void setup() {
  Serial.begin(9600);               // initialize serial communication
  // if the display doesn't work, quit:
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display setup failed");
    return;
  }
  // display is good, use it:
  // set the text color to white:
  display.setTextColor(SSD1306_WHITE);

  // while you're not connected to a WiFi AP,
  while ( WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Connecting to:");
    display.println(SECRET_SSID);           // print the network name (SSID)
    display.display();
    WiFi.begin(SECRET_SSID, SECRET_PASS);  // try to connect
    delay(2000);
  }

  // When you're connected, print out the device's network status:
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(SECRET_SSID);
  display.println(WiFi.localIP());
  display.display();
}

void loop() {
  HttpClient http(netSocket, server, portNumber); // make an HTTP client
  http.get(route);                                // make a GET request

  // update display:
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("requesting ");
  display.print(route);
  display.display();

  while (http.connected()) {              // while connected to the server,
    if (http.available()) {               // if there is a response from the server,
      String result = http.readString();  // read it
      // and print it:
      if (result.length() > 0) {
        display.setCursor(0, 0);
        display.clearDisplay();
        display.print(result);
        display.display();
      }
    }
  }
  // when there's nothing left to the response,
  http.stop();                     // close the request
  delay(10000);                    // wait 10 seconds
}
