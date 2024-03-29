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

#include "daisy_pod.h"
#include "daisysp.h"

#include "midimap.h"


using namespace daisy;
using namespace daisysp;
// springvoice is 92% CPU usage constant with moog filter, overload (no midi) if set to 5. 
#define SPRING_VOICE_POLYPHONY	4
#define MALLET_VOICE_POLYPHONY	2 // 98% with moog filter
#define OSC_VOICE_POLYPHONY		8 // 18% cpu usage with moog filter
#define HIHAT_VOICE_POLYPHONY   2
#define FORMANT_VOICE_POLYPHONY 8
#define NOISE_VOICE_POLYPHONY	8
// we have to instantiate max
#define MAX_POLYPHONY			OSC_VOICE_POLYPHONY


// ADSR settings
#define ADSR_ATTACK_MIN			0.01f
#define ADSR_ATTACK_DEFAULT		0.1f
#define ADSR_ATTACK_MAX			5.0f
#define ADSR_DECAY_MIN			0.01f
#define ADSR_DECAY_DEFAULT		0.5f
#define ADSR_DECAY_MAX			5.0f
#define ADSR_SUSTAIN_MIN		0.01f
#define ADSR_SUSTAIN_DEFAULT	0.0f
#define ADSR_SUSTAIN_MAX		1.0f
#define ADSR_RELEASE_MIN		0.01f
#define ADSR_RELEASE_DEFAULT	0.2f
#define ADSR_RELEASE_MAX		1.0f



typedef struct
{	
	uint8_t midiNote; // > 0 if playing
	float amplitude; // start gain
	
}Note;

class NullVoice
{
public:
	virtual void Init(DaisyPod *phw, float SR);
	virtual float Process();
	
	virtual void NoteOn(NoteOnEvent *p);
	virtual void NoteOff(NoteOffEvent *p);
	virtual void SetFreq(float freq);

	virtual void SetCC0(uint8_t value);
	virtual void SetCC1(uint8_t value);
	virtual void SetCC2(uint8_t value);
	virtual void SetCC3(uint8_t value);
	virtual void SetCC4(uint8_t value){}
	virtual void SetCC5(uint8_t value){}
	
	virtual void ProcessParm0();
	virtual void ProcessParm1();
	virtual void ProcessParm2();
	virtual void ProcessParm3();
	virtual void ProcessParm4(){}
	virtual void ProcessParm5(){}
	
	virtual void Panic();
	
protected:
	
	float sampleRate;
	uint8_t polyphony;
	Note notes[MAX_POLYPHONY];
	DaisyPod *hw;
};


class OscVoice : public NullVoice
{
public:
	void Init(DaisyPod *phw, float SR) override;
	float Process() override;
	
	void NoteOn(NoteOnEvent *p) override;
	void NoteOff(NoteOffEvent *p) override;
	void SetFreq(float freq) override;
	
	void SetCC0(uint8_t value) override;
	void SetCC1(uint8_t value) override;
	void SetCC2(uint8_t value) override;
	void SetCC3(uint8_t value) override;
	
	void ProcessParm0() override;
	void ProcessParm1() override;
	void ProcessParm2() override;
	void ProcessParm3() override;

	void Panic() override;
	
private:
	Oscillator synth[MAX_POLYPHONY];
	Adsr adsr[MAX_POLYPHONY]; 
	
	void StartNote(uint8_t i, NoteOnEvent *p);

	bool ADSROn;
	
	void SetADSRAttack(float v);
	float ADSRAttack;
	void SetADSRDecay(float v);
	float ADSRDecay;
	void SetADSRSustain(float v);
	float ADSRSustain;
	void SetADSRRelease(float v);
	float ADSRRelease;
	
	Parameter ADSRAttackPotParm; // sets range and plot 
	Parameter ADSRDecayPotParm; // sets range and plot 
	Parameter ADSRSustainPotParm; // sets range and plot 
	Parameter ADSRReleasePotParm; // sets range and plot 
	
};


class SpringVoice : public NullVoice
{
public:
	void Init(DaisyPod *phw, float SR) override;
	float Process() override;
	
	void NoteOn(NoteOnEvent *p) override;
	void NoteOff(NoteOffEvent *p) override;
	void SetFreq(float freq) override;

	void SetCC0(uint8_t value) override;
	void SetCC1(uint8_t value) override;
	void SetCC2(uint8_t value) override;
	void SetCC3(uint8_t value) override;
	
