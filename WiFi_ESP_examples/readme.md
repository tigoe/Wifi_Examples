# WiFi Examples for ESP32 Boards

The examples listed below were written originally for the Nano ESP32 board, which has an ESP32-S3 processor, but they have also been tested on the Seeed Xiao ESP3C3 board. They all use the ESP32 WiFi library, which is installed in the Arduino IDE when you install the Espressif [ESP32-Arduino boards](https://github.com/espressif/arduino-esp32) definition. Installing the Nano ESP32 board also installs this library. 

The [Uno R4 WiFi board](https://docs.arduino.cc/hardware/uno-r4-wifi/) also has an ESP32-S3 on board for WiFi connectivity, and uses a variant on the ESP32 WiFi library, called
`WiFiS3.h`. 

## WPA2 Enterprise Examples, and Eduroam
One of the key differences between the ESP32 WiFi library and the [WiFiNINA](https://docs.arduino.cc/libraries/wifinina/) and [WiFi101](https://docs.arduino.cc/libraries/wifi101/) libraries for other Arduino boards is that the ESP32 library includes support for enterprise WPA2 networks, using the `esp_wpa2.h` library. You can see this in action in the [HttpClientSimple_ESP_eduroam](HttpClientSimple_ESP_eduroam) and [WiFi_ESP_examples/HttpsClientSimple_ESP_eduroam](WiFi_ESP_examples/HttpsClientSimple_ESP_eduroam) examples, which show you how to connect to [Eduroam](https://eduroam.org/), a common WPA2 enterprise in many univerities. If you're a student or university employee, the big advantage of Eduroam is that you can use your school login credentials at other universities. This means that the same microcontroller device can run on your school's network via eduroam, or another school's. 

## HTTPS Examples
Another difference between the WiFiNINA and WiFi101 libraries and the ESP32 WiFi libraries is how they support secure socket layer (SSL) connections for HTTP requests. For background on what this means, see this [explainer article]( https://www.cloudflare.com/learning/ssl/how-does-ssl-work/) on how SSL works. The WiFiNINA and WiFi101 libraries rely on the [WiFiSSLClient](https://github.com/arduino-libraries/WiFiNINA/blob/master/src/WiFiSSLClient.h) class to create SSL clients. This client doesn't require you to obtain a website's root CA certificate in advance. By contrast, the ESP32 WiFi library uses the [WiFiClientSecure](https://github.com/espressif/arduino-esp32/blob/master/libraries/NetworkClientSecure/src/WiFiClientSecure.h) class, part of the ESP32 [NetworkClientSecure](https://github.com/espressif/arduino-esp32/tree/master/libraries/NetworkClientSecure) library, to handle SSL clients, and it requires you to supply the root certificate for the site to which you want to connect. 

In the examples here, [HttpsClientSimple_ESP](HttpsClientSimple_ESP) and [HttpsClientSimple_ESP_eduroam](HttpsClientSimple_ESP_eduroam), a root certificate for [www.example.com](https://www.example.com) is used, but you can replace it with the root certificate for another site by going to [ssl.org](https://www.ssl.org/), and entering the URL.  From the resulting certificates, choose the root CA. 


