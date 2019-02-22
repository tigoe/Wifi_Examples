
/*
  Uptime server
  A mashup of Arturo Guadalupi's WiFiRTC example
  (https://www.arduino.cc/en/Tutorial/WiFiRTC)
  and the simple web server example
  (https://www.arduino.cc/en/Tutorial/Wifi101WiFiWebServer)

  modified 19 Feb 2018
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>

RTCZero rtc;

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

const int GMT = -5; //change this to adapt to to your time zone
unsigned long startTime;

void setup() {
  Serial.begin(9600);

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(2000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWiFiStatus();

  rtc.begin();

  unsigned long epoch;
  int numberOfTries = 0, maxTries = 6;
  do {
    Serial.print("Trying to get time, attempt number: ");
    Serial.println(numberOfTries);
    epoch = WiFi.getTime();
    numberOfTries++;
    delay(1000);
  }
  while ((epoch == 0) || (numberOfTries < maxTries));

  if (numberOfTries > maxTries) {
    Serial.print("NTP unreachable; couldn't get time");
    // stop forever
    while (1);
  }
  else {
    Serial.print("Epoch received: ");
    Serial.println(epoch);
    rtc.setEpoch(epoch);
    //set the epoch as the start time of the server:
    startTime = epoch;
  }
}

void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.print("current time: ");
          client.println(getNow());
          client.println("<br />");
          client.print("uptime: ");
          client.println(getUptime());
          client.println("<br />");

          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

String getUptime() {
  unsigned long upNow = rtc.getEpoch() - startTime;
  int upSecs = upNow % 60;
  int upMins = upNow % 3600L / 60;
  int upHours = upNow % 86400L / 3600;
  int upDays = upNow % 2073600L / 86400L;
  String uptime = format2Digits(upDays);
  uptime += " days, ";
  uptime += format2Digits(upHours);
  uptime += ":";
  uptime += format2Digits(upMins);
  uptime += ":";
  uptime += format2Digits(upSecs);
  return uptime;
}
String getNow() {
  String now = format2Digits(rtc.getDay() );
  now += "/";
  now += format2Digits(rtc.getMonth());
  now += "/";
  now +=  format2Digits(rtc.getYear());
  now += ",";
  now += format2Digits(rtc.getHours() + GMT);
  now += ":";
  now += format2Digits(rtc.getMinutes());
  now += ":";
  now +=  format2Digits(rtc.getSeconds());
  return now;
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

String format2Digits(int number) {
  String result = "";
  if (number < 10) {
    result += "0";
  }
  result += String(number);
  return result;
}
