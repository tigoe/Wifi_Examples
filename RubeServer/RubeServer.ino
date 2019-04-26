/*
  Rube Goldberg web server

  When it gets an HTTP request with the right type of request
  for the right path, and with the right body, it starts a countdown
  until the launch time. Then it runs a launch function.

  The parts you care about are:
  void startMachine() {
   runs when the machine is supposed to start

change this for your location, always in UTC. If you need an epoch converter,
use https://www.epochconverter.com/ 

  Be sure to add a file arduino_secrets.h with:
  #define SECRET_SSID // your network SSID (name)
  #define SECRET_PASS // your network password
  #define SECRET_PATH // verb and path
  #define SECRET_BODY // string for the body of the request
  #define SECRET_LAUNCHTIME  // start time in UTC epoch
 
  created 12 April 2019
  modified 26 April 2019
  by Tom Igoe
*/

#include <SPI.h>
//#include <WiFi101.h>  // if you are using a MKR 1000
#include <WiFiNINA.h>   // or a MKR 1010
#include <SD.h>
#include "arduino_secrets.h"
#include <RTCZero.h>
#include <Servo.h>

// for the OLED display:
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "font.h"

WiFiServer server(80);  // make an instance of the server class
int reconnects = 0;     // how many times you've reconnected to network

// realtime clock:
RTCZero rtc;

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

const int bananaPin = 0;        // pin number for the banana servo
int bananaAngle = 0;            // banana starting angle
long lastServoMove = 0;         // last time the servo was updated
Servo bananaServo;              // servo instance for banana

// possible system statuses:
const int RUBE_WAITING = 0;       // waiting for HTTP request
const int RUBE_COUNTING = 1;      // counting down to end
const int RUBE_RUNNING = 2;       // running the local activity
const int RUBE_FINISHED = 3;      // finished
int systemStatus = RUBE_WAITING;

const int pushButton = 5;       // emergency pushbutton to trigger event

void setup() {

  // initialize banana servo:
  bananaServo.attach(bananaPin);
  bananaServo.write(bananaAngle);

  pinMode(LED_BUILTIN, OUTPUT);
  // pushbutton activates system in an emergency:
  pinMode(pushButton, INPUT_PULLUP);
  // trigger the start when the button goes low:
  attachInterrupt(digitalPinToInterrupt(pushButton), startMachine, FALLING);

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
  String statusMessage = "";
  // state machine, of sorts:
  switch (systemStatus) {
    case RUBE_WAITING:  // wait for trigger, display "waiting for request"
      statusMessage = "Waiting for signal";
      break;
    case RUBE_COUNTING:  // count down, display time left
      statusMessage = "T minus " + timeLeft();
      break;
    case  RUBE_RUNNING: // run servo
      statusMessage = "Machine running!";
      // increment banana angle, but constrain to 0-180:
      bananaAngle = constrain(bananaAngle++, 0, 180);
      // when you reach the end, stop the banana:
      if (bananaAngle == 180) {
        systemStatus = RUBE_FINISHED;
      }
      break;
    case RUBE_FINISHED: // display "event ended"
      statusMessage = "Event ended";
      break;

  }

  // show the status:
  displayWrite(statusMessage);

  // update the servo no matter what:
  if (millis() - lastServoMove > 20) {
    // write to the servo:
    bananaServo.write(bananaAngle);
    lastServoMove = millis();
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
    // display that you're responding to client:
    displayWrite("Responding to client");
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

          // if you're not already counting down:
          if (systemStatus != RUBE_COUNTING) {
            // enable the countdown:
            systemStatus = RUBE_COUNTING;
            // set alarm for launch time:
            rtc.setAlarmEpoch(SECRET_LAUNCHTIME);
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
      client.println("Status: " + statusMessage);

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
  String result = "";
  // get the diff between the SECRET_LAUNCHTIME as an epoch
  // and the current time as an epoch:
  long remaining = SECRET_LAUNCHTIME - rtc.getEpoch();
  if ( remaining < 0) {
    startMachine();
    systemStatus = RUBE_RUNNING;
  }
  // separate out days, hours, minutes, seconds:
  int days = remaining / 60 / 60 / 24;
  int hours = remaining / 60 / 60 % 24;
  int minutes = remaining / 60 % 60;
  int seconds = remaining % 60;
  // assemble a string:
  result += String(days) + ":";
  if (hours <= 9) result += "0";
  result += String(hours) + ":";
  if (minutes <= 9) result += "0";
  result += String(minutes)  + ":";
  if (seconds <= 9) result += "0";
  result += String(seconds);
  // return it:
  return result;
}

// this function runs when the machine is supposed to start:
void startMachine() {
  Serial.println("Go Rube! Go Rube! It's your birthday!");
  systemStatus = RUBE_RUNNING;
  bananaAngle = 0;
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
