#pragma once

#include "daisy_pod.h"
#include "daisysp.h"

#include "midimap.h"


using namespace daisy;
using namespace daisysp;
// springvoice is 92% CPU usage constant with moog filter, overload (no midi) if set to 5. 
#define SPRING_VOICE_POLYPHONY	4
#define MALLET_VOICE_POLYPHONY	2 // 98% with moog filter
#define OSC_VOICE_POLYPHONY		8 // 18% cpu usage with moog filter
// we have to instantiate max
#define MAX_POLYPHONY			OSC_VOICE_POLYPHONY

typedef struct
{	
	uint8_t midiNote; // > 0 if playing
	float amplitude; // start gain
	
}Note;

class NullVoice
{
public:
	virtual void Init(float SR);
	virtual float Process();
	
	virtual void NoteOn(NoteOnEvent *p);
	virtual void NoteOff(NoteOffEvent *p);

	virtual void SetCC0(uint8_t value);
	virtual void SetCC1(uint8_t value);
	virtual void SetCC2(uint8_t value);
	virtual void SetCC3(uint8_t value);
	
	virtual void Panic();
	
protected:
	
	float sampleRate;
	uint8_t polyphony;
	Note notes[MAX_POLYPHONY];
};


class OscVoice : public NullVoice
{
public:
	void Init(float SR) override;
	float Process() override;
	
	void NoteOn(NoteOnEvent *p) override;
	void NoteOff(NoteOffEvent *p) override;
	
	void SetCC0(uint8_t value) override;
	void SetCC1(uint8_t value) override;
	void SetCC2(uint8_t value) override;
	void SetCC3(uint8_t value) override;
	
	void Panic() override;
	
private:
	Oscillator synth[MAX_POLYPHONY];
	Adsr adsr[MAX_POLYPHONY]; 
	
	void StartNote(uint8_t i, NoteOnEvent *p);

	bool ADSROn;
	void SetADSRAttackCC(uint8_t value);
	float ADSRAttack;
	void SetADSRDecayCC(uint8_t value);
	float ADSRDecay;
	void SetADSRSustainCC(uint8_t value);
	float ADSRSustain;
	void SetADSRReleaseCC(uint8_t value);
	float ADSRRelease;
	
};


class SpringVoice : public NullVoice
{
public:
	void Init(float SR) override;
	float Process() override;
	
	virtual void NoteOn(NoteOnEvent *p) override;
	virtual void NoteOff(NoteOffEvent *p) override;
	
	void SetCC0(uint8_t value) override;
	void SetCC1(uint8_t value) override;
	void SetCC2(uint8_t value) override;
	void SetCC3(uint8_t value) override;
	
	void Panic() override;
	
private:
	
	StringVoice spring[MAX_POLYPHONY];
	
	void StartNote(uint8_t i, NoteOnEvent *p);
	
	void SetAccentCC(uint8_t value);
	float accent; // string voice only
	void SetDampingCC(uint8_t value);
	float damping; //string voice only
	void SetStructureCC(uint8_t value);
	float structure;
	void SetBrightnessCC(uint8_t value);
	float brightness;

};


class MalletVoice : public NullVoice
{
public:
	void Init(float SR) override;
	float Process() override;
	
	virtual void NoteOn(NoteOnEvent *p) override;
	virtual void NoteOff(NoteOffEvent *p) override;
	
	void SetCC0(uint8_t value) override;
	void SetCC1(uint8_t value) override;
	void SetCC2(uint8_t value) override;
	void SetCC3(uint8_t value) override;
	
	void Panic() override;
	
private:
	
	ModalVoice mallet[MAX_POLYPHONY];
	
	void StartNote(uint8_t i, NoteOnEvent *p);
	
	void SetAccentCC(uint8_t value);
	float accent; // string voice only
	void SetDampingCC(uint8_t value);
	float damping; //string voice only
	void SetStructureCC(uint8_t value);
	float structure;
	void SetBrightnessCC(uint8_t value);
	float brightness;

};


// a container for voices

class Voices : public CCMIDIMapable
{
	public:
	Voices() {}
		
	void Init(DaisyPod *pod, float SR); 
	
	// 1/-1 rotates thru voices, 0 does nothing
	void Select(int8_t sel);
	
	float Process(void);
	
	void UpdateBackGround(void);
	
	void NoteOn(NoteOnEvent *p);
	void NoteOff(NoteOffEvent *p);
	
	void SetCC0(uint8_t value);
	void SetCC1(uint8_t value);
	void SetCC2(uint8_t value);
	void SetCC3(uint8_t value);
	
	void CCProcess(uint8_t ccFuncNumber, uint8_t value);

	
	private:
	
	DaisyPod *phw;
	float sampleRate;
	
	void Panic(void);
	
	uint8_t currentVoiceSelector;
	
	typedef enum
	{
		SYNTH_VOICE,
		SPRING_VOICE,
		MALLET_VOICE
	}VOICE_TYPE;
	
	#define NUM_VOICES 3
	
	void ChangeVoice(uint8_t sel);

	
	OscVoice	oscVoice;
	SpringVoice springVoice;
	MalletVoice malletVoice;
	
	NullVoice *pvoice;
	
};

