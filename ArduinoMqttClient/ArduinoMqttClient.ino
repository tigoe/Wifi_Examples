/*
  Shows how to make an MQTT client using SSL
  to connect to .

  For SSL You need to install/update the SSL certificates first:
  https://github.com/arduino-libraries/WiFi101-FirmwareUpdater#to-update-ssl-certificates
  Add the domain shiftr.io:443
  You can check on your device after a successful
  connection here: https://shiftr.io/try.

  created 11 Mar 2019
  by Tom Igoe
*/

#include <ArduinoMqttClient.h>
//#include <WiFi101.h>  // for MKR1000 
#include <WiFiNINA.h>   // for MKR 1010
#include "arduino_secrets.h"

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

char broker[] = "broker.shiftr.io";
int port = 1883;
char deviceName[] = "arduino";
char username[] = "try";
char password[] = "try";
String mainTopic = "sensorReading";
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

  // You can provide a unique client ID, if not set the library uses Arduin5jmn ,mko-millis()
  // Each client must have a unique client ID
  mqttClient.setId(deviceName);

  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword(username, password);
}

void loop() {
  // call poll() regularly to allow the library 
  // to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  if (!mqttClient.connected()) {
    connect();
  }

  // publish a message every five seconds:
  if (millis() - timestamp > 5000) {
    timestamp = millis();
    int sensorReading = analogRead(A0);

    String topic = mainTopic + "/potentiometer";
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print(sensorReading);
    mqttClient.endMessage();
  }
}


void connect() {
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  while (!mqttClient.connect(broker, port)) {
    delay(500);
  }

  // set the message receive callback
  mqttClient.onMessage(messageReceived);
  // subscribe to a topic
  mqttClient.subscribe(mainTopic + "/potentiometer");
  Serial.println("connected");
}

void messageReceived(int messageSize) {
  String payload = "";
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic: '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length: ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    payload = mqttClient.readString();
  }
  Serial.println("received: " + mqttClient.messageTopic() + "/" + payload);
}
