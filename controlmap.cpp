
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
#include <stdint.h>
#include "daisy_pod.h"
#include "voice.h"
#include "filter.h"
#include "utilities.h"
#include "controlmap.h"


/* proposed map
 
button1	picks what the selector switch rotates through. 
selector leds		selects	
--------------------------
0		temp br		voices
1		temp bg		filters
//2		temp bb		key - how do we know which key?

Also assign two midi notes to up key and down key in rotation

The tem led shows its color for 2 seconds then reverts to button 2 setting

button2
pots control as below

Button 2 maps the pots to their function
We do not jump immediatly to the pot value when changed
rather the pot has to move significantly before it takes effect after a switch
The rgb color combo indicates what the pots do

(which pot controls what)
button 2	leds	pot1	pot2
--------------------------------
0			rr		gainR	gainL 
1			rg		filterF filterRes
2 			rb		p1		p2
3			gr		p3		p4
4			gg		p5		p6
//5			gb		?		?

voice	parameter function

			p1			p2			p3			p4			p5			p6
synth		adsrattack	adsrdecay	adsrsustain	adsrrelease
formant		adsrattack	adsrdecay	adsrsustain	adsrrelease	formantfreq	phaseshift
mallet		damping		structure	brightness	accent
noise		resonance	drive		attack		release
spring		damping		structure	brightness	accent
 */

#define NUM_SELECTORS	2 // voices and filters
#define NUM_PARM_PAIRS  5 // number of parameter pairs

void ControlMap::Init(DaisyPod *phw, Voices *pvoices, Filters *pfilts, float *pgainL, float *pgainR)
{
	voices = pvoices;
	filters = pfilts;
	hw = phw;
	gainL = pgainL;
	gainR = pgainR;
	
	// allow clipping and adjust for low volume voices
	gainLeftPot.Init(hw->knob1, 0, 16, Parameter::LOGARITHMIC);
	gainRightPot.Init(hw->knob2, 0, 16, Parameter::LOGARITHMIC);

	button1 = 0;
	button2 = 0;
	
	knobsChanged = false;

	SetLEDs(RED_ON, RED_ON);
	log("Control map init");		
}

void ControlMap::Control()
{
	if (hw->button1.RisingEdge())
	{
		button1++;
		if (button1 == NUM_SELECTORS)
		{
			button1 = 0;
		}
		
		switch (button1)
		{
		case 0:
			BlinkLEDs(3000, BLUE_ON, RED_ON);
			log("Select voice");
			break;
		
		case 1:
			BlinkLEDs(3000, BLUE_ON, GREEN_ON);
			log("Select filter");
			break;
		
		case 2:
		default:
			break;
		}
	
	}
	
	//Returns + 1 if the encoder was turned clockwise, -1 if it was turned counter - clockwise, or 0 if it was not turned.
	int8_t selector = hw->encoder.Increment();
	switch (button1)
	{
	case 0:
		voices->Select(selector);
		break;
		
	case 1:
		filters->Select(selector);
		break;
		
	case 2:
	default:
		break;
	}
	
	
	if (hw->button2.RisingEdge())
	{
		button2++;
		if (button2 == NUM_PARM_PAIRS)
		{
			button2 = 0;
		}
		
		knobsChanged = true;
		knob1LastValue = hw->knob1.Value();
		knob2LastValue = hw->knob2.Value();
		
		switch (button2)
		{
		case 0:
			SetLEDs(RED_ON, RED_ON);
			log("RR - Volume L, R");
			break;
		
		case 1:
			SetLEDs(RED_ON, GREEN_ON);
			log("RG - Filter Freq, Filter Res");
			break;
		
		case 3:
			SetLEDs(RED_ON, BLUE_ON);
			log("RB - Voice P1, P2");
			break;
		
		case 4:
			SetLEDs(GREEN_ON, RED_ON);
			log("GR - Voice P2, P3");
			break;
		
		default:
			break;
		}
	}

	
	if (knobsChanged)
	{
		// allow knob changes only if a knob was turned significantly so we don't jump to the previous knob setting
		if (fabs(knob1LastValue - hw->knob1.Value())> 0.25 || fabs(knob2LastValue - hw->knob2.Value()) > 0.25)
		{
			knobsChanged = false;	
		}
		else
		{
			return;
		}
	}
	

	switch (button2)
	{
	case 0:
		*gainL = gainLeftPot.Process();
		*gainR = gainRightPot.Process();
		break;
		
	case 1:
		filters->ProcessFreq();
		filters->ProcessRes();
		break;
		
	case 3:
		voices->ProcessParm0();
		voices->ProcessParm1();
		break;
		
	case 4:
		voices->ProcessParm2();
		voices->ProcessParm3();
		break;
		
	default:
		break;
	}
}

