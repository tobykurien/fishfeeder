#include <Arduino.h>
#include <RTClib.h>

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal

#include "config.h"
#include "index.html.h"
#include "assets.h"
#include "timer.h"
#include "logger.h"
#include "feeder.h"

RTC_DS3231 rtc;
Logger logger;
Feeder feeder;

const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
IPAddress         apIP(10, 10, 10, 1);    // Private network for server
DNSServer         dnsServer;              // Create the DNS object
ESP8266WebServer  server(80);

Timer             feedTimer(5*60*1000),     // how often to check feeding
                  tempLogTimer(60*60*1000), // how often to log temperature
                  debugTimer(1000);

void setup() {
    pinMode(LED, OUTPUT);
    pinMode(TEMP_POWER, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
   
    digitalWrite(LED, HIGH);
    digitalWrite(BUTTON, HIGH); // pull-up

    Serial.println(ESP.getResetReason());
    Serial.begin(115200);
    Serial.println("Starting fish feeder");

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC, stopping.");
        while (1);
    }

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, lets set the time!");
        // following line sets the RTC to the date & time this sketch was compiled
        //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        //rtc.adjust(DateTime(2018, 8, 28, 16, 16, 0));
    }

    feedTimer.start();
    tempLogTimer.start();
    debugTimer.start();
    logger.start();
    feeder.start();

    logger.logTemperature(rtc.now().unixtime(), temperature());

    startWifi();
}

void loop() {
    if (feedTimer.done()) {
        int amount = feeder.checkAndFeed();
        if (amount > 0) {
            // log the feeding event
            logger.logFeeding(rtc.now().unixtime(), amount, temperature());
        }
    }

    if (tempLogTimer.done()) {
        logger.logTemperature(rtc.now().unixtime(), temperature());
    }

    if (debugTimer.done()) {
        Serial.print(getTime());
        Serial.print("\t");
        Serial.print(temperature());
        Serial.println("°C");
    }

    delay(1000);
}

float temperature() {
    digitalWrite(TEMP_POWER, HIGH);
    delay(10);
    int t = analogRead(TEMP_SENS);
    digitalWrite(TEMP_POWER, LOW);

    // calibrated to degrees celcius with thermometer
    int reading = map(t*10, 4970, 7390, 270, 50);
    return reading / 10.0;
}

String getTime() {
    DateTime now = rtc.now();
    
    return String(now.year()) + '/' +
        String(now.month()) + '/' +
        String(now.day()) + ' ' +
        String(now.hour()) + ':' +
        String(now.minute()) + ':' +
        String(now.second());
}

void startWifi() {
    Serial.print("Setting soft-AP ... ");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    boolean result = WiFi.softAP(WIFI_SSID, WIFI_PASS);

    if(result == true) {
        Serial.println("Ready");
    } else {
        Serial.println("Failed!");
    }

    long startTime = millis();
    while (WiFi.softAPgetStationNum() == 0 
            && (millis() - startTime) < WIFI_TIMEOUT_MS) {
        digitalWrite(LED, HIGH);
        delay(100);
        digitalWrite(LED, LOW);
        delay(100);
    }

    handleWebsite();
    digitalWrite(LED, HIGH);

    WiFi.disconnect();
    Serial.println("SoftAP shut down");
}

void handleWebsite() {
  dnsServer.start(DNS_PORT, "*", apIP); // captive portal

  server.on("/monitor", [](){
      server.send(200, "application/json", 
        "{ \"time\": \"" + getTime() + "\"," +
        "  \"temperature\": " + String(temperature()) + " }");
  });

  server.on("/feedings", [](){
      WiFiClient client = server.client();
      client.write("Content-Type: application/json");
      client.write("{ \"lastFeedings\": [");
      // TODO - implement circular buffer with check for valid timestamp
      for (int i=0; i < 10; i++) {
          Feeding f = logger.getData().feedings[i];
          client.print("{ \"time\": " + String(f.timestamp));
          client.print(", \"temperature\": " + String(f.temperature));
          client.print(", \"amount\": " + String(f.amount) + "}");
          if (i != 9) client.write(",");
          client.flush();
      }
      client.write("]}");
  });

  server.on("/temperatures", [](){
      // TODO
      server.send(200, "application/json", "[]");
  });

  server.on("/feed", [](){
      server.send(200, "text/plain", "Ok");
      feeder.feedNow();
  });

  server.on("/app.js", [](){
      server.send(200, "text/javascript", FPSTR(APP_JS));
  });

  server.on("/app.css", [](){
      server.send(200, "text/stylesheet", FPSTR(APP_CSS));
  });

  server.on("/markup.js", [](){
      server.send(200, "text/javascript", FPSTR(MARKUP_JS));
  });

  server.on("/set-time", []() {
      server.send(200, "text/plain", "Ok");
      rtc.adjust(DateTime(
          server.arg("year").toInt(), 
          server.arg("month").toInt(), 
          server.arg("day").toInt(), 
          server.arg("hour").toInt(), 
          server.arg("minute").toInt(), 
          server.arg("second").toInt()));
  });

  server.on("/", handleRoot);
  server.onNotFound(handleRoot);

  server.begin();
  Serial.println("HTTP server started");

  while (WiFi.softAPgetStationNum() != 0) {
    dnsServer.processNextRequest();
    server.handleClient(); 
  } 

  server.stop();
  Serial.println("HTTP server stopped");
}

void handleRoot() {
  server.send(200, "text/html", FPSTR(INDEX_HTML));
}
