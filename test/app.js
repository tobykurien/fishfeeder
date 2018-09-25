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

    makeRequest("/feedings", function() {
        var data = JSON.parse(this.response);
        var template = document.getElementById("tpl_feedings").innerHTML;
        var out = Mark.up(template, data);
        document.getElementById("feedings").innerHTML = out;
    });
}

function toast(message) {
    console.log(message);
    alert(message);
}

setInterval(updateMonitor, 1000);
