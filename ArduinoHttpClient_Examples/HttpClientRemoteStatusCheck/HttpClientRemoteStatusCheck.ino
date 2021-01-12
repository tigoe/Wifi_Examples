/*
  HTTP POST client for ArduinoHttpClient library with remote
  status check
  Connects to a server once every ten seconds,
  sends a POST request and a request body.

  Responds to UDP requests on port 43770 with
  the last request time and request count, so you can check status.

  To send a UDP message from the command line :
  $ echo "status" | nc -uc yourAddress 43770 -w3

  note: WiFi SSID and password are stored in arduino_secrets.h file.
  Uses the following libraries:
  http://librarymanager/All#ArduinoHttpClient
  http://librarymanager/All#WiFi101   // use this for MKR1000
  http://librarymanager/All#WiFiNINA    // use this for MKR1010 or Nano 33 IoT
  http://librarymanager/All#WiFiUdp
  http://librarymanager/All#RTCZero

  created 24 Feb 2018
  modified 11 Jan 2021
  by Tom Igoe
*/
#include <ArduinoHttpClient.h>
//#include <WiFi101.h>    // use this for MKR1000
#include <WiFiNINA.h>     // use this for MKR1010 or Nano 33 IoT
#include <WiFiUdp.h>
#include <RTCZero.h>
#include "arduino_secrets.h"

const char serverAddress[] = "example.com";  // server address
int port = 80;                  // HTTP request port
long requestCount = 0;          // count of requests so far
String lastRequestTime;         // ISO8601 timestring of last request

WiFiUDP Udp;                    // UDP instance
unsigned int udpPort = 43770;   // local port to listen on

RTCZero rtc;                    // Realtime clock
WiFiClient wifi;                // WiFi instance
int wiFiStatus = WL_IDLE_STATUS;  // WiFi status

// HTTP client instance:
HttpClient http = HttpClient(wifi, serverAddress, port);
bool requesting = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // try to connect to the network:
  while ( wiFiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);       // print the network name (SSID);
    // Connect to WPA/WPA2 network:
    wiFiStatus = WiFi.begin(SECRET_SSID, SECRET_PASS);
  }

  // print the SSID of the network you're attached to:
  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());

  // print your IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // set up a UDB server to listen for status requests:
  Serial.print("Listening for UDP on port ");
  Serial.println(udpPort);
  Udp.begin(udpPort);

  rtc.begin();            // initialize RTC
  unsigned long epoch = 0;// variable for time since 1970 in seconds
  int numberOfTries = 0;  // attempts to get network time
  int maxTries = 6;       // max times to try

  //try to set the time from the network:
  while ((epoch == 0) || (numberOfTries < maxTries))  {
    Serial.println("Trying to get network time ");
    epoch = WiFi.getTime();
    numberOfTries++;
    delay(1000);
  }
  // if you failed:
  if (epoch == 0) {
    Serial.print("NTP unreachable; couldn't get time");
    rtc.setTime(0, 0, 0); // fill in your hour, minute, second
    rtc.setDate(0, 0, 0); // fill in your day, month, year
  }
  
  //set an alarm for when to make a request:
  rtc.setAlarmSeconds(00);          // set an alarm for processor wakeup
  rtc.enableAlarm(rtc.MATCH_SS);    // enable it for once a minute
  rtc.attachInterrupt(requestCheck); // have the request happen on the alarm
}

void loop() {
  udpCheck();       // listen for incoming UDP
  if (requesting) { // if the interrupt function set this variable,
    sendRequest();  // then make a request
  }
}

void requestCheck() {
  // in an interrrupt function, you shouldn't do much, so
  // just set this variable to true:
  requesting = true;
}

void sendRequest() {
  digitalWrite(LED_BUILTIN, HIGH);
  // assemble the path for the POST message:
  int sensorReading = analogRead(A0);

  String postData = String(sensorReading);
  String path = "/sensor";
  String contentType = "application/text";

  // send the POST request
  Serial.println("making POST request");
  http.post(path, contentType, postData);

  // get the HTTP response status code:
  Serial.println(http.responseStatusCode());
  // read the headers
  while (!http.endOfHeadersReached()) { // while you're getting headers,
    if (http.headerAvailable()) {       // if there's one to read,
      // if there is a date:
      if (http.readHeaderName() == "Date") {
        // save it as the last request time:
        lastRequestTime = http.readHeaderValue();
      }
    }
  }

  // print the response body:
  Serial.print("Response: ");
  Serial.println(http.responseBody());
  // increment the request count:
  requestCount++;
  http.stop();
  digitalWrite(LED_BUILTIN, LOW);
  requesting = false;   // set requesting false to prevent multiple reads
}

void udpCheck() {
  // if there's data available, read a packet
  if (Udp.parsePacket() > 0) {        // parse incoming packet
    String message = "";
    Serial.print(" from: ");           // print the sender
    Serial.print(Udp.remoteIP());
    Serial.print(" on port: ");       // and the port they sent on
    Serial.println(Udp.remotePort());
    while (Udp.available() > 0) {     // parse the body of the message
      message = Udp.readString();
    }
    Serial.print("msg: " + message);  // print it
    String response = "";
    if (message == "status") {
      // if they ask for status
      response += "last request at: ";
      response += lastRequestTime;
      response += ", requests so far: ";
      response += requestCount;
    } else {              // otherwise send back what they sent
      response = message;
    }
    // start a response packet:
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.println(response);
    Udp.endPacket();                   // finish and send packet
  }
}
