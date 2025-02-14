/*
  Test Client for WPA2-enterprise on ESP32

  Tested with Nano ESP32 on eduroam at 
  Cornell Tech, Aalborg University Copenhagen,
  NYU.
  Note that the WiFi radio on the ESP uses 2.4GHz,
  so you should check to see if your institution's 
  version of eduroam still implements 2.4Ghz. At some institutions, for example,
  there is a separate network called eduroam-2.4Ghz. 

  Here's what your arduino_secrets.h file should look like:
  #define EAP_IDENTITY ""      // use identity@institution.domain in for most institutions 
  #define EAP_USERNAME ""      // usually just a repeat of the identity
  #define EAP_PASSWORD ""      // your  password
  #define SECRET_SSID "eduroam"    // eduroam SSID
  
  modified 14 Feb 2025
  by Tom Igoe
*/

#include <WiFi.h>      //Wifi library
#include "esp_wpa2.h"  //wpa2 library for connections to Enterprise networks
#include "arduino_secrets.h"

void setup() {
  if (!Serial) delay(3000);  // wait for serial monitor to openZ
  Serial.begin(9600);        // initialize serial communication
  connectToNetwork();
}
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToNetwork();
    return;
  }
  // print every three seconds:
  if (millis() % 3000 < 2) {
    Serial.print("Connected to: ");
    Serial.print(SECRET_SSID);  // print the network name (SSID)
    Serial.print("\tWiFi signal strength: ");
    Serial.print(WiFi.RSSI());  // print the network name (SSID)
    Serial.print(" dBm\t");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

void connectToNetwork() {
  WiFi.begin(SECRET_SSID, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);

  // while you're not connected to a WiFi AP,
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);  // print the network name (SSID)
    delay(2000);
  }

  // When you're connected, print out the device's network status:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
