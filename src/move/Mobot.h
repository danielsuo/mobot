#ifndef MOBOT_H
#define MOBOT_H

#if defined(ARDUINO) && ARDUINO < 100
#error "This library requires Arduino 1.0 or newer."
#endif

#include <Arduino.h>

class Mobot {
public:
  Mobot();
};

#endif