#pragma once

#include <stdint.h>

void log(const char* format, ...);
void RotateLED(uint8_t led);
void LEDOff(uint8_t led);
void RotateLED(uint8_t led);
float GetCCMinMax(uint8_t CCValue, float min, float max);




