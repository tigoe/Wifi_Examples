
/*
  RSSI test

  tests the signal strength on a given network. Prints out the RSSI and
  BSSID (aka MAC address) of each WiFi Access Point to which the device connects.
 Uses the following libraries:
  http://librarymanager/All#WiFiNINA
  or
  http://librarymanager/All#WiFi101

  created 8 Jan 2020
  updated 11 Jan 2021
  by Tom Igoe
*/


#include <WiFiNINA.h>   // use this for MKR1010 or Nano 33 IoT
//#include <WiFi101.h>  // use this for MKR1000

#include "arduino_secrets.h"

// number of times the device has reconnected
int reconnects = 0;
// Wifi status names:
String statusLabel[] = {
  "WL_IDLE_STATUS", "WL_NO_SSID_AVAIL", "WL_SCAN_COMPLETED", "WL_CONNECTED",
  "WL_CONNECT_FAILED", "WL_CONNECTION_LOST", "WL_DISCONNECTED",
  "WL_AP_LISTENING", "WL_AP_CONNECTED", "WL_AP_FAILED", "WL_PROVISIONING",
  "WL_PROVISIONING_FAILED"
};

void setup() {
  // initialize serial and builtin LED:
  Serial.begin(9600);
  
  // LED to indicate connection:
  pinMode(LED_BUILTIN, OUTPUT);
  
  // get the Arduino's MAC address:
  Serial.println(getMacAddress(1));
  
  // get the firmware version of the Arduino's radio:
  Serial.print("WiFi firmware version: ");
  Serial.println(WiFi.firmwareVersion());
  Serial.println();
  
  // connect:
  connectToNetwork();
}

void loop() {
  // if not connected, attempt to connect:
  if (WiFi.status() != WL_CONNECTED) {
  // turn off the connected LED:
    digitalWrite(LED_BUILTIN, LOW);
    connectToNetwork();
  }

// if signal strength drops below -80 dBm, disconnect:
  if (WiFi.RSSI() < -80) {
    Serial.println("Wifi too weak, disconnecting");
    WiFi.disconnect();
    connectToNetwork();
  }
// log current data:
  logData();
  delay(2000);
}


void logData() {
  // prints SSID, signal strength, and BSSID of the WAP
  // to which you are connected, and the number of reconnects
  // so far:
  Serial.print(WiFi.SSID());
  Serial.print("\t");
  Serial.print(getMacAddress(0) + "\t" );
  Serial.print(WiFi.RSSI());
  Serial.print("dBm\t");
  // current status of the WiFi radio, in words:
  Serial.print(statusLabel[WiFi.status()] + "\t" );
  Serial.println(reconnects);
}

// format MAC address:
String getMacAddress(int which) {
  String result = "";
  byte mac[6];
  if (which == 1 ) {       // get MAC address of this device
    WiFi.macAddress(mac);
  } else if (which == 0) {  // get MAC address of router
    WiFi.BSSID(mac);
  }
  for (int b = 5; b >= 0; b--) {
    if (mac[b] < 16) {
      result += "0";
    }
    result += String(mac[b], HEX);
    if (b > 0) {
      result += ":";
    }
  }
  return result;
}


void connectToNetwork() {
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
    // increment the reconnect attempts:
    reconnects++;
  }

  // You're connected, turn on the LED:
  digitalWrite(LED_BUILTIN, HIGH);
}
