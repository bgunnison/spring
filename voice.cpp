#include "daisy_pod.h"
#include "daisysp.h"

#include "utilities.h"
#include "voice.h"

using namespace daisy;
using namespace daisysp;

void NullVoice::Init(float SR)
{
	sampleRate = SR;
	Panic();
}

void NullVoice::Panic()
{
	//log("Null voice Panic");
	
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		notes[i].amplitude = 0.0;
		notes[i].midiNote = 0;
	}
}


void Voices::Init(DaisyPod *pod, float SR) 
{ 
	phw = pod; 
	sampleRate = SR; 
	currentVoiceSelector = 0; 
	oscVoice.Init(sampleRate);
	springVoice.Init(sampleRate);
	pvoice = &oscVoice;
}

void Voices::Panic(void)
{
	pvoice->Panic();
}

void Voices::Select(int8_t sel)
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
		pvoice = &oscVoice;
		break;
		
	case SPRING_VOICE:
		log("Spring voice");
		pvoice = &springVoice;
		break;
		
	default:
		log("Unused voice");
		break;
	}	
}

void Voices::UpdateBackGround(void)
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

float Voices::Process(void)
{	
	return pvoice->Process();
}


void Voices::NoteOn(NoteOnEvent *p)
{
	pvoice->NoteOn(p);
}

void Voices::NoteOff(NoteOffEvent *p)
{
	pvoice->NoteOff(p);
}

	
void Voices::SetCC0(uint8_t value)
{	
	pvoice->SetCC0(value);
}


void Voices::SetCC1(uint8_t value)
{	
	pvoice->SetCC1(value);
}


void Voices::SetCC2(uint8_t value)
{	
	pvoice->SetCC2(value);	
}


void Voices::SetCC3(uint8_t value)
{	
	pvoice->SetCC3(value);
}
	