/*
 This file is part of Virtual Robot's Pine Synthesizer. 

Copyright <2022> <Brian Gunnison>

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, 
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, subject to the following 
conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once
#include "daisy_pod.h"
#include <stdint.h>

using namespace daisy;


void log(const char* format, ...);
const char *GetNoteName(uint8_t n);
char * GetMidiNoteName(uint8_t n);

void RotateLED(uint8_t led);
void LEDOff(uint8_t led);



typedef enum
{
	LED_OFF,
	RED_ON,
	GREEN_ON,
	BLUE_ON
}LED_COLOR_ON;

void SetLEDs(LED_COLOR_ON led1, LED_COLOR_ON led2, bool blinked=false);
void BlinkLEDs(uint16_t msec, LED_COLOR_ON led1, LED_COLOR_ON led2);
void ResetLEDs();


float GetCCMinMax(uint8_t CCValue, float min, float max);
void ComInit(DaisyPod *hw);



