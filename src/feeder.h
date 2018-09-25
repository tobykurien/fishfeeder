#include <Arduino.h>
#include <Servo.h>
#include "config.h"
#include "logger.h"

#ifndef __FEEDER__
#define __FEEDER__

class Feeder {
    public:
        Feeder(Logger* logger);
        void start();
        void stop();

        int checkAndFeed();
        void feedNow();

    private:
        Logger* logger;
        Servo servo;
        void dumpFood();
};

#endif