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


void FormantVoice::Init(float SR) 
{
	NullVoice::Init(SR);
	polyphony = FORMANT_VOICE_POLYPHONY;
	ADSROn = true;
	ADSRAttack = ADSR_ATTACK_DEFAULT;
	ADSRDecay = ADSR_DECAY_DEFAULT;
	ADSRSustain = 1.0;
	ADSRRelease = ADSR_RELEASE_DEFAULT;
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		formant[i].Init(sampleRate);
		formant[i].SetFormantFreq(1000);
		formant[i].SetPhaseShift(0);
		
		adsr[i].Init(sampleRate);
		adsr[i].SetAttackTime(ADSRAttack);
		adsr[i].SetDecayTime(ADSRDecay);
		adsr[i].SetSustainLevel(ADSRSustain);
		adsr[i].SetReleaseTime(ADSRRelease);
	}	
	
	Panic();
}

void FormantVoice::Panic() 
{
	NullVoice::Panic();
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		adsr[i].Process(false);	
	}
}

void FormantVoice::StartNote(uint8_t i, NoteOnEvent *p)
{
	//log("n: %d, m: %d", i, p->note);
	notes[i].midiNote = p->note;
	notes[i].amplitude = (float)p->velocity / 127.0f;
			
	formant[i].SetCarrierFreq(mtof(p->note));
	adsr[i].Retrigger(false); // set attack mode
	
}


void FormantVoice::NoteOn(NoteOnEvent *p)
{
	for (uint8_t i = 0; i < polyphony; i++)
	{
		if (notes[i].midiNote == p->note)
		{
			// if we are playing the same note
			StartNote(i, p);
			return;
		}
		
		// start new note if unused and adsr is done 
		if (notes[i].midiNote == 0 && adsr[i].IsRunning() == false)
		{
			StartNote(i, p);
			return;	
		}
	}
}


void FormantVoice::NoteOff(NoteOffEvent *p)
{
	for (uint8_t i = 0; i < polyphony; i++)
	{
		if (notes[i].midiNote == p->note)
		{
			notes[i].midiNote = 0;	
		}
	}
}


float FormantVoice::Process(void) 
{
	float sig = 0.0;
	for (uint8_t i = 0; i < polyphony; i++)
	{
		bool attack = true;
		if (notes[i].midiNote == 0)
		{
			attack = false; // release
		}
				
		float ADSRLevel = 1.0;
		if (ADSROn == true)
		{
			ADSRLevel = adsr[i].Process(attack);
		}
		sig += formant[i].Process() * ADSRLevel * notes[i].amplitude;
	}
	
	return sig / polyphony;
}
	
void FormantVoice::SetCC0(uint8_t value)
{
	SetADSRAttackCC(value);
}

void FormantVoice::SetCC1(uint8_t value)
{
	SetADSRDecayCC(value);
}

void FormantVoice::SetCC2(uint8_t value)
{
	SetADSRSustainCC(value);
}

void FormantVoice::SetCC3(uint8_t value)
{
	SetADSRReleaseCC(value);
}

void FormantVoice::SetCC4(uint8_t value)
{
	SetFormantFreqCC(value);
}

void FormantVoice::SetCC5(uint8_t value)
{
	SetPhaseShiftCC(value);
}

#define CARRIER_FREQ_MIN 1000.0
#define CARRIER_FREQ_MAX 20000.0

void FormantVoice::SetFormantFreqCC(uint8_t value)
{
	float f = GetCCMinMax(value, CARRIER_FREQ_MIN, CARRIER_FREQ_MAX);
	if (f == formantFreq)
	{
		return;
	}
	formantFreq = f;
	//log("Formant Freq: %u ", (uint32_t)(formantFreq * 1000));

	for (uint8_t i = 0; i < polyphony; i++)
	{
		formant[i].SetCarrierFreq(formantFreq); 
	}
}


void FormantVoice::SetPhaseShiftCC(uint8_t value)
{
	float p = float(value)/127.0;
	if (p == phaseShift)
	{
		return;
	}
	phaseShift = p;
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		formant[i].SetPhaseShift(phaseShift); 
	}
}


void FormantVoice::SetADSRAttackCC(uint8_t value)
{
	float a = GetCCMinMax(value, ADSR_ATTACK_MIN, ADSR_ATTACK_MAX);
	if (a == ADSRAttack)
	{
		return;
	}
	ADSRAttack = a;
	//log("Attack: %d msec", (uint32_t)(ADSRAttack * 1000));
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		adsr[i].SetAttackTime(ADSRAttack); 
	}
}


void FormantVoice::SetADSRDecayCC(uint8_t value)
{
	float d = GetCCMinMax(value, ADSR_DECAY_MIN, ADSR_DECAY_MAX);
	if (d == ADSRDecay)
	{
		return;
	}
	ADSRDecay = d;
	//log("Decay: %d", (uint32_t)(ADSRDecay * 1000));
	
	for (uint8_t i = 0; i < polyphony; i++)
	{
		adsr[i].SetDecayTime(ADSRDecay); 
	}
}


void FormantVoice::SetADSRSustainCC(uint8_t value)
{
	if (value == 127)
	{
		ADSROn = false;
		log("ADSR off");
	}
	else
	{
		ADSROn = true;
	}
	
	float s = value / 127.0;
	
	if (s == ADSRSustain)
	{
		return;
	}
	ADSRSustain = s;
	//log("Sustain: %d", (uint32_t)(ADSRSustain * 1000));

	for (uint8_t i = 0; i < polyphony; i++)
	{
		adsr[i].SetSustainLevel(ADSRSustain); 
	}
}

void FormantVoice::SetADSRReleaseCC(uint8_t value)
{
	float r = GetCCMinMax(value, ADSR_RELEASE_MIN, ADSR_RELEASE_MAX);
	if (r == ADSRRelease)
	{
		return;
	}
	ADSRRelease = r;
	//log("Release: %d", (uint32_t)(ADSRRelease * 1000));

	for (uint8_t i = 0; i < polyphony; i++)
	{
		adsr[i].SetReleaseTime(ADSRRelease); 
	}
}