	void ProcessParm0() override;
	void ProcessParm1() override;
	void ProcessParm2() override;
	void ProcessParm3() override;
	
	void Panic() override;
	
private:
	
	StringVoice spring[MAX_POLYPHONY];
	
	void StartNote(uint8_t i, NoteOnEvent *p);
	
	void SetDamping(float v);
	float damping; 
	void SetStructure(float v);
	float structure;
	void SetBrightness(float v);
	float brightness;
	void SetAccent(float v);
	float accent; 

	
	Parameter DampingPotParm; // sets range and plot 
	Parameter StructurePotParm; // sets range and plot 
	Parameter BrightnessPotParm; // sets range and plot 
	Parameter AccentPotParm; // sets range and plot 

};


class MalletVoice : public NullVoice
{
public:
	void Init(DaisyPod *phw, float SR) override;
	float Process() override;
	
	virtual void NoteOn(NoteOnEvent *p) override;
	virtual void NoteOff(NoteOffEvent *p) override;
	virtual void SetFreq(float freq) override;

	void SetCC0(uint8_t value) override;
	void SetCC1(uint8_t value) override;
	void SetCC2(uint8_t value) override;
	void SetCC3(uint8_t value) override;
	
	void ProcessParm0() override;
	void ProcessParm1() override;
	void ProcessParm2() override;
	void ProcessParm3() override;
	
	void Panic() override;
	
private:
	
	ModalVoice mallet[MAX_POLYPHONY];
	
	void StartNote(uint8_t i, NoteOnEvent *p);
	
	void SetDamping(float v);
	float damping; 
	void SetStructure(float v);
	float structure;
	void SetBrightness(float v);
	float brightness;
	void SetAccent(float v);
	float accent; 
	
	Parameter DampingPotParm; // sets range and plot 
	Parameter StructurePotParm; // sets range and plot 
	Parameter BrightnessPotParm; // sets range and plot 
	Parameter AccentPotParm; // sets range and plot 


};


class HiHatVoice : public NullVoice
{
public:
	void Init(DaisyPod *phw, float SR) override;
	float Process() override;
	
	virtual void NoteOn(NoteOnEvent *p) override;
	virtual void NoteOff(NoteOffEvent *p) override;
	virtual void SetFreq(float freq) override;

	void SetCC0(uint8_t value) override;
	void SetCC1(uint8_t value) override;
	void SetCC2(uint8_t value) override;
	void SetCC3(uint8_t value) override;
	
	void ProcessParm0() override;
	void ProcessParm1() override;
	void ProcessParm2() override;
	void ProcessParm3() override;
	
	void Panic() override;
	
private:
	// or <RingModNoise> - This is much more hihat, but much less tonal
	HiHat<SquareNoise> hihat[MAX_POLYPHONY];
	
	void StartNote(uint8_t i, NoteOnEvent *p);
	
	void SetAccentCC(uint8_t value);
	float accent; 
	void SetSustainCC(uint8_t value); // bool
	bool sustain; 
	void SetDecayCC(uint8_t value);
	float decay; 
	void SetToneCC(uint8_t value);
	float tone;
	void SetNoisinessCC(uint8_t value);
	float noisiness;

};


class FormantVoice : public NullVoice
{
public:
	void Init(DaisyPod *phw, float SR) override;
	float Process() override;
	
	virtual void NoteOn(NoteOnEvent *p) override;
	virtual void NoteOff(NoteOffEvent *p) override;
	virtual void SetFreq(float freq) override;

	void SetCC0(uint8_t value) override;
	void SetCC1(uint8_t value) override;
	void SetCC2(uint8_t value) override;
	void SetCC3(uint8_t value) override;
	void SetCC4(uint8_t value) override;
	void SetCC5(uint8_t value) override;
	
	void ProcessParm0() override;
	void ProcessParm1() override;
	void ProcessParm2() override;
	void ProcessParm3() override;
	
	void Panic() override;
	
private:
	FormantOscillator formant[MAX_POLYPHONY];
	Adsr adsr[MAX_POLYPHONY]; 

	void StartNote(uint8_t i, NoteOnEvent *p);
	
	bool ADSROn;
	
	void SetADSRAttack(float v);
	float ADSRAttack;
	void SetADSRDecay(float v);
	float ADSRDecay;
	void SetADSRSustain(float v);
	float ADSRSustain;
	void SetADSRRelease(float v);
	float ADSRRelease;
	
