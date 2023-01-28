
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
#include "daisy_pod.h"
#include "daisysp.h"
#include <stdio.h>
#include <string.h>
#include "utilities.h"

using namespace daisy;
using namespace daisysp;

extern DaisyPod   hw;


void log(const char* format, ...)
{
	char  buff[512];
	va_list va;
	
	va_start(va, format);
	vsnprintf(buff, 510, format, va);
	va_end(va);
	
	strncat(buff, "\n", 512);
	
	hw.seed.usb_handle.TransmitInternal((uint8_t *)buff, strlen(buff));
}

const char *GetNoteName(uint8_t n)
{	
	const char *names[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
	
	if (n > 11)
	{
		return "?";
	}
	
	return names[n];

}

char * GetMidiNoteName(uint8_t n)
{
	static char str[] = "C0";
	
	uint8_t noteName = n % 12;
	uint8_t octave = (n / 12) - 1;
	
	sprintf(str, "%s%d", GetNoteName(noteName), octave);
	
	return str;
}

// todo class up and implement midi map parser as well as commands such as get current settings etc. 
// UsbHandle::ReceiveCallback
void echo_rx_callback(uint8_t* buf, uint32_t* len)
{
	// do nothing
	char inBuf[32];
	
	if (len == NULL || buf == NULL)
	{
		return;
	}
	
	uint32_t l = *len;
	uint8_t i = 0;
	for ( i < l; i++;)
	{
		inBuf[i] = buf[i];
		if (i == 32)
		{
			break;
		}
	}
	
	inBuf[i - 1] = 0;
		
	log(inBuf);
	
	
}

void ComInit(DaisyPod *hw)
{
	hw->seed.usb_handle.Init(UsbHandle::FS_INTERNAL);
	System::Delay(250);
	hw->seed.usb_handle.SetReceiveCallback( echo_rx_callback, UsbHandle::FS_INTERNAL);
}

// remember what the LEDs are set to
static LED_COLOR_ON prevLED1 = LED_OFF;
static LED_COLOR_ON prevLED2 = LED_OFF;


void RotateRGB(float &r, float &g, float &b)
{
	if (r > 0.0)
	{
		r = 0.0;
		g = 1.0;
		b = 0.0;
		return;
	}
	
	if (g > 0.0)
	{
		r = 0.0;
		g = 0.0;
		b = 1.0;
		return;
	}

	if (b > 0.0)
	{
		r = 1.0;
		g = 0.0;
		b = 0.0;
		return;
	}

	return;
}


void SetLEDs(LED_COLOR_ON led1, LED_COLOR_ON led2, bool blinked)
{	
	float rgb1[3];
	float rgb2[3];
	
	if (blinked == false)
	{
		prevLED1 = led1;
		prevLED2 = led2;		
	}

	switch (led1)
	{
	case RED_ON:
		rgb1[0] = 1.0;
		rgb1[1] = 0.0;
		rgb1[2] = 0.0;
		break;
		
	case GREEN_ON:
		rgb1[0] = 0.0;
		rgb1[1] = 1.0;
		rgb1[2] = 0.0;
		break;
	
	case BLUE_ON:
		rgb1[0] = 0.0;
		rgb1[1] = 0.0;
		rgb1[2] = 1.0;
		break;
		
	case LED_OFF:
		rgb1[0] = 0.0;
		rgb1[1] = 0.0;
		rgb1[2] = 0.0;
		break;
		
	default:
		return;
	}
	
	switch (led2)
	{
	case RED_ON:
		rgb2[0] = 1.0;
		rgb2[1] = 0.0;
		rgb2[2] = 0.0;
		break;
		
	case GREEN_ON:
		rgb2[0] = 0.0;
		rgb2[1] = 1.0;
		rgb2[2] = 0.0;
		break;
	
	case BLUE_ON:
		rgb2[0] = 0.0;
		rgb2[1] = 0.0;
		rgb2[2] = 1.0;
		break;
		
	case LED_OFF:
		rgb2[0] = 0.0;
		rgb2[1] = 0.0;
		rgb2[2] = 0.0;
		break;

	default:
		return;
	}
	
	hw.led1.Set(rgb1[0], rgb1[1], rgb1[2]);
	hw.led2.Set(rgb2[0], rgb2[1], rgb2[2]);
	hw.UpdateLeds();
}

static uint32_t ledtimeout = 0;
static uint32_t startTimer = 0;

void BlinkLEDs(uint16_t msec, LED_COLOR_ON led1, LED_COLOR_ON led2)
{
	ledtimeout = msec;
	startTimer = System::GetNow();
		
	SetLEDs(led1, led2, true);
}



// called in main loop
void ResetLEDs()
{
	if (startTimer == 0)
	{
		return;
	}
	
	uint32_t tp = System::GetNow() - startTimer;
	if (tp < ledtimeout)
	{
		return;
	}

	SetLEDs(prevLED1, prevLED2);
	
	startTimer = 0;
}

void RotateLED(uint8_t led)
{
	static float rgb1[3] = { 1.0, 0.0, 0.0 };
	static float rgb2[3] = { 1.0, 0.0, 0.0 };
		
	switch (led)
	{
	case 1:
		RotateRGB(rgb1[0], rgb1[1], rgb1[2]);
		hw.led1.Set(rgb1[0], rgb1[1], rgb1[2]);
		break;
		
	case 2:
		RotateRGB(rgb2[0], rgb2[1], rgb2[2]);
		hw.led2.Set(rgb2[0], rgb2[1], rgb2[2]);
		break;
		
	default:
		return;

	}
	
	hw.UpdateLeds();
}

void LEDOff(uint8_t led)
{
	switch (led)
	{
	case 1:
		hw.led1.Set(0, 0, 0);
		break;
		
	case 2:
		hw.led2.Set(0, 0, 0);
		break;
		
	default:
		return;

	}
	
	hw.UpdateLeds();
}



float GetCCMinMax(uint8_t CCValue, float min, float max)
{
	float v = (float)CCValue / 127.0f;
	float ret = min + ((max - min) * v);
	return ret;
}