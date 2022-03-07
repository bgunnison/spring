#include "daisy_pod.h"
#include "daisysp.h"

#include "utilities.h"
#include "voice.h"

using namespace daisy;
using namespace daisysp;


void Voice::Init(DaisyPod *pod, float SR) 
{ 
	phw = pod; 
	sampleRate = SR; 
	currentVoiceSelector = 0; 
	accent = 0;
	damping = 0.5;
	
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		synth[i].Init(sampleRate);
		synth[i].SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
		synth[i].SetAmp(0);
		spring[i].Init(sampleRate);
		notes[i].amplitude = 0.0;
		notes[i].midiNote = 0;
	}
}

void Voice::Panic(void)
{
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		synth[i].SetAmp(0);
		spring[i].Reset();
		notes[i].amplitude = 0.0;
		notes[i].midiNote = 0;
	}	
}

void Voice::Select(int8_t sel)
{
	if (sel == 0)
	{
		return;
	}
	
	int8_t s = currentVoiceSelector;
	
	s += sel;
	if (s < 0)
	{
		s = NUM_VOICES - 1;
	}
	
	if (s > NUM_VOICES - 1)
	{
		s = 0;
	}
	
	if (s != currentVoiceSelector)
	{
		Panic();
	}
	
	currentVoiceSelector = s;
	
	switch (currentVoiceSelector)
	{
	case SYNTH_VOICE:
		log("Synth voice");
		break;
		
	case SPRING_VOICE:
		log("Spring voice");
		break;
		
	default:
		log("Unused voice");
		break;
	}	
}

void Voice::UpdateBackGround(void)
{
	switch (currentVoiceSelector)
	{
	case SYNTH_VOICE:
		phw->led1.Set(0.0, 1.0, 0.0);
		break;
			
	case SPRING_VOICE:
		phw->led1.Set(0.0, 0.0, 1.0);
		break;
			
	default:
		phw->led1.Set(1.0, 0.0, 0.0);
		break;
	}
		
	phw->UpdateLeds();
}

float Voice::Process(void)
{
	float sig = 0;
	
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		switch (currentVoiceSelector)
		{
		case SYNTH_VOICE:
			sig += synth[i].Process();
			break;
			
		case SPRING_VOICE:
			sig += spring[i].Process() * notes[i].amplitude;
			break;
	
		default:
			break;
		
		}
	}
	
	sig /= POLYPHONY;

	return sig;
}

void Voice::SetAccent(float set)
{
	if (set == accent)
	{
		return;
	}
	
	accent = set;
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		spring[i].SetAccent(accent);
	}
}


void Voice::SetDamping(float set)
{
	if (set == damping)
	{
		return;
	}
	
	damping = set;
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		spring[i].SetDamping(damping);
	}
}


void Voice::NoteOn(NoteOnEvent *p)
{
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		if (notes[i].midiNote == 0)
		{
			notes[i].midiNote = p->note;
			notes[i].amplitude = p->velocity / 127.0f;
			switch (currentVoiceSelector)
			{
			case SYNTH_VOICE:
				synth[i].SetFreq(mtof(p->note));
				synth[i].SetAmp(notes[i].amplitude);
				return;
			
			case SPRING_VOICE:
				spring[i].SetFreq(mtof(p->note));
				spring[i].Trig();
				// no SetAmp so we multiply velocity in Process
				return;
	
			default:
				return;
		
			}
		}
	}
}

void Voice::NoteOff(NoteOffEvent *p)
{
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		if (notes[i].midiNote == p->note)
		{
			notes[i].midiNote = 0;	
			switch (currentVoiceSelector)
			{
			case SYNTH_VOICE:
				synth[i].SetAmp(0);
				break;
			
			case SPRING_VOICE: // it just fades away
				break;
	
			default:
				break;
			}
		}
	}	
}
