#include <Arduino.h>
#include <Servo.h>
#include "config.h"
#include "logger.h"

#ifndef __FEEDER__
#define __FEEDER__

#define ERR_TOO_SOON    -1
#define ERR_AMOUNT_ZERO -2
#define ERR_NIGHT_FEED  -3

class Feeder {
    public:
        Feeder(Logger* logger);
        void start();
        void stop();

        void checkAndFeed();
        int feedNow();

    private:
        Logger* logger;
        Servo servo;
        int dumpFood();
};

#endif