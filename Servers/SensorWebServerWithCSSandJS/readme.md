# Sensor Web Server With CSS and JS

This example  ([link to code here](../SensorWebServerWithCSSandJS/)) shows how to make a web server program to serve sensor values. Unlike the [Uptime Web Server](../UptimeServer/), which only responds to one request, or the [Simpler Web Server](../SimplerWebServer/) example, which responds to two, this example shows how to parse and respond to multiple possible request. Specfically, it responds to a request for the root, `/`, or the index page, `index.html`. It also serves a `styles.css` page and a `script.js` page, and a custom request for `readings`. Using this approach, you can construct a more complex API for the server.


## The Arduino Sketch
The main loop is similar to the other two examples mentioned here. It listens for new clients, and when one is found, it processes it. When the client is no longer present, it disconnects like so:

````arduino
// wait for a new client:
WiFiClient client = server.available();

// when you get a new client:
if (client) {
  if (Serial) Serial.println("new client");
  // prepare a response string and content type:
  String response = "";
  String contentType = "text/";
  while (client.connected()) {
    if (client.available()) {
      // process the client's incoming request here
    }
  }
    // if the client is diconnected, close the connection:
  client.stop();
  if (Serial) Serial.println("client disconnected");
}
````
The centerpiece of the sketch is the way it handles incoming text from the client. Like the [Simpler Web Server](../SimplerWebServer/) example, it uses the String class' parsing functions. First, it reads the incoming request one line at a time, using the `.readStringUntil()` function to look for a newline (`\n`). If the resulting string is empty, ending only in a return character (`\r`) , you've reached the end of the request, so you process it, sending the HTTP response value, the content type, and any additional header characteristics, like `Connection: close`, like so:

````arduino
// read until you get a newline character
String input = client.readStringUntil('\n');
if (Serial) Serial.println(input);

if (input == "\r") {
  Serial.println("request finished");
  // send a standard HTTP response header
  client.println("HTTP/1.1 200 OK");
  // set the content type (HTML, CSS, or JS):
  client.println("Content-Type: " + contentType);
  // the connection will be closed after completion of the response
  client.println("Connection: close");
  client.println();
  // send the response and a blank line after:
  client.println(response);
  client.println();
  break;
}
````
There are four possible requests to handle: the index page, the styles.css page, the script.js, or the readings request. Each has a different content type (e.g. HTML, CSS, JS, or JSON), so you have to set the content type differently for each request. This is done like so:

````arduino
// check what the GET request is (always ends with /HTTP/1.1):
// the index page request:
if (input.endsWith("/ HTTP/1.1") || input.endsWith("/index.html HTTP/1.1")) {
  response = String(INDEXHTML);
  contentType += "html";
}
// the stylesheet request:
if (input.endsWith("/styles.css HTTP/1.1")) {
  response = String(STYLESCSS);
  contentType += "css";
}
// the javascript response:
if (input.endsWith("/script.js HTTP/1.1")) {
  response = String(SCRIPTJS);
  contentType += "js";
}
// a custom response that sends the readings as JSON:
if (input.endsWith("/readings HTTP/1.1")) {
  response = getReadings();
  contentType += "json";
}
````

Setting the actual response for the index, css, and js requests is done by populating the `response` string with values set in external header files, `index.h`, `styles.h`, and `script.h`. All of these are verbatim copies of a typical index.html, styles.css, or script.js file. To make them C-compatible, you wrap them quotation marks in a `#define` like so: 

````arduino
#define INDEXHTML ""
````
Then add a backslash line exender, `\`, to the end of each line. Make sure any single or double quotes are escaped with the backslash as well. You can see the results in [`index.h`](./index.h), [`styles.h`](./styles.h), and [`script.h`](./script.h). 

The response for `/readings` is a custom function that generates a JSON string that looks like this:

````json
{mcpTemp: 86, humidity: 38.3, dhtTemp: 83.48}
````
When the server responds to a requests for `/readings`, it sends this JSON as content type `text/json`. At this point, the Arduino sketch's work is done, and the rest is handled by the HTML and JavaScript that it served.

## The HTML
The HTML for this application is made up of a bunch of SPAN elements, some of which are labels for properties, and some of which hold the properties' values:
````html
<!DOCTYPE html> 
<html> 
<head> 
  <link rel="stylesheet" href="styles.css"> 
  <script src="script.js"></script> 
  <title>Temperature</title> 
</head> 
<body> 
    <span class="name">MCP9808 Temp: </span><span class="value" id="mcpTemp"></span><span class="units">&deg;F<br></span> 
    <span class="name">DHT22 Temp: </span><span class="value" id="dhtTemp"></span><span class="units">&deg;F</span><br> 
    <span class="name">DHT22 Humidity: </span><span class="value" id="humidity"></span><span class="units">%</span><br> 
</body> 
</html>
````

## The CSS
The stylesheet doesn't do much other than set the positions of the HTML SPAN elements:

````css
span.name { 
  position: sticky; 
  left: 20px; 
} 
span.value {  
  position:sticky;
  left: 150px;
}
span.units { 
  position:sticky;
  left: 200px;
}
````
## The Javascript
The JavaScript in `script.js` processes this on the server side. When the page loads in the browser, the script calls setup, which sets an interval function running every three seconds:

````js
function setup(event) { 
  console.log("begin");   
  setInterval(fetchJSON, 3000);   
} 
// handle the fetchJSON function here

window.addEventListener('DOMContentLoaded', setup);
````

When the interval function it calls, it uses the asynchronous JS fetch() to make an HTTP call for `readings`:

````js
function fetchJSON() {  
  fetch('/readings')    
    .then(response => response.json())    
    .then(data => getResponse(data))     
    .catch(error => getResponse(error));  
}

````
When it gets a response, it parses the JSON and looks for HTML elements with the same names as the JSON object's properties (in this case, `mcpTemp`, `humidity`, and `dhtTemp`). It fills those elements with each property's value:

````js
function getResponse(data) {   
  console.log(data);  
  for (property in data) {  
    let label = property;  
    let value = data[property]; 
    console.log(label);
    console.log(value); 
    if (document.getElementById(label) != null) {
    document.getElementById(label).innerHTML = value;
    }
  }
}
````
That's the whole application. The Arduino server isn't a high-volume server, as it doesn't multi-thread or handle simultaneous requests. But, by separating the HTML, CSS, JS, and custom requests, you can design your web app on a desktop or server, then quickly move it to the microcontroller when you're ready. Or you can serve all of it from a dedicated server except the sensor values, using mostly the same content files to do so. 