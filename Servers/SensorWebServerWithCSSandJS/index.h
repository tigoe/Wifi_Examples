/*
  This is the HTML file. You can take a HTML file, paste it here in quotes,
  add \ at the end of each line, escape the internal quotes with \, and you're done
*/

#define INDEXHTML "<!DOCTYPE html> \
<html> \
<head> \
  <link rel=\"stylesheet\" href=\"styles.css\"> \
  <script src=\"script.js\"></script> \
  <title>Temperature</title> \
</head> \
<body> \
    <span class=\"name\">MCP9808 Temp: </span><span class=\"value\" id=\"mcpTemp\"></span><span class=\"units\">&deg;F<br></span> \
    <span class=\"name\">DHT22 Temp: </span><span class=\"value\" id=\"dhtTemp\"></span><span class=\"units\">&deg;F</span><br> \
    <span class=\"name\">DHT22 Humidity: </span><span class=\"value\" id=\"humidity\"></span><span class=\"units\">%</span><br> \
</body> \
</html>"