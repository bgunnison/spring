#include "daisy_pod.h"
#include "daisysp.h"

#include "utilities.h"
#include "voice.h"

using namespace daisy;
using namespace daisysp;

// ADSR for synth voice ranges
#define ADSR_ATTACK_MIN			0.01f
#define ADSR_ATTACK_DEFAULT		0.1f
#define ADSR_ATTACK_MAX			5.0f
#define ADSR_DECAY_MIN			0.01f
#define ADSR_DECAY_DEFAULT		0.5f
#define ADSR_DECAY_MAX			5.0f
#define ADSR_SUSTAIN_MIN		0.01f
#define ADSR_SUSTAIN_DEFAULT	2.0f
#define ADSR_SUSTAIN_MAX		20.0f
#define ADSR_RELEASE_MIN		0.01f
#define ADSR_RELEASE_DEFAULT	0.2f
#define ADSR_RELEASE_MAX		20.0f

void Voice::Init(DaisyPod *pod, float SR) 
{ 
	phw = pod; 
	sampleRate = SR; 
	currentVoiceSelector = 0; 
	accent = 0;
	damping = 0.5;
	ADSROn = true;
	ADSRAttack = ADSR_ATTACK_DEFAULT;
	ADSRDecay = ADSR_DECAY_DEFAULT;
	ADSRSustain = ADSR_SUSTAIN_DEFAULT;
	ADSRRelease = ADSR_RELEASE_DEFAULT;
	
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		synth[i].Init(sampleRate);
		synth[i].SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
		synth[i].SetAmp(0);
		spring[i].Init(sampleRate);
		notes[i].amplitude = 0.0;
		notes[i].midiNote = 0;
		
		adsr[i].Init(sampleRate);
		adsr[i].SetTime(ADSR_SEG_ATTACK, ADSRAttack);
		adsr[i].SetTime(ADSR_SEG_DECAY, ADSRDecay);
		adsr[i].SetSustainLevel(ADSRSustain);
		adsr[i].SetTime(ADSR_SEG_RELEASE, ADSRRelease);
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
		adsr[i].Process(false);
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
			{			
				// if (adsr[i].IsRunning() == false) // save some time don't process this osc?
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
				sig += synth[i].Process() * ADSRLevel;
				break;
			}
			
		case SPRING_VOICE:
			sig += spring[i].Process() * notes[i].amplitude;
			sig *= 2;
			break;
	
		default:
			break;
		
		}
	}
	
	sig /= POLYPHONY;

	return sig;
}


void Voice::NoteOn(NoteOnEvent *p)
{
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		if (notes[i].midiNote == 0)
		{
			notes[i].midiNote = p->note;
			notes[i].amplitude = (float)p->velocity / 127.0f;
			switch (currentVoiceSelector)
			{
			case SYNTH_VOICE:
				synth[i].SetFreq(mtof(p->note));
				synth[i].SetAmp(notes[i].amplitude);
				adsr[i].Retrigger(false);

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
				if (ADSROn == false)
				{
					synth[i].SetAmp(0.0);
				}
				break;
			
			case SPRING_VOICE: // it just fades away
				break;
	
			default:
				break;
			}
		}
	}	
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


void Voice::SetStructure(float set)
{
	if (set == structure)
	{
		return;
	}
	
	structure = set;
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		spring[i].SetStructure(structure);
	}
}


void Voice::SetBrightness(float set)
{
	if (set == brightness)
	{
		return;
	}
	
	brightness = set;
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		spring[i].SetBrightness(brightness);
	}
}


float Voice::GetCCMinMax(uint8_t CCValue, float min, float max)
{
	float v = (float)CCValue / 127.0f;
	float ret = min + ((max - min) * v);
	return ret;
}

void Voice::SetADSRAttackCC(uint8_t value)
{
	float a = GetCCMinMax(value, ADSR_ATTACK_MIN, ADSR_ATTACK_MAX);
	if (a == ADSRAttack)
	{
		return;
	}
	ADSRAttack = a;
	log("Attack: %d msec", (uint32_t)(ADSRAttack * 1000));
	
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		adsr[i].SetTime(ADSR_SEG_ATTACK, ADSRAttack); 
	}
}


void Voice::SetADSRDecayCC(uint8_t value)
{
	float d = GetCCMinMax(value, ADSR_DECAY_MIN, ADSR_DECAY_MAX);
	if (d == ADSRDecay)
	{
		return;
	}
	ADSRDecay = d;
	log("Decay: %d", (uint32_t)(ADSRDecay * 1000));
	
	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		adsr[i].SetTime(ADSR_SEG_DECAY, ADSRDecay); 
	}
}


void Voice::SetADSRSustainCC(uint8_t value)
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
	
	float s = GetCCMinMax(value, ADSR_SUSTAIN_MIN, ADSR_SUSTAIN_MAX);
	if (s == ADSRSustain)
	{
		return;
	}
	ADSRSustain = s;
	log("Sustain: %d", (uint32_t)(ADSRSustain * 1000));

	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		adsr[i].SetTime(ADSR_SEG_IDLE, ADSRSustain); 
	}
}

void Voice::SetADSRReleaseCC(uint8_t value)
{
	float r = GetCCMinMax(value, ADSR_RELEASE_MIN, ADSR_RELEASE_MAX);
	if (r == ADSRRelease)
	{
		return;
	}
	ADSRRelease = r;
	log("Release: %d", (uint32_t)(ADSRRelease * 1000));

	for (uint8_t i = 0; i < POLYPHONY; i++)
	{
		adsr[i].SetTime(ADSR_SEG_RELEASE, ADSRRelease); 
	}
}
	
void Voice::SetCC0(uint8_t value)
{	
	switch (currentVoiceSelector)
	{
	case SYNTH_VOICE:
		SetADSRAttackCC(value);
		break;
			
	case SPRING_VOICE:
		// string sustain
		SetAccent((float)value / 127.0f);
		break;
	}
}


void Voice::SetCC1(uint8_t value)
{	
	switch (currentVoiceSelector)
	{
	case SYNTH_VOICE:
		SetADSRDecayCC(value);
		break;
			
	case SPRING_VOICE:
		// string sustain
		SetDamping((float)value / 127.0f);
		break;
	}
}


void Voice::SetCC2(uint8_t value)
{	
	switch (currentVoiceSelector)
	{
	case SYNTH_VOICE:
		SetADSRSustainCC(value);
		break;
			
	case SPRING_VOICE:
		// string sustain
		SetBrightness((float)value / 127.0f);
		break;
	}
}


void Voice::SetCC3(uint8_t value)
{	
	switch (currentVoiceSelector)
	{
	case SYNTH_VOICE:
		SetADSRReleaseCC(value);
		break;
			
	case SPRING_VOICE:
		// string sustain
		SetStructure((float)value / 127.0f);
		break;
	}
}
	