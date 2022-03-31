#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

#define POLYPHONY 4


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
	
	Note notes[POLYPHONY];


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
	Oscillator synth[POLYPHONY];
	Adsr adsr[POLYPHONY]; 
	
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
	
	StringVoice spring[POLYPHONY];
	
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

class Voices
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
	
	private:
	
	DaisyPod *phw;
	float sampleRate;
	
	void Panic(void);
	
	uint8_t currentVoiceSelector;
	
	typedef enum
	{
		SYNTH_VOICE,
		SPRING_VOICE
	}VOICE_TYPE;
	
	#define NUM_VOICES 2
	
	OscVoice	oscVoice;
	SpringVoice springVoice;
	
	NullVoice *pvoice;
	
};

