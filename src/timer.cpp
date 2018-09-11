#include "timer.h"

Timer::Timer(unsigned long intervalMillis) {
    interval = intervalMillis;
}

void Timer::start() {
    startTime = millis();
}

void Timer::reset() {
    start();
}

bool Timer::done(){
    if (timeElapsed(interval)) {
        reset();
        return true;
    }

    return false;
}

bool Timer::timeElapsed(unsigned long timeoutMillis) {
    return (millis() - startTime) >= timeoutMillis;
}