	Parameter ADSRAttackPotParm; // sets range and plot 
	Parameter ADSRDecayPotParm; // sets range and plot 
	Parameter ADSRSustainPotParm; // sets range and plot 
	Parameter ADSRReleasePotParm; // sets range and plot 

	void SetFormantFreqCC(uint8_t value);
	float formantFreq; 
	void SetPhaseShiftCC(uint8_t value); 
	float phaseShift; 

};


// copied from DaisySP
class MyWhiteNoise
{
public:
	
	/** Initializes the WhiteNoise object
	*/
	void Init(int32_t seed)
	{
		amp_      = 1.0f;
		randseed_ = seed;
	}

	/** sets the amplitude of the noise output
	*/
	inline void SetAmp(float a) { amp_ = a; }
	/** returns a new sample of noise in the range of -amp_ to amp_
	*/
	inline float Process()
	{
		randseed_ *= 16807;
		return (randseed_ * coeff_) * amp_;
	}

private:
	static constexpr float coeff_ = 4.6566129e-010f;
	float                  amp_;
	int32_t                randseed_;
};

// we get some tonality by setting the filter freq to the note
// noise -> filter -> filter
class NoiseFilter 
{
public:
	void Init(float sampleRate, int32_t seed);
	float Process(float adsrLevel);
	void SetAmp(float amp) { wn.SetAmp(amp); }
	void SetFreq(float freq);
	void SetResonance(float Res);
	void SetDrive(float drive);
	
private:
	float	sampleRate;
	MyWhiteNoise wn;
	Svf		hpFilter;	// set cutoff an octave below note to prevent rumble
	static constexpr uint8_t NUM_FILTERS = 3;
	Svf		filter[NUM_FILTERS];
	float	resGain; // as the resonance goes up the gain goes down
};

class NoiseVoice : public NullVoice
{
public:
	void Init(DaisyPod *phw, float SR) override;
	float Process() override;
	
	void NoteOn(NoteOnEvent *p) override;
	void NoteOff(NoteOffEvent *p) override;
	void SetFreq(float freq) override;

	void SetCC0(uint8_t value) override;
	void SetCC1(uint8_t value) override;
	void SetCC2(uint8_t value) override;
	void SetCC3(uint8_t value) override;
	
	void ProcessParm0() override;
	void ProcessParm1() override;
	void ProcessParm2() override;
	void ProcessParm3() override;
	void ProcessParm4() override;
	void ProcessParm5() override;

	
	void Panic() override;
	
private:
	NoiseFilter noise[MAX_POLYPHONY];
	Adsr adsr[MAX_POLYPHONY]; 
	
	void StartNote(uint8_t i, NoteOnEvent *p);
	
	void SetResonance(float v);
	float resonance;
	void SetDrive(float v);
	float drive;
	
	bool ADSROn;

	void SetADSRAttack(float v);
	float ADSRAttack;
	void SetADSRDecay(float v);
	float ADSRDecay;
	void SetADSRSustain(float v);
	float ADSRSustain;
	void SetADSRRelease(float v);
	float ADSRRelease;
	
	Parameter ADSRAttackPotParm; // sets range and plot 
	Parameter ADSRDecayPotParm; // sets range and plot 
	Parameter ADSRSustainPotParm; // sets range and plot 
	Parameter ADSRReleasePotParm; // sets range and plot 
	Parameter ResonancePotParm; // sets range and plot 
	Parameter DrivePotParm; // sets range and plot 


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
	void SetFreq(float freq);

	void SetCC0(uint8_t value);
	void SetCC1(uint8_t value);
	void SetCC2(uint8_t value);
	void SetCC3(uint8_t value);
	
	void CCProcess(uint8_t ccFuncNumber, uint8_t value);

	void ProcessParm0();
	void ProcessParm1();
	void ProcessParm2();
	void ProcessParm3();

	
	private:
	
	DaisyPod *phw;
	float sampleRate;
	
	void Panic(void);
	
	uint8_t currentVoiceSelector;
	
	typedef enum
	{
		SYNTH_VOICE,
		SPRING_VOICE,
		MALLET_VOICE,
		FORMANT_VOICE,
		NOISE_VOICE
	}VOICE_TYPE;
	
	#define NUM_VOICES 5
	
	void ChangeVoice(uint8_t sel);

	
	OscVoice	oscVoice;
	SpringVoice springVoice;
	MalletVoice malletVoice;
	FormantVoice  formantVoice;
	NoiseVoice  noiseVoice;
	
	NullVoice *pvoice;
	
};

