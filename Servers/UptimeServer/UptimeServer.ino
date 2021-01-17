
/*
  Uptime server
  A mashup of Arturo Guadalupi's WiFiRTC example
  (https://www.arduino.cc/en/Tutorial/WiFiRTC)
  and the simple web server example
  (https://www.arduino.cc/en/Tutorial/Wifi101WiFiWebServer)

  modified 17 Jan 2021
  by Tom Igoe
*/

//#include <WiFi101.h>      // use this for MKR1000 boards
#include <WiFiNINA.h>       // use this for MKR1010 and Nano 33 IoT boards
#include <WiFiUdp.h>
#include <RTCZero.h>
#include "arduino_secrets.h"

RTCZero rtc;
WiFiServer server(80);

unsigned long startTime;
void setup() {
  Serial.begin(9600);
  printWiFiStatus();

  Serial.print("Attempting to connect to SSID: ");

  // attempt to connect to WiFi network:
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WiFi.status());
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);

    // wait 2 seconds for connection:
    delay(2000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWiFiStatus();
  // start the realtime clock
  rtc.begin();

  // try to get the network time:
  unsigned long epoch;
  do {
    Serial.println("Trying to get time");
    epoch = WiFi.getTime();
    delay(1000);
  } while (epoch == 0);

  Serial.print("Epoch received: ");
  Serial.println(epoch);
  rtc.setEpoch(epoch);
  //set the epoch as the start time of the server:
  startTime = epoch;
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
          client.println(" <!DOCTYPE HTML > ");
          client.println("<html>\n<head></head>\n<body>");
          client.print("Firmware version: ");
          client.println(WiFi.firmwareVersion());
          client.println(" <br /> ");
          client.print("RSSI: ");
          client.println(WiFi.RSSI());
          client.println(" <br /> ");
          client.print("current time: ");
          client.println(getNow());
          client.println(" <br /> ");
          client.print("uptime: ");
          client.println(getUptime(1));
          client.println(" <br /> ");

          client.println("</body></ html > ");
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

String getUptime(int format) {
  unsigned long upNow = rtc.getEpoch() - startTime;
  int upSecs = upNow % 60;
  int upMins = upNow % 3600L / 60;
  int upHours = upNow % 86400L / 3600;
  int upDays = upNow % 31556926L / 86400L;
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

String getNow() {
  String now = format2Digits(rtc.getDay() );
  now += " /";
  now += format2Digits(rtc.getMonth());
  now += " /";
  now +=  format2Digits(rtc.getYear());
  now += ", ";
  now += format2Digits(rtc.getHours());
  now += ":";
  now += format2Digits(rtc.getMinutes());
  now += ":";
  now +=  format2Digits(rtc.getSeconds());
  now += " UTC";
  return now;
}


void printWiFiStatus() {
  // print firmware version:
  Serial.print("WiFi firmware version: ");
  Serial.println(WiFi.firmwareVersion());
  // print MAC address:

  Serial.print("MAC address: ");
  byte mac[6];
  WiFi.macAddress(mac);
  for (int b = 5; b >= 0; b--) {
    if (mac[b] < 15) Serial.print("0");
    Serial.print(mac[b], HEX);
    if (b > 0) Serial.print(": ");
  }
  Serial.println();
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
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
