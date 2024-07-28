/*
  This is the javaScript file. You can take a JavaScript, paste it here in quotes,
  add \ at the end of each line, escape the internal quotes with \, and you're done
*/

#define SCRIPTJS "function setup(event) { \ 
  console.log(\"begin\");  \ 
setInterval(fetchJSON, 3000);   \ 
}  \ 
function fetchJSON() {  \ 
  fetch(\'/readings\')   \ 
    .then(response => response.json())    \ 
    .then(data => getResponse(data))     \ 
    .catch(error => getResponse(error));  \ 
}  \ 
function getResponse(data) {   \ 
  console.log(data);  \ 
  for (property in data) {  \ 
    let label = property;  \ 
    let value = data[property];  \ 
    console.log(label);  \ 
    console.log(value);  \ 
    if (document.getElementById(label) != null) {  \ 
    document.getElementById(label).innerHTML = value;   \ 
    }  \ 
  }  \ 
}  \ 
window.addEventListener(\'DOMContentLoaded\', setup);"