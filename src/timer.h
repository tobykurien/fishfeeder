#ifndef __TIMER__
#define __TIMER__

#include <Arduino.h>

class Timer {
    public:
        Timer(unsigned long intervalMillis);
        void start();
        bool timeElapsed(unsigned long timeoutMillis);
        void reset();
        bool done();
        void stop();

    private:
        unsigned long startTime = 0;
        unsigned long interval = 0;
};

#endif