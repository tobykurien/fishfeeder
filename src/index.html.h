#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"=====(
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>Fish Feeder</title>
</head>
<body>

<h1>Fish Feeder v1.0</h1>

<p>
<b>Time:</b> {{time}}<br/>
<b>Water temperature:</b> {{temperature}} °C<br/>
</p>

<p>
Last feedings: {{lastFeedings}}
</p>

<p>
<b>Feeding scheme:</b>
<select onchange="onSchemeChanged(this)">
    <option>Auto</option>
    <option>Once a week</option>
    <option>Twice a week</option>
    <option>Thrice a week</option>
    <option>Once a day</option>
    <option>Twice a day</option>
    <option>Thrice a day</option>
</select>
</p>

<p>
<b>Dumps per feeding:</b>
<select onchange="onDumpsChanged(this)">
    <option>One</option>
    <option>Two</option>
    <option>Three</option>
    <option>Four</option>
    <option>Five</option>
</select>
</p>

<p>
    <input type="button" value="Feed now" onclick="onFeed()"/>
</p>

<script>
    function onDumpsChanged(selectList) {
        alert('TODO');
    }

    function onSchemeChanged(selectList) {
        alert('TODO');
    }

    function onFeed() {
        makeRequest("/feed");
    }

    function makeRequest(url) {
        var oReq = new XMLHttpRequest();
        oReq.addEventListener("load", function() {
            alert(this.responseText);
        });
        oReq.open("GET", url);
        oReq.send();        
    }
</script>

</body>
</html>
)=====";