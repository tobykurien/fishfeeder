
function onSaveSettings(button) {
    let form = document.forms[0];

    let days = 0;
    for (let i=0; i < 7; i++) {
        if (form.days[i].checked) {
            days |= 1 << i;
        }
    }

    let params = "scheme=" + form.scheme.selectedOptions[0].value +
        "&days=" + days +
        "&amount=" + form.amount.selectedOptions[0].value;

    makeRequest("/save-settings?" + params, function() {
        toast(this.responseText);
    });    
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

makeRequest("/get-settings", function() {
    var data = JSON.parse(this.response);
    let form = document.forms[0];

    for (opt in form.scheme.options) {
        let option = form.scheme.options[opt];
        if (option.value == data.scheme) {
            option.selected = true;
        }
    }
    
    for (let i=0; i < 7; i++) {
        form.days[i].checked = data.days >> i & 1;
    }

    for (opt in form.amount.options) {
        let option = form.amount.options[opt];
        if (option.value == data.amount) {
            option.selected = true;
        }
    }
});
