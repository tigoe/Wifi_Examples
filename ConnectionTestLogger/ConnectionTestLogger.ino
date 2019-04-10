
/*
  Connection time logger
  Logs how long a device has been connected to the network
  Writes to an SD card whenever device disconnects, and once a minute

  created 9 April 2019
  by Tom Igoe
*/

#include <SPI.h>
//#include <WiFi101.h>
#include <WiFiNINA.h>
#include <RTCZero.h>
#include <SD.h>

#include "arduino_secrets.h"

#define SD_CHIP_SELECT 4

RTCZero rtc;
unsigned long startTime;

// Wifi status names:
String statusLabel[] = {
  "WL_IDLE_STATUS", "WL_NO_SSID_AVAIL", "WL_SCAN_COMPLETED", "WL_CONNECTED",
  "WL_CONNECT_FAILED", "WL_CONNECTION_LOST", "WL_DISCONNECTED",
  "WL_AP_LISTENING", "WL_AP_CONNECTED", "WL_AP_FAILED", "WL_PROVISIONING",
  "WL_PROVISIONING_FAILED"
};

// number of times the device has reconnected
int reconnects = 0;
// log file to write to on the SD card:
String logFile = "DATALOG.TXT";
// whether the SD card is accessible
bool SDAvailable = false;
// Whether it's time to write to the SD card:
volatile bool updateNeeded = false;

void setup() {
  // initialize serial and builtin LED:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize SD card:
  SDAvailable = SD.begin(SD_CHIP_SELECT);
  // print to serial port to check if card is working:
  Serial.println("Card present: " + String(SDAvailable));
Serial.println(getMacAddress());
  // if card is working, print to it. otherwise, print to serial:
  if (SDAvailable) {
    File dataFile = SD.open(logFile, FILE_WRITE);
    dataFile.print("WiFi firmware version: ");
    dataFile.println(WiFi.firmwareVersion());
    dataFile.println(getColumnHeaders());
    dataFile.close();
  } else {
    Serial.print("WiFi firmware version: ");
    Serial.println(WiFi.firmwareVersion());
    Serial.println(getColumnHeaders());
  }
  // start realtime clock:
  rtc.begin();
  // connect:
  connectToNetwork();
  // set alarm for one-minute updates:
  rtc.setAlarmTime(0, 0, 0);
  rtc.enableAlarm(rtc.MATCH_SS);
  rtc.attachInterrupt(timedLog);
}

void loop() {
  // if not connected, attempt to connect:
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    connectToNetwork();
  }

  // if update flag is set, log:
  if (updateNeeded) {
    // if SD card is reachable, write to it:
    if (SDAvailable) {
      // open SD card file:
      File dataFile = SD.open(logFile, FILE_WRITE);
      dataFile.print(getDateStamp() + ",");
      dataFile.print(getTimeStamp() + "\t");
      dataFile.print(getMacAddress() + "\t");
      dataFile.print(WiFi.SSID());
      dataFile.print("\t");
      dataFile.print(WiFi.localIP());
      dataFile.print("\t");
      dataFile.print(WiFi.RSSI());
      dataFile.print("dBm\t");
      dataFile.print(statusLabel[WiFi.status()] + "\t");
      dataFile.print(reconnects);
      dataFile.print("\t");
      dataFile.println(getUptime(0));
      // close SD card file:
      dataFile.close();
    } else {
      Serial.print(getDateStamp() + ",");
      Serial.print(getTimeStamp() + "\t");
      Serial.print(getMacAddress() + "\t");
      Serial.print(WiFi.SSID());
      Serial.print("\t");
      Serial.print(WiFi.localIP());
      Serial.print("\t");
      Serial.print(WiFi.RSSI());
      Serial.print("dBm\t");
      Serial.print(statusLabel[WiFi.status()] + "\t");
      Serial.print(reconnects);
      Serial.print("\t");
      Serial.println(getUptime(0));
    }
    // clear update flag:
    updateNeeded = false;
  }
}

// this is the RTC interrupt service routine. 
// it's called once a minute:
void timedLog() {
  updateNeeded = true;
}

// format MAC address:
String getMacAddress() {
  String result = "";
  byte mac[6];
  WiFi.macAddress(mac);

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

// format uptime:
String getUptime(int format) {
  unsigned long upNow = rtc.getEpoch() - startTime;
  int upSecs = upNow % 60;
  int upMins = (upNow % 3600L) / 60;
  int upHours = (upNow % 86400L) / 3600;
  int upDays = (upNow % 31556926L) / 86400L;
  String uptime = format2Digits(upDays);
  if (format == 0) { // short
    uptime += ":";
    uptime += format2Digits(upHours);
    uptime += ":";
    uptime += format2Digits(upMins);
    uptime += ":";
    uptime += format2Digits(upSecs);
  } else { // long

    uptime += " days, ";
    uptime += format2Digits(upHours);
    uptime += ": ";
    uptime += format2Digits(upMins);
    uptime += ": ";
    uptime += format2Digits(upSecs);
  }
  return uptime;
}

// format digit string:
String format2Digits(int number) {
  String result = "";
  if (number < 10) {
    result += "0";
  }
  result += String(number);
  return result;
}

// format the date as dd-mm-yyyy
String getDateStamp() {
  String dateStamp = "";
  if (rtc.getDay() <= 9) dateStamp += "0";
  dateStamp += rtc.getDay();
  dateStamp += "-";
  if (rtc.getMonth() <= 9) dateStamp += "0";
  dateStamp += rtc.getMonth();
  dateStamp += "-20";
  dateStamp += rtc.getYear();
  return dateStamp;
}

// format the time as hh:mm:ss
String getTimeStamp() {
  String timestamp = "";
  if (rtc.getHours() <= 9) timestamp += "0";
  timestamp += rtc.getHours();
  timestamp += ":";
  if (rtc.getMinutes() <= 9) timestamp += "0";
  timestamp += rtc.getMinutes();
  timestamp += ":";
  if (rtc.getSeconds() <= 9) timestamp += "0";
  timestamp += rtc.getSeconds();
  timestamp += " UTC";
  return timestamp;
}


void connectToNetwork() {
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  // You're connected, turn on the LED:
  digitalWrite(LED_BUILTIN, HIGH);
  
  // set the time from the network:
  unsigned long epoch;
  do {
    epoch = WiFi.getTime();
    delay(1000);
  } while (epoch == 0);

  rtc.setEpoch(epoch);
  
  //set the startTime as the connect time of the server:
  startTime = rtc.getEpoch();
  // increment the reconnect count:
  reconnects++;
  //  set the update flag:
  updateNeeded = true;
}

// return column headers for printing:
String getColumnHeaders() {
  String result = "DateTime: ";
  result += "\t";
  result += "MAC address: ";
  result += "\t";
  result += "SSID: ";
  result += "\t";
  result += "IP Address: ";
  result += "\t";
  result += "signal strength (RSSI): ";
  result += "\t";
  result += "status: ";
  result += "\t";
  result += "reconnects: ";
  result += "\t";
  result += "uptime: ";
  return result;
}
