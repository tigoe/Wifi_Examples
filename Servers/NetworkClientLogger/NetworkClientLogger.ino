/*
  Network Client Logger

  Listens for clients on popular web ports and logs their IP
  addresses to an SD file, along with the time and date
  Uses the following libraries:
  http://librarymanager/All#WiFiNINA
  http://librarymanager/All#ArduinoHttpClient
  http://librarymanager/All#RTCZero
  http://librarymanager/All#SD

  created 18 April 2019
  modified 17 Jan 2021
  by Tom Igoe
*/

//#include <WiFi101.h>    // use this for the MKR1000
#include <WiFiNINA.h>     // use this for the MKR1010 and Nano 33 IoT
#include <SD.h>
#include <RTCZero.h>
#include "arduino_secrets.h"

// The chip select pin.
// For MKRZero it's SDCARD_SS_PIN 
// for MKR mem shield, it's 4.
// for Nano 33 IoT, it's 10
const int  SD_CHIP_SELECT = 4;

// whether or not the SD card initialized:
bool SDAvailable = false;
// name of the file to write to:
String logFile = "DATALOG.CSV";

RTCZero rtc;
unsigned long startTime;

// number of times the device has reconnected
int reconnects = 0;
// the ports to listen on:
WiFiServer http(80);      // HTTP
WiFiServer https(443);    // HTTPS
WiFiServer httpAlt(8080); // an alternate development port for HTTP
WiFiServer ssh(22);        // SSH
WiFiServer smtp(25);      // SMTP
WiFiServer smtps(587);    // SMTPS

void setup() {
  Serial.begin(9600);
  // wait 5 seconds if serial monitor is not open:
  if (!Serial) delay(3000);
  // use the builtin LED to tell when a connection attempt happens:
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize the realtime clock:
  rtc.begin();

  SDAvailable = SD.begin(SD_CHIP_SELECT);
  // send serial only when serial monitor is open:
  if (Serial) Serial.println("Card working: " + String(SDAvailable));
  // open the log file on the SD Card:
  File dataFile = SD.open(logFile, FILE_WRITE);
  // print a header to the SD card file:
  if (dataFile) {
    dataFile.print("WiFi firmware version: ");
    dataFile.println(WiFi.firmwareVersion());
    dataFile.close();
  }

  // connect to network and start listening on ports:
  connectToNetwork();
  http.begin();
  https.begin();
  httpAlt.begin();
  ssh.begin();
  smtp.begin();
  smtps.begin();
}

void loop() {
  // listen for incoming connections:
  getClient(http, 80);
  getClient(https, 443);
  getClient(httpAlt, 8080);
  getClient(ssh, 22);
  getClient(smtp, 25);
  getClient(smtps, 587);
}

void getClient(WiFiServer server, int myPort) {
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    digitalWrite(LED_BUILTIN, HIGH);        // turn on LED

    // read incoming message from client:
    String message = client.readString();
    // make a log entry with it:
    String logEntry = getDateStamp();
    logEntry += ",";
    logEntry += getTimeStamp();
    logEntry += "\nincoming port: ";
    logEntry += String(myPort);
    logEntry += "ipAddr: ";
    logEntry += client.remoteIP();
    logEntry += ":";
    logEntry += client.remotePort();
    logEntry += "\nmessage: ";
    logEntry += message;

    // write the message to an SD card:
    if (SDAvailable) {
      File dataFile = SD.open(logFile, FILE_WRITE);
      if (dataFile) {
        dataFile.println(logEntry);
        dataFile.close();
      }
      // if SD card is not available but Serial Monitor
      // is, write to it instead:
    } else {
      if (Serial) Serial.println(logEntry);
    }
    client.print("Hello, ");
    client.print(client.remoteIP());
    client.print("The time is: ");
    client.println(getTimeStamp());
    client.println();
    // close the connection:
    client.stop();
    digitalWrite(LED_BUILTIN, LOW);
  }
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
  if (SDAvailable) {
    File dataFile = SD.open(logFile, FILE_WRITE);
    String logEntry = "connected to: ";
    logEntry += String(SECRET_SSID);
    logEntry += "\ntime: ";
    logEntry += getTimeStamp();
    logEntry += "\n";
    logEntry += "reconnects: ";
    logEntry += String(reconnects);

    if (dataFile) {
      dataFile.println(logEntry);
      dataFile.close();
    }
  }
  if (Serial) Serial.println(logEntry));
    if (Serial) Serial.println(getTimeStamp());
      IPAddress ip = WiFi.localIP();
      if (Serial) Serial.println(ip);
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
  return timestamp;
}

// format the date as dd-mm-yyyy:
String getDateStamp() {
  String datestamp = "";
  if (rtc.getDay() <= 9) datestamp += "0";
  datestamp += rtc.getDay();
  datestamp += "-";
  if (rtc.getMonth() <= 9) datestamp += "0";
  datestamp += rtc.getMonth();
  // add century:
  datestamp += "-20";
  if (rtc.getYear() <= 9) datestamp += "0";
  datestamp += rtc.getYear();
  return datestamp;
}
