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

#include "utilities.h"
#include "voice.h"

using namespace daisy;
using namespace daisysp;

void MalletVoice::Init(DaisyPod *phw, float SR) 
{
	NullVoice::Init(phw, SR);
	polyphony = MALLET_VOICE_POLYPHONY;
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		mallet[i].Init(sampleRate);
	}	
	
	DampingPotParm.Init(hw->knob1, 0, 1, Parameter::LINEAR);
	StructurePotParm.Init(hw->knob2, 0, 1, Parameter::LINEAR); // sets range and plot 
	BrightnessPotParm.Init(hw->knob1, 0, 1, Parameter::LINEAR); // sets range and plot 
	AccentPotParm.Init(hw->knob2, 0, 1, Parameter::LINEAR); // sets range and plot 

	Panic();
}

void MalletVoice::Panic() 
{
	NullVoice::Panic();
	
	//for (uint8_t i = 0; i < polyphony; i++)
	//{
	//	mallet[i].Reset();
	//}
}


void MalletVoice::StartNote(uint8_t i, NoteOnEvent *p)
{
	//log("n: %d, m: %d", i, p->note);
	notes[i].midiNote = p->note;
	notes[i].amplitude = (float)p->velocity / 127.0f;
			
	mallet[i].SetFreq(mtof(p->note));
	mallet[i].Trig();
	
}

void MalletVoice::NoteOn(NoteOnEvent *p)
{
	for (uint8_t i = 0; i < polyphony; i++)
	{
		
		if (notes[i].midiNote == p->note)
		{
			// if we are playing the same note
			StartNote(i, p);
			return;
		}
		
		// start new note if unused and (todo) adsr is done 
		if (notes[i].midiNote == 0 )
		{
			StartNote(i, p);
			return;	
		}
	}
}


void MalletVoice::NoteOff(NoteOffEvent *p)
{
	for (uint8_t i = 0; i < polyphony; i++)
	{
		if (notes[i].midiNote == p->note)
		{
			notes[i].midiNote = 0;	
		}
	}
}


void MalletVoice::SetFreq(float f)
{
	
}

float MalletVoice::Process(void) 
{
	float sig = 0.0;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		sig += mallet[i].Process() * notes[i].amplitude;
	}
	
	return sig / polyphony;
}
	

void MalletVoice::SetDamping(float v)
{
	if (v == damping)
	{
		return;
	}
	
	damping = v;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		mallet[i].SetDamping(damping);
	}
}


void MalletVoice::SetStructure(float v)
{
	if (v == structure)
	{
		return;
	}
	
	structure = v;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		mallet[i].SetStructure(structure);
	}
}


void MalletVoice::SetBrightness(float v)
{
	if (v == brightness)
	{
		return;
	}
	
	brightness = v;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		mallet[i].SetBrightness(brightness);
	}
}


void MalletVoice::SetAccent(float v)
{
	if (v == accent)
	{
		return;
	}
	
	accent = v;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		mallet[i].SetAccent(accent);
	}
}


void MalletVoice::ProcessParm0()
{
	SetDamping(DampingPotParm.Process());
}


void MalletVoice::ProcessParm1()
{
	SetStructure(StructurePotParm.Process());
}

void MalletVoice::ProcessParm2()
{
	SetBrightness(BrightnessPotParm.Process());
}

void MalletVoice::ProcessParm3()
{
	SetAccent(AccentPotParm.Process());
}

	
void MalletVoice::SetCC0(uint8_t value)
{
	SetDamping(value / 127.0);
}

void MalletVoice::SetCC1(uint8_t value)
{
	SetStructure(value / 127.0);
}

void MalletVoice::SetCC2(uint8_t value)
{
	SetBrightness(value / 127.0);
}

void MalletVoice::SetCC3(uint8_t value)
{
	SetAccent(value / 127.0);
}



