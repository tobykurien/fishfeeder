#include <Arduino.h>
#ifndef __INDEX_HTML__
#define __INDEX_HTML__

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
            <td>{{amount}}</td>
        </tr>
        {{/lastFeedings}}            
    </script>
    <script type="text/template" id="tpl_temperatures">
        {{temperatures}}
        <tr>
            <td>{{time}}</td>
            <td>{{temperature}}</td>
        </tr>
        {{/temperatures}}            
    </script>

    <div class="container">

        <p style="display: flex; justify-content: center">
            <b>Fish Feeder v2.0</b>
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
                    <p>
                        <b>Feeding scheme:</b><br />
                        <select name="scheme">
                            <option value="0">Auto</option>
                            <option value="1">Once a day</option>
                            <option value="2">Twice a day</option>
                            <option value="3">Thrice a day</option>
                            <option value="10">Never</option>
                        </select>
                    </p>

                    <p>
                        <b>Feeding days:</b><br />
                        <input type="checkbox" name="days" value="0"/> Sunday<br/>
                        <input type="checkbox" name="days" value="1"/> Monday<br/>
                        <input type="checkbox" name="days" value="2"/> Tuesday<br/>
                        <input type="checkbox" name="days" value="3"/> Wednesday<br/>
                        <input type="checkbox" name="days" value="4"/> Thursday<br/>
                        <input type="checkbox" name="days" value="5"/> Friday<br/>
                        <input type="checkbox" name="days" value="6"/> Saturday<br/>
                    </p>

                    <p>
                        <b>Feed amount:</b><br />
                        <select name="amount">
                            <option>One</option>
                            <option>Two</option>
                            <option>Three</option>
                            <option>Four</option>
                            <option>Five</option>
                        </select>
                    </p>
                </div>
                <div class="card-footer">
                    <button onclick="onFeed()" class="btn btn-secondary">Feed now</button>
                    <button onclick="onSaveSettings()" class="btn btn-primary">Apply Settings</button>
                </div>
            </div>
        </div>

        <p></p>

        <table class="table table-striped table-hover">
            <thead>
                <tr>
                    <th>Time</th>
                    <th>Water °C</th>
                    <th>Amount</th>
                </tr>
            </thead>
            <tbody id="feedings"></tbody>
        </table>

        <p></p>

        <table class="table table-striped table-hover">
            <thead>
                <tr>
                    <th>Time</th>
                    <th>Water °C</th>
                </tr>
            </thead>
            <tbody id="temperatures"></tbody>
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

        templates = ["temperature", "time"]
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

makeRequest("/feedings", function() {
    var data = JSON.parse(this.response);
    var template = document.getElementById("tpl_feedings").innerHTML;
    var out = Mark.up(template, data);
    document.getElementById("feedings").innerHTML = out;
});

makeRequest("/temperatures", function() {
    var data = JSON.parse(this.response);
    var template = document.getElementById("tpl_temperatures").innerHTML;
    var out = Mark.up(template, data);
    document.getElementById("temperatures").innerHTML = out;
});
)=====";

#endif