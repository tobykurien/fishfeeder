#include <Arduino.h>
#ifndef __INDEX_HTML__
#define __INDEX_HTML__

const char MONITOR_JSON[] PROGMEM = R"=====(
{
    "time": "{{time}}",
    "temperature": {{temperature}},
    "lastFeedings": [ {{lastFeedings}} ]
}
)=====";

const char INDEX_HTML[] PROGMEM = R"=====(
<html>

<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
    <title>Fish Feeder</title>

    <script src="markup.js"></script>
    <link rel="stylesheet" href="app.css">
</head>

<body>
    <script type="text/template" id="tpl_temperature">
        {{temperature}} °C
    </script>
    <script type="text/template" id="tpl_time">
        {{time}}
    </script>
    <script type="text/template" id="tpl_feedings">
        {{lastFeedings}}
        <tr>
            <td>{{time}}</td>
            <td>{{temperature}}</td>
            <td>{{feeds}}</td>
        </tr>
        {{/lastFeedings}}            
    </script>

    <div class="container">

        <p style="display: flex; justify-content: center">
            <b>Fish Feeder v1.0</b>
        </p>

        <div class="cards">

            <div class="cards-stacked">
                <div class="card">
                    <div class="card-header">
                        <div class="card-title h5">Temperature</div>
                    </div>
                    <div class="card-body" id="temperature"></div>
                </div>

                <div class="card">
                    <div class="card-header">
                        <div class="card-title h5">Time</div>
                    </div>
                    <div class="card-body" id="time"></div>
                    <div class="card-footer">
                        <button class="btn btn-primary" onclick="onSetTime()">Update time</button>
                    </div>
                </div>
            </div>

            <div class="card">
                <div class="card-header">
                    <div class="card-title h5">Configuration</div>
                </div>
                <div class="card-body">

                    <b>Feeding scheme:</b><br />
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
                        <b>Feed amount:</b><br />
                        <select onchange="onDumpsChanged(this)">
                            <option>One</option>
                            <option>Two</option>
                            <option>Three</option>
                            <option>Four</option>
                            <option>Five</option>
                        </select>
                </div>
                <div class="card-footer">
                    <button onclick="onFeed()" class="btn btn-primary">Feed now</button>
                </div>
            </div>
        </div>

        <p></p>

        <table class="table table-striped table-hover">
            <thead>
                <tr>
                    <th>Time</th>
                    <th>Water °C</th>
                    <th>Feeds</th>
                </tr>
            </thead>
            <tbody id="feedings"></tbody>
        </table>
    </div>

    <script src="app.js"></script>
</body>

</html>
)=====";

const char APP_JS[] PROGMEM = R"=====(
function onDumpsChanged(selectList) {
    alert('TODO');
}

function onSchemeChanged(selectList) {
    alert('TODO');
}

function onFeed() {
    makeRequest("/feed", function() {
        toast(this.responseText);
    });
}

function onSetTime() {
    let now = new Date();
    let params = "year=" + now.getFullYear() +
                 "&month=" + (now.getMonth() + 1) + 
                 "&day=" + now.getDate() + 
                 "&hour=" + now.getHours() + 
                 "&minute=" + now.getMinutes() + 
                 "&second=" + now.getSeconds();
    makeRequest("/set-time?" + params, function() {
        toast(this.responseText);
    });
}

function makeRequest(url, onComplete) {
    var oReq = new XMLHttpRequest();
    oReq.addEventListener("load", onComplete);
    oReq.open("GET", url);
    oReq.send();        
}

function updateMonitor() {
    makeRequest("/monitor", function() {
        var data = JSON.parse(this.response);

        templates = ["temperature", "time", "feedings"]
        for (i in templates) {
            let id = templates[i]
            var template = document.getElementById("tpl_" + id).innerHTML;
            var out = Mark.up(template, data);
            document.getElementById(id).innerHTML = out;
        }
    });
}

function toast(message) {
    console.log(message);
    alert(message);
}

setInterval(updateMonitor, 1000);
)=====";

#endif