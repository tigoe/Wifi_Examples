
/*
  Connection time logger
  Logs how long a device has been connected to the network
  Writes to an SD card whenever device disconnects, and once a minute

  created 9 April 2019
  modified 2 May 2019
  by Tom Igoe
*/

#include <SPI.h>
#include <WiFi101.h>
//#include <WiFiNINA.h>
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
String logFile = "DATALOG.CSV";
// whether the SD card is accessible
bool SDAvailable = false;
// Whether it's time to write to the SD card:
volatile bool updateNeeded = false;

void setup() {
  // initialize serial and builtin LED:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // start realtime clock:
  rtc.begin();
  setDateFromCompile();
  setTimeFromCompile();

  // initialize SD card:
  SDAvailable = SD.begin(SD_CHIP_SELECT);
  // print to serial port to check if card is working:
  Serial.println("Card present: " + String(SDAvailable));
  Serial.println(getMacAddress(1));
  // if card is working, print to it. otherwise, print to serial:
  if (SDAvailable) {
    File dataFile = SD.open(logFile, FILE_WRITE);
    dataFile.println("Start date: " + getDateStamp());
    dataFile.println("Start time: " + getTimeStamp());
    dataFile.print("WiFi firmware version: ");
    dataFile.println(WiFi.firmwareVersion());
    dataFile.println(getColumnHeaders());
    dataFile.close();
  } else {
    Serial.println("Start date: " + getDateStamp());
    Serial.println("Start time: " + getTimeStamp());
    Serial.print("WiFi firmware version: ");
    Serial.println(WiFi.firmwareVersion());
    Serial.println(getColumnHeaders());
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
  // if SD card is reachable, write to it:
  if (SDAvailable) {
    // open SD card file:
    File dataFile = SD.open(logFile, FILE_WRITE);
    dataFile.print(getDateStamp() + ",");
    dataFile.print(getTimeStamp() + ",");
    dataFile.print(getMacAddress(1) + ",");
    dataFile.print(WiFi.SSID());
    dataFile.print(",");
    dataFile.print(getMacAddress(0) + "," );
    dataFile.print(WiFi.localIP());
    dataFile.print(",");
    dataFile.print(WiFi.RSSI());
    dataFile.print("dBm,");
    dataFile.print(statusLabel[WiFi.status()] + ",");
    dataFile.print(reconnects);
    dataFile.print(",");
    dataFile.println(getUptime(0));
    // close SD card file:
    dataFile.close();
  } else {
    Serial.print(getDateStamp() + ",");
    Serial.print(getTimeStamp() + ",");
    Serial.print(getMacAddress(1) + ",");
    Serial.print(WiFi.SSID());
    Serial.print(",");
    Serial.print(getMacAddress(0) + "," );
    Serial.print(WiFi.localIP());
    Serial.print(",");
    Serial.print(WiFi.RSSI());
    Serial.print("dBm,");
    Serial.print(statusLabel[WiFi.status()] + ",");
    Serial.print(reconnects);
    Serial.print(",");
    Serial.println(getUptime(0));
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


// set the rtc time from the compile time:
void setTimeFromCompile() {
  // get the compile time string:
  String compileTime = String(__TIME__);

  // break the compile time on the colons:
  int h = compileTime.substring(0, 2).toInt();
  int m = compileTime.substring(3, 5).toInt();
  int s = compileTime.substring(6, 8).toInt();

  // set the time from the derived numbers:
  rtc.setTime(h, m, s);
}

// set the rtc time from the compile date:
void setDateFromCompile() {
  String months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  // get the compile date:
  String compileDate = String(__DATE__);

  // get the date substring
  String monthStr = compileDate.substring(0, 3);

  int m = 0;    // variable for the date as an integer
  // see which month matches the month string:
  for (int i = 0; i < 12; i++) {
    if (monthStr == months[i]) {
      // the first month is 1, but its array position is 0, so:
      m = i + 1;
      // no need to continue the rest of the for loop:
      break;
    }
  }

  // get the day and year as substrings, and convert to numbers:
  int d = compileDate.substring(4, 6).toInt();
  int y = compileDate.substring(9, 11).toInt();

  // set the date from the derived numbers:
  rtc.setDate(d, m, y);
}
