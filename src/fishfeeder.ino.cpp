# 1 "/var/folders/cw/p_bjsb5126x2c87y9zzw05180000gn/T/tmpsK1Y0c"
#include <Arduino.h>
# 1 "/Users/tobykurien/Projects/fishfeeder/src/fishfeeder.ino"
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include "config.h"
#include "index.html.h"
#include "assets.h"
#include "timer.h"
#include "logger.h"
#include "feeder.h"

Logger logger;
Feeder feeder = Feeder(&logger);

const byte DNS_PORT = 53;
IPAddress apIP(10, 10, 10, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

Timer feedTimer(5*60*1000),
                  tempLogTimer(60*60*1000),
                  debugTimer(1000);
void setup();
void loop();
void startWifi();
void handleWebsite();
void handleRoot();
#line 26 "/Users/tobykurien/Projects/fishfeeder/src/fishfeeder.ino"
void setup() {
    pinMode(LED, OUTPUT);
    pinMode(TEMP_POWER, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);

    digitalWrite(LED, HIGH);
    digitalWrite(BUTTON, HIGH);

    Serial.println(ESP.getResetReason());
    Serial.begin(115200);
    Serial.println("Starting fish feeder");

    feedTimer.start();
    tempLogTimer.start();
    debugTimer.start();
    logger.start();
    feeder.start();

    startWifi();
}

void loop() {
    if (feedTimer.done()) {
        feeder.checkAndFeed();
    }

    if (tempLogTimer.done()) {
        logger.logTemperature();
    }

    if (DEBUG && debugTimer.done()) {
        DateTime now = DateTime(logger.getCurrentTime());
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


    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);
    Serial.println("SoftAP shut down");
}

void handleWebsite() {
  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/monitor", [](){
      server.send(200, "application/json",
        "{ \"time\": \"" + logger.getTime() + "\"," +
        "  \"temperature\": " + String(logger.getTemperature()) + " }");
  });

  server.on("/get-settings", [](){
      Settings* settings = logger.getSettings();
      server.send(200, "application/json",
        "{ \"scheme\": " + String(settings->feedingScheme) + "," +
        "  \"days\": " + String(settings->feedingDays) + "," +
        "  \"amount\": " + String(settings->feedingAmount) + " }");
  });

  server.on("/feedings", [](){
      server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      server.send(200, "application/json", String());
      server.sendContent("{ \"lastFeedings\": [");


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
      server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      server.send(200, "text/plain", String());
      int ret = feeder.feedNow();
      if (ret == ERR_AMOUNT_ZERO) {
        server.sendContent("Error: Feeding disabled");
      } else if (ret == ERR_TOO_SOON) {
        server.sendContent("Error: Feeding too soon");
      } else if (ret == ERR_NIGHT_FEED) {
        server.sendContent("Error: Feeding at night");
      } else {
        server.sendContent("Ok");
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

  server.on("/save-settings", [](){
      Settings* settings = logger.getSettings();
      settings->feedingScheme = server.arg("scheme").toInt();
      settings->feedingDays = server.arg("days").toInt();
      settings->feedingAmount = server.arg("amount").toInt();
      logger.saveSettings();
      server.send(200, "text/plain", "Ok");
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