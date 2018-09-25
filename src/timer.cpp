#include "timer.h"

Timer::Timer(unsigned long intervalMillis) {
    interval = intervalMillis;
}

void Timer::start() {
    startTime = millis();
}

void Timer::stop() {
    startTime = 0;
}

void Timer::reset() {
    start();
}

bool Timer::done(){
    if (startTime == 0) return false;

    if (timeElapsed(interval)) {
        reset();
        return true;
    }

    return false;
}

bool Timer::timeElapsed(unsigned long timeoutMillis) {
    if (startTime == 0) return false;
    return (millis() - startTime) >= timeoutMillis;
}
