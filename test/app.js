const debug = true;

function onDumpsChanged(selectList) {
    alert('TODO');
}

function onSchemeChanged(selectList) {
    alert('TODO');
}

function onFeed() {
    makeRequest("/feed", function() {
        alert(this.responseText);
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
        alert(this.responseText);
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
        var template = document.getElementById('monitor').firstChild.textContent;
        var out = Mark.up(template, data);
        document.getElementById('monitor-output').innerHTML = out;
    });
}

setInterval(updateMonitor, 1000);
