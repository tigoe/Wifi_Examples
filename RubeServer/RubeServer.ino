/*
  Rube Goldberg web server

  When it gets an HTTP request with the right type of request
  for the right path, and with the right body, it starts a countdown
  until the launch time. Then it runs a launch function.

  The functions you care about are:

  String timeLeft()
    runs once a second from when the server gets a good hit
    to when the machine is supposed to start

  void startMachine() {
   runs when the machine is supposed to start

  Be sure to add a file arduino_secrets.h with:
  #define SECRET_SSID // your network SSID (name)
  #define SECRET_PASS // your network password
  #define SECRET_PATH // verb and path
  #define SECRET_BODY // string for the body of the request

  created 12 April 2019
  by Tom Igoe
*/

#include <SPI.h>
//#include <WiFi101.h>  // if you are using a MKR 1000
#include <WiFiNINA.h>   // or a MKR 1010
#include <SD.h>
#include "arduino_secrets.h"
#include <RTCZero.h>

// for the OLED display:
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "font.h"

WiFiServer server(80);  // make an instance of the server class
bool counting = false;  // whether you're counting down or not
int reconnects = 0;     // how many times you've reconnected to network
int lastSecond = 0;     // the seconds last time through the loop

// realtime clock:
RTCZero rtc;
// start time in UTC (GMT) (change this for your location, always in UTC):
byte launch[] = {21, 30, 00};
// remaining countdown time:
byte countDown[] = {0, 0, 0};

// OLED display:
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// SD card:
const int SD_CHIP_SELECT = 4;
String logFile = "DATALOG.CSV";  // data file name
bool SDAvailable = false;        // SD card slot working?
bool displayAvailable = false;    // display working?
volatile bool logWritten = false;// log entry written?


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);// initialize serial communications

  // initialize the display library:
  displayAvailable = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.println("display working: " + String(displayAvailable));
  // initialize SD card:
  SDAvailable = SD.begin(SD_CHIP_SELECT);
  Serial.println("Card working: " + String(SDAvailable));
  // if either is not working, stop:
  while (!displayAvailable || !SDAvailable) {
    // stop and do nothing
  }

  // open the data file:
  File dataFile = SD.open(logFile, FILE_WRITE);
  // write a header to the file:
  if (dataFile) {
    dataFile.println("date:,time, client IP");
    dataFile.close();
  }

  // start realtime clock:
  rtc.begin();
  // connect:
  connectToNetwork();
  // When you're connected, start the server
  server.begin();

  // print out the device's network status
  Serial.print("To see this device's web interface, go to http://");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  Serial.println(getTimeStamp());
}

void loop() {
  // if you are counting down, display countdown:
  if (counting) {
    if (rtc.getSeconds() != lastSecond) {
      Serial.println(timeLeft());
      displayWrite("T minus " + timeLeft());
      lastSecond = rtc.getSeconds();
    }
    // if you are not counting down, display current time:
  } else {
    String statusString = "now: " + getTimeStamp();
    displayWrite(statusString);
  }
  // if you're not connected to Wifi, reconnect:
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    connectToNetwork();
  }

  // listen for incoming clients
  WiFiClient client = server.available();
  // while the client is connected,
  while (client.connected()) {
    // and there are incoming bytes to read,
    if (client.available()) {
      // print out client info:
      Serial.print("client address: ");
      Serial.println(client.remoteIP());

     // Log client address to the data file:
      File dataFile = SD.open(logFile, FILE_WRITE);
      // write a header to the file:
      if (dataFile) {
        dataFile.print(getDateStamp());
        dataFile.print(",");
        dataFile.print(getTimeStamp());
        dataFile.print(",");
        dataFile.println(client.remoteIP());
        dataFile.close();
      }
       
      // if it's the right kind of request and the right body:
      if (client.find(SECRET_PATH)) {
        if (client.find(SECRET_BODY)) {

          // if you're not already counthing down:
          if (!counting) {
            // enable the countdown:
            counting = true;
            // set alarm for launch time:
            rtc.setAlarmTime(launch[0], launch[1], launch[2]);
            rtc.enableAlarm(rtc.MATCH_HHMMSS);
            // attach the function to run at launch:
            rtc.attachInterrupt(startMachine);
            Serial.println("starting countdown");
          }
        }
      }
      // print a response back to the client:
      client.println("HTTP 200 OK\n"); // send an HTTP response
      client.println("\n\n"); // send an HTTP response
      client.print("Thanks. time at our end: "); // send an HTTP response
      client.println(getTimeStamp()); // send an HTTP response

      // wait a little before disconnecting them:
      delay(10);                       // give the server time to get the data
      if (client.connected()) {        // if the client's still connected
        client.stop();                 // disconnect the client
      }
    }
  }
}

void connectToNetwork() {
  displayWrite("connecting");
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    //Connect to WPA / WPA2 network:
    Serial.println("connecting to: " + String(SECRET_SSID));
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

  // increment the reconnect count:
  reconnects++;

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

// this function runs once a second from when the server
// gets a good hit to when the machine is supposed to start:

String timeLeft() {
  // calculate time left
  // if  hours are less than launch time:
  if (rtc.getHours() <= launch[0]) {
    countDown[0] = launch[0] - rtc.getHours();
  }
  // if  minutes are less than launch time:
  if (rtc.getMinutes() <= launch[1]) {
    countDown[1] = launch[1] - rtc.getMinutes();
  } else {
    countDown[1] = abs(launch[1] - (60 - rtc.getMinutes()));
  }
  // if  seconds are less than launch time:
  if (rtc.getSeconds() <= launch[2]) {
    countDown[2] = launch[2] - rtc.getSeconds();
  } else {
    countDown[2] = abs(launch[2] - (60 - rtc.getSeconds()));
  }
  // make a string with the countdown time:
  String result = "";
  for (int i = 0; i < 3; i++) {
    result += String(countDown[i]);
    if (countDown[i] < 10) result += "0";
    if (i < 2) result += ":";
  }
  return result;
}


// this function runs when the machine is supposed to start:
void startMachine() {
  Serial.println("Go Rube! Go Rube! It's your birthday!");
  counting = false;
}

// write a string to the OLED display:
void displayWrite(String message) {
  display.setFont(&Open_Sans_Light_12);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 12);
  display.println(WiFi.localIP());
  display.print(message);
  display.display();
}
