#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;


typedef struct
{	
	uint8_t midiNote; // > 0 if playing
	float amplitude; // start gain
	
}Note;

// a container for voices

class Voice
{
	public:
	Voice() {}
		
	void Init(DaisyPod *pod, float SR); 
	
	// 1/-1 rotates thru voices, 0 does nothing
	void Select(int8_t sel);
	
	float Process(void);
	
	void UpdateBackGround(void);
	
	void NoteOn(NoteOnEvent *p);
	void NoteOff(NoteOffEvent *p);
	
	void SetAccent(float set);
	
	void SetDamping(float set);


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
	#define POLYPHONY 4
	
	Note notes[POLYPHONY];

	Oscillator synth[POLYPHONY];
	StringVoice spring[POLYPHONY];
	float accent; // string voice only
	float damping; //string voice only
	
};

