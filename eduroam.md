# Connecting to Eduroam

Many schools and universities have trouble using the WiFi libraries because their schools implement WPA2 enterprise WiFi networks. The ESP32 WiFi library has methods for connecting using this protocol and the Nano ESP32 works pretty well on them. 

## What is Eduroam?
Many schools implement the [eduroam network](https://eduroam.org/) which enables scholars to connect using their home institution's credentials at any other university that uses eduroam. To connect to eduroam, most universities use your school email address, e.g. `username@university.edu`, as your login, and your email password as your password. This makes it relatively easy to connect a WPA2 Enterprise-enabled Arduino at any university. 

To connect to eduroam, you need a few extra credntials: 
* your EAP identity - that's your eduroam login
* your EAP username - this is usually the same thing as your identity, but check with your institution to be sure
* EAP password - your password. 
* SSID - this is usually just `eduroam`, but some institutions set up separate 5GHz and 2.4GHz networks. If your school does this, the network name might be, for example, `eduroam-2.4Ghz`.

Make sure to NEVER EVER upload your credentials to a public server. On GitHub, a good solution is to keep a separate file called arduino_secrets.h in your Arduino project, and .gitignore that file, so that it's never uploaded. This is how all the examples in this repository work. 

Here's the arduino_secrets.h file for a typical eduroam setup: 

````arduino
#define EAP_IDENTITY ""      // use identity@institution.domain in for most institutions 
#define EAP_USERNAME ""      // usually just a repeat of the identity
#define EAP_PASSWORD ""   // your  password
#define SECRET_SSID "eduroam-2.4Ghz"    // eduroam SSID
````

To test this, use the sketch [WiFI_ESP_eduroam_connection](https://github.com/tigoe/Wifi_examples/tree/main/WiFi_Connection_Examples/WiFI_ESP_eduroam_connection). This sketch will connect, print out the board's IP address and the SSID, and then print the WiFi signal strength once every two seconds. Depending on the institution, you may find that it takes several attempts for the board to connect. 

Once you're connected, you can use this same basic approach for connecting to eduroam for any application. The following examples should work over eduroam:

* [HttpClientSimple_ESP_eduroam](https://github.com/tigoe/Wifi_examples/tree/main/WiFi_ESP_examples/HttpClientSimple_ESP_eduroam/HttpClientSimple_ESP_eduroam.ino) An HTTP client that connects to www.example.com
* [WifiUDP_OSC_Button_Client_ESP32_eduroam](https://github.com/tigoe/Wifi_examples/tree/main/WiFi_ESP_examples/WifiUDP_OSC_Button_Client_ESP32_eduroam/WifiUDP_OSC_Button_Client_ESP32_eduroam.ino) - an OSC client that connects to a remote OSC server when you press a button attached to digital pin 2



