#include <Arduino.h>

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal

#include "config.h"
#include "index.html.h"
#include "assets.h"
#include "timer.h"
#include "logger.h"
#include "feeder.h"

Logger logger;
Feeder feeder = Feeder(&logger);

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

    feedTimer.start();
    tempLogTimer.start();
    debugTimer.start();
    logger.start();
    feeder.start();

    logger.logTemperature();

    startWifi();
}

void loop() {
    if (feedTimer.done()) {
        feeder.checkAndFeed();
    }

    if (tempLogTimer.done()) {
        logger.logTemperature();
    }

    if (debugTimer.done()) {
        Serial.print(logger.getTime());
        Serial.print("\t");
        Serial.print(logger.getTemperature());
        Serial.println("°C");
    }

    delay(1000);
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
        "{ \"time\": \"" + logger.getTime() + "\"," +
        "  \"temperature\": " + String(logger.getTemperature()) + " }");
  });

  server.on("/feedings", [](){
      server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      server.send(200, "application/json", String());
      server.sendContent("{ \"lastFeedings\": [");

      // Loop through data history and output valid ones
      DataStruct* data = logger.getData();
      bool addComma = false;
      for (int i=data->latestFeeding; i != data->latestFeeding+1; i--) {
          Feeding f = data->feedings[i];
          if (f.timestamp == 0) break;
          if (addComma) server.sendContent(",");
          server.sendContent("{ \"time\": \"" + logger.getTime(f.timestamp) + "\"");
          server.sendContent(", \"temperature\": " + String(f.temperature));
          server.sendContent(", \"amount\": " + String(f.amount) + "}");
          addComma = true;
      }
      server.sendContent("]}");
  });

  server.on("/temperatures", [](){
      server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      server.send(200, "application/json", String());
      server.sendContent("{ \"temperatures\": [");

      // Loop through data history and output valid ones
      DataStruct* data = logger.getData();
      bool addComma = false;
      for (int i=data->latestTemperature; i != data->latestTemperature+1; i--) {
          Temperature t = data->temperatures[i];
          if (t.timestamp == 0) break;
          if (addComma) server.sendContent(",");
          server.sendContent("{ \"time\": \"" + logger.getTime(t.timestamp) + "\"");
          server.sendContent(", \"temperature\": " + String(t.temperature) + "}");
          addComma = true;
      }
      server.sendContent("]}");
  });

  server.on("/feed", [](){
      int ret = feeder.feedNow();
      if (ret == ERR_AMOUNT_ZERO) {
        server.send(200, "text/plain", "Error: Feeding disabled");
      } else if (ret == ERR_TOO_SOON) {
        server.send(200, "text/plain", "Error: Feeding too soon");
      } else {
          server.send(200, "text/plain", "Ok");
      }
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
      logger.setTime(DateTime(
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
