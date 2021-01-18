/*
  Test HTTPS Client
  Based on the Test HTTP Client from Making Things Talk, 3rd ed.

  makes an HTTPS call and displays the result on an SSD1306 OLED
  Uses the following libraries:
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA    // use this for MKR1010 or Nano 33 IoT
  http://librarymanager/All#ArduinoHttpClient
  http://librarymanager/All#Adafruit_SSD1306

  created 12 Feb 2018
  modified 17 Jan 2021
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
// request timestamp in ms:
long lastRequest = 0;
// interval between requests:
int interval = 10000;

void setup() {
  Serial.begin(9600);               // initialize serial communication
  // if the display doesn't work, quit:
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display setup failed");
    return;
  }

  // while you're not connected to a WiFi AP,
  while ( WiFi.status() != WL_CONNECTED) {
    showMessage("connecting to: " + String(SECRET_SSID));
    WiFi.begin(SECRET_SSID, SECRET_PASS);  // try to connect
    delay(2000);
  }

  // When you're connected, print out the device's network status:
  String networkStatus = SECRET_SSID;
  // add a newline:
  networkStatus += "\n";
  // now get the IP address, which is a 4-byte array, as a String:
  unsigned int ip = WiFi.localIP();
  while  (ip > 0) {
    networkStatus += String(ip % 256);  // get the last byte as a String
    ip >> 8;                      // shift the higher bits down
    if (ip > 0)  networkStatus += ".";  // add a dot
  }
  showMessage(networkStatus);
}

void loop() {
  if (millis() - lastRequest > interval ) {
    HttpClient http(netSocket, server, portNumber); // make an HTTP client
    http.get(route);                                // make a GET request

    // update display:
    String requestStatus = "requesting " + route;
    showMessage(requestStatus);

    while (http.connected()) {              // while connected to the server,
      if (http.available()) {               // if there is a response from the server,
        String result = http.readString();  // read it
        // and print it:
        if (result.length() > 0) {
          showMessage(result);
        }
      }
    }
    // when there's nothing left to the response,
    http.stop();                     // close the request
    lastRequest = millis();
  }
}

// displays a string on the OLED:
void showMessage(String message) {
  // set the text color to white:
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.print(message);
  display.display();
}
