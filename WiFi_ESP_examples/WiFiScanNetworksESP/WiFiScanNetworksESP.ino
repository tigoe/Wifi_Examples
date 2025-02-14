/*
 This example  prints the board's MAC address, and
 scans for available WiFi networks using the ESP32 module.
 Every ten seconds, it scans again. It doesn't actually
 connect to any network, so no encryption scheme is specified.

 Circuit:
 * Board with ESP32 module (Arduino nano ESP32)

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 2 Nov 2024
 by Tom Igoe
 */


#include <WiFi.h>
void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  // scan for existing networks:
  Serial.println("Scanning available networks...");
  listNetworks();
  delay(10000);
}

void listNetworks() {
  // print the MAC address:
  Serial.print("MAC: ");
  // print your MAC address. The ESP32 WiFi library has a function
  // that converts it to a string for you:
  String macAddr = WiFi.macAddress();
  Serial.println(macAddr);
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == 0) {
    Serial.println("Couldn't get a WiFi connection");
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(")\t");

    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tBSSID: ");
    String bssid = WiFi.BSSIDstr(thisNet);
    Serial.print(bssid);
    Serial.print("\t");
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tChannel: ");
    Serial.print(WiFi.channel(thisNet));
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void printEncryptionType(int thisType) {
  switch (WiFi.encryptionType(thisType)) {
    case WIFI_AUTH_OPEN:
      Serial.print("open");
      break;
    case WIFI_AUTH_WEP:
      Serial.print("WEP");
      break;
    case WIFI_AUTH_WPA_PSK:
      Serial.print("WPA");
      break;
    case WIFI_AUTH_WPA2_PSK:
      Serial.print("WPA2");
      break;
    case WIFI_AUTH_WPA_WPA2_PSK:
      Serial.print("WPA+WPA2");
      break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
      Serial.print("WPA2-EAP");
      break;
    case WIFI_AUTH_WPA3_PSK:
      Serial.print("WPA3");
      break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
      Serial.print("WPA2+WPA3");
      break;
    case WIFI_AUTH_WAPI_PSK:
      Serial.print("WAPI");
      break;
    default:
      Serial.print("unknown");
  }
  Serial.println();
}
