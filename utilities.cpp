
#include "daisy_pod.h"
#include "daisysp.h"
#include <stdio.h>
#include <string.h>

using namespace daisy;
using namespace daisysp;

extern DaisyPod   hw;


void log(const char* format, ...)
{
	char  buff[512];
	va_list va;
	
	va_start(va, format);
	vsnprintf(buff, 512, format, va);
	va_end(va);
	
	strncat(buff, "\n", 512);
	
	hw.seed.usb_handle.TransmitInternal((uint8_t *)buff, strlen(buff));
}


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