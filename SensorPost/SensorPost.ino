/*
  HTTPS POST client for ArduinoHttpClient library
  Connects to a server once every ten seconds,
  sends a POST request and a request body.

  Shows how to use Strings to assemble path and body

  note: WiFi SSID and password are stored in arduino_secrets.h file.

  Follow these instructions to add the server's SSL certificate
  to the radio's firmware:
  https://github.com/cvalenzuela/understanding-networks/blob/master/HTTPS.md

  created 13 Feb 2018
  by Tom Igoe
*/
#include <ArduinoHttpClient.h>
#include <WiFi101.h>
#include <RTCZero.h>
#include "arduino_secrets.h"

#define REQUEST_STRING "{\
\"macAddress\":\"currentMac\",\
\"sessionKey\":\"SECRET_SESSIONKEY\",\
\"data\": {\
\"time\":\"currentTime\",\
\"temp\":currentTemp,\
\"setpoint\": currentSetpoint\
}\
}"

const char serverAddress[] = "example.com";  // server address
int port = 443;
int count = 0;
RTCZero rtc;
WiFiSSLClient wifi;
HttpClient https = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;
int statusCode = 0;
String response;
byte mac[6];

void setup() {
  Serial.begin(9600);
  // try to connect to the network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);       // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);
  }

  // print the SSID of the network you're attached to:
  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());
  WiFi.macAddress(mac);
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  rtc.begin(); // initialize RTC
  unsigned long epoch = 0;// variable for time since 1970 in seconds
  int numberOfTries = 0;  // attempts to get network time
  int maxTries = 6;       // max times to try

  //try to set the time from the network:
  while ((epoch == 0) || (numberOfTries < maxTries))  {
    Serial.print("Trying to get network time ");
    Serial.println(numberOfTries);
    epoch = WiFi.getTime();
    numberOfTries++;
    delay(1000);
  }
  // if you failed
  if (epoch == 0) {
    Serial.print("NTP unreachable; couldn't get time");
    rtc.setTime(0, 0, 0); // fill in your hour, minute, second
    rtc.setDate(0, 0, 0); // fill in your day, month, year
  }
}

void loop() {
  // assemble the path for the POST message:
  String postData = String(REQUEST_STRING);
  String path = "/add";
  String contentType = "application/json";

  // assemble the body of the POST message:
  int sensorReading = analogRead(A0);
  float voltage = sensorReading *  (3.3 / 1024.0);
  // convert the millivolts to temperature celsius:
  float temperature = (voltage - 0.5) / 0.01;
  sensorReading = analogRead(A1);
  float setpoint = map(sensorReading, 0, 1023, 15, 25);
  postData.replace("currentMac", setMacAddress(mac));
  postData.replace("currentTime", setTimestamp());
  postData.replace("currentTemp", String(temperature));
  postData.replace("currentSetpoint", String(setpoint));
  Serial.println("making POST request");

  // send the POST request
  https.post(path, contentType, postData);
  Serial.println(postData);

  // read the status code and body of the response
  statusCode = https.responseStatusCode();
  response = https.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  Serial.println(count++);

  Serial.println("Wait ten seconds\n");
  delay(10000);
}

String setMacAddress(byte addr[]) {
  String result = "";
  for (int i = 0; i < 6; i++) {
    if (addr[i] < 9) {
      result += "0";
    }
    result += String(addr[i], HEX);
    if (i < 5) {
      result += ":";
    }
  }
  return result;
}

String setTimestamp() {
  //2015-03-25T12:00:00Z
  String result = "20";

  if (rtc.getYear() < 9) {
    result += "0";
  }
  result += String(rtc.getYear());
  result += "-";
  if (rtc.getMonth() < 9) {
    result += "0";
  }
  result += String(rtc.getMonth());
  result += "-";
  if (rtc.getDay() < 9) {
    result += "0";
  }
  result += String(rtc.getDay());
  result += "T";
  if (rtc.getHours() < 9) {
    result += "0";
  }
  result += String(rtc.getHours());
  result += ":";
  if (rtc.getMinutes() < 9) {
    result += "0";
  }
  result += String(rtc.getMinutes());
  result += ":";
  if (rtc.getSeconds() < 9) {
    result += "0";
  }
  result += String(rtc.getSeconds());
  result += "-5";
  return result;
}
