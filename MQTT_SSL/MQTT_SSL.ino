/*
  Shows how to make an MQTT client using SSL
  to connect to .

  IMPORTANT: You need to install/update the SSL certificates first:
  https://github.com/arduino-libraries/WiFi101-FirmwareUpdater#to-update-ssl-certificates
  Add the domain shiftr.io:443
  You can check on your device after a successful
  connection here: https://shiftr.io/try.

  modified from an example by Gilberto Conti
  https://github.com/256dpi/arduino-mqtt
  updated 4 Mar 2018
  by Tom Igoe
*/

//#include <WiFi101.h>
#include <WiFiNINA.h>
#include <MQTTClient.h>
// put your network SSID and password in
// a tab called arduino_secrets.h:
#include "arduino_secrets.h"

WiFiSSLClient netConnection;
MQTTClient client;
char server[] = "broker.shiftr.io";
int port = 8883;
char device[] = "yourdevice";
char username[] = "try";
char password[] = "try";
String mainTopic = "yourTopic";
unsigned long timestamp = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  // try to connect to the network:
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(SECRET_SSID);       // print the network name (SSID);

    // Connect to WPA/WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }

  // print the SSID of the network you're attached to:
  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  client.begin(server, port, netConnection);
  client.onMessage(messageReceived);
  connect();
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message every five seconds:
  if (millis() - timestamp > 5000) {
    timestamp = millis();
    int sensorReading = analogRead(A0);
    String topic = mainTopic + "/potentiometer";
    client.publish(topic, String(sensorReading));
  }
}

void connect() {
  Serial.println("connecting...");
  while (!client.connect(device, username, password)) {
    delay(500);
  }
  Serial.println("connected");
  client.subscribe(mainTopic + "/+/temperature");
}


void messageReceived(String &topic, String &payload) {
  Serial.println("received: " + topic + "/" + payload);
}
