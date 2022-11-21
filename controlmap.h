

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

#include <stdint.h>
#include "utilities.h"



// control map - maps the pod knobs to voice or filter parms based on selector knob
class ControlMap
{
public:
	
	void Init(DaisyPod *hw, Voices *voice, Filters *filt, float *gl, float *gr);
	
	void Control();
	
	
private:
	
	DaisyPod *hw;
	Voices *voices;
	Filters *filters;
	float *gainL;
	float *gainR;
	Parameter gainLeftPot;
	Parameter gainRightPot;
	
	uint8_t button1;
	uint8_t button2;
	
	bool knobsChanged;
	float knob1LastValue;
	float knob2LastValue;


		

	
	
};
