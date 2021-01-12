
/*
  WiFi Connection time logger
  Logs how long a device has been connected to the network
  Writes to an SD card whenever device disconnects, and once a minute
    Uses the following libraries:
  http://librarymanager/All#WiFiNINA
  or
  http://librarymanager/All#WiFi101
  http://librarymanager/All#RTCZero
  http://librarymanager/All#SD

  created 9 April 2019
  modified 11 Jan 2021
  by Tom Igoe
*/

#include <WiFiNINA.h>   // use this for MKR1010 or Nano 33 IoT
//#include <WiFi101.h>  // use this for MKR1000
#include <RTCZero.h>
#include <SD.h>
#include "arduino_secrets.h"

// chip select pin for the SD card reader:
#define SD_CHIP_SELECT 4

// realtime clock variables:
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
String logFile = "DATALOG.CSV";
// whether the SD card is accessible
bool SDAvailable = false;
// Whether it's time to write to the SD card:
volatile bool updateNeeded = false;

void setup() {
  // initialize serial and builtin LED:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  // if serial monitor is closed, wait 3 sec:
  if (!Serial) delay(3000);
  // start realtime clock:
  rtc.begin();

  // initialize SD card:
  //  SDAvailable = SD.begin(SD_CHIP_SELECT);
  // print to serial port to check if card is working:
  if (Serial) Serial.println("Card present: " + String(SDAvailable));
  if (Serial) Serial.println(getMacAddress(1));

  // if card is working, print to it. otherwise, print to serial:
  String header = "Start date: ";
  header += getDateStamp();
  header += "\nStart time: ";
  header += getTimeStamp();
  header += "\nWiFi firmware version: ";
  header += String(WiFi.firmwareVersion());
  header += "\n";
  header += getColumnHeaders();

  if (SDAvailable) {
    File dataFile = SD.open(logFile, FILE_WRITE);
    dataFile.println(header);
    dataFile.close();
  } else {
    if (Serial) Serial.println(header);
  }

  // connect:
  connectToNetwork();
  // set alarm for one-hour updates:
  rtc.setAlarmTime(0, 0, 0);
  rtc.enableAlarm(rtc.MATCH_MMSS);
  rtc.attachInterrupt(timedLog);
}

void loop() {
  // if not connected, attempt to connect:
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    logData();
    connectToNetwork();
  }

  // if update flag is set, log:
  if (updateNeeded) {
    logData();
  }
}

// this is the RTC interrupt service routine.
// it's called once a minute:
void timedLog() {
  updateNeeded = true;
}

void logData() {
  String logEntry = getDateStamp();
  logEntry += ",";
  logEntry += getTimeStamp();
  logEntry += ",";
  logEntry += getMacAddress(1);
  logEntry += ",";
  logEntry += WiFi.SSID();
  logEntry += ",";
  logEntry += getMacAddress(0);
  logEntry += ",";
  // IP address is a single unsigned int. Separate it into bytes:
  unsigned int ipAddr = WiFi.localIP();
  while  (ipAddr > 0) {
    logEntry += (ipAddr % 256);
    ipAddr /= 256;
    if (ipAddr > 0)  logEntry += ".";
  }
  logEntry += ",";
  logEntry += WiFi.RSSI();
  logEntry += "dBm,";
  logEntry += statusLabel[WiFi.status()];
  logEntry += ",";
  logEntry += reconnects;
  logEntry += ",";
  logEntry += getUptime(0);
  // if SD card is reachable, write to it:
  if (SDAvailable) {
    // open SD card file:
    File dataFile = SD.open(logFile, FILE_WRITE);
    dataFile.println(logEntry);
    // close SD card file:
    dataFile.close();
  } else {
    if (Serial) Serial.println(logEntry);
  }
  // clear update flag:
  updateNeeded = false;
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
  if (rtc.getYear() <= 9) dateStamp += "0";
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
    // increment the reconnect attempts:
    reconnects++;
  }

  // You're connected, turn on the LED:
  digitalWrite(LED_BUILTIN, HIGH);

  // set the time from the network:
  int attempts = 0;
  unsigned long epoch;
  do {
    epoch = WiFi.getTime();
    delay(1000);
    attempts++;
  } while (attempts < 5);

  rtc.setEpoch(epoch);

  //set the startTime as the connect time of the server:
  startTime = rtc.getEpoch();

  //  set the update flag:
  updateNeeded = true;
}

// return column headers for printing:
String getColumnHeaders() {
  String result = "Date:,Time:,";
  result += "MAC address:,";
  result += "SSID:,";
  result += "BSSID:,";
  result += "IP Address:,";
  result += "signal strength (RSSI):,";
  result += "status:,";
  result += "reconnects:,";
  result += "uptime:";
  return result;
}
