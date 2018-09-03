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
Last 5 feedings:
<ul>
    <li>{{feeding1}}</li>
    <li>{{feeding2}}</li>
    <li>{{feeding3}}</li>
    <li>{{feeding4}}</li>
    <li>{{feeding5}}</li>
</ul>
</p>

<p>
<b>Feeding scheme:</b>
<select onchange="alert('TODO')">
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
<select onchange="alert('TODO')">
    <option>One</option>
    <option>Two</option>
    <option>Three</option>
    <option>Four</option>
    <option>Five</option>
</select>
</p>

</body>
</html>
)=====";