#include <Arduino.h>
#include <Servo.h>
#include <RTClib.h>

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include "index.html.h"

#define TEMP_SENS   A0
#define TEMP_POWER  D8
#define SERVO       D2
#define SERVO_POWER D7
#define LED         2
#define BUTTON      D10

#define WIFI_SSID       "Fish-Feeder"
#define WIFI_PASS       "FeedingNemo"
#define WIFI_TIMEOUT_MS 30000

Servo servo;
RTC_DS3231 rtc;

const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
IPAddress         apIP(10, 10, 10, 1);    // Private network for server
DNSServer         dnsServer;              // Create the DNS object
ESP8266WebServer  server(80);

char daysOfTheWeek[7][12] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", 
    "Thursday", "Friday", "Saturday"
};

void setup() {
    pinMode(LED, OUTPUT);
    pinMode(TEMP_POWER, OUTPUT);
    pinMode(SERVO_POWER, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
   
    digitalWrite(LED, HIGH);
    digitalWrite(SERVO_POWER, LOW);
    digitalWrite(BUTTON, HIGH); // pull-up

    Serial.println(ESP.getResetReason());
    Serial.begin(115200);
    Serial.println("Starting fish feeder");

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, lets set the time!");
        // following line sets the RTC to the date & time this sketch was compiled
        //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        //rtc.adjust(DateTime(2018, 8, 28, 16, 16, 0));
    }

    startWifi();
    //dumpFood();
}

void loop() {
    Serial.println(temperature());
    Serial.println(getTime());
    delay(1000);
}

void dumpFood() {
    Serial.println("Dumping food");

    // start up servo
    digitalWrite(SERVO_POWER, HIGH);
    delay(500);
    servo.attach(SERVO);

    // turn the feeder
    servo.write(0);
    delay(500);
    servo.write(90);
    delay(500);
    servo.write(180);
    delay(500);
    servo.write(90);
    delay(500);
    servo.write(0);
    delay(500);

    // shutdown the servo
    servo.detach();
    digitalWrite(SERVO_POWER, LOW);
}

int temperature() {
    digitalWrite(TEMP_POWER, HIGH);
    delay(10);
    int t = analogRead(TEMP_SENS);
    //Serial.println(t);
    digitalWrite(TEMP_POWER, LOW);

    // calibrated to degrees celcius with thermometer
    int reading = map(t, 255, 462, -1, 20);
    return reading;
}

String getTime() {
    DateTime now = rtc.now();
    
    return String(now.year()) + '/' +
        String(now.month()) + '/' +
        String(now.day()) + ' ' +
        String(now.hour()) + ':' +
        String(now.minute()) + ':' +
        String(now.second());
    
    // Serial.print(" since midnight 1/1/1970 = ");
    // Serial.print(now.unixtime());
    // Serial.print("s = ");
    // Serial.print(now.unixtime() / 86400L);
    // Serial.println("d");   

    // calculate a date which is 7 days and 30 seconds into the future
    // DateTime future (now + TimeSpan(7,12,30,6));     
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
  
  server.on("/feed", [](){
      server.send(200, "text/plain", "Done");
      dumpFood();
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
  server.send(200, "text/html", parseTemplate(FPSTR(INDEX_HTML)));
}

String parseTemplate(String str) {
    str.replace("{{time}}", getTime());
    str.replace("{{temperature}}", String(temperature()));
    str.replace("{{lastFeedings}}", ""); // TODO - comma-separated list of times
    return str;
}