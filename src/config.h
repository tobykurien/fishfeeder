// App configuration
#ifndef __CONFIG_H__
#define __CONFIG_H__

// Hardware pins
#define TEMP_SENS   A0
#define TEMP_POWER  D8
#define SERVO       D2
#define SERVO_POWER D7
#define LED         2
#define BUTTON      D10

// Wifi config
#define WIFI_SSID       "Fish-Feeder"
#define WIFI_PASS       "FeedingNemo"
#define WIFI_TIMEOUT_MS 30000

// Misc

// minimum time between feedings (for safety)
#define MIN_FEEDING_TIMEOUT 3*60*60*1000 // milliseconds

#endif