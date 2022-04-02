#include "daisy_pod.h"
#include "daisysp.h"
#include <stdio.h>
#include <string.h>

#include "utilities.h"
#include "voice.h"
#include "filter.h"

using namespace daisy;
using namespace daisysp;

#define MIDI_CHANNEL 0
#define AUDIO_BLOCK_SIZE 48

DaisyPod   hw;

Voices  voice;
Filters filt;

float sampleRate;
float finalGain;

uint32_t noteCounter = 0;
uint32_t outClipIndicator = 0;

CpuLoadMeter loadMeter;
uint8_t currentCpuLoad = 0;

Parameter volumePot;


void logMidiNote(MidiEvent *m)
{	
	log("%d - Note Received:\t%d\t%d\t%d", noteCounter, m->channel, m->data[0], m->data[1]);
	noteCounter++;
}


void UpdateControls()
{
	hw.ProcessAnalogControls();
	hw.ProcessDigitalControls();

	// returns 0 - max
	finalGain = volumePot.Process();
	
	//Returns + 1 if the encoder was turned clockwise, -1 if it was turned counter - clockwise, or 0 if it was not just turned.
	//voice.Select(hw.encoder.Increment());	}
	
	// use button1 for voice select
	voice.Select(hw.button1.RisingEdge());
	filt.Select(hw.button2.RisingEdge());
}


void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
	AudioHandle::InterleavingOutputBuffer out,
	size_t                                size)
{
	float sig = 0;
	loadMeter.OnBlockStart();
	
	UpdateControls();

	for (size_t i = 0; i < size; i += 2)
	{
		sig = filt.Process(voice.Process()) * finalGain; 
		
		if (sig > 1.0)
		{
			outClipIndicator++;
		}
		
		out[i] = out[i + 1] = sig;
	}
	
	loadMeter.OnBlockEnd();
	
	currentCpuLoad = (uint8_t)(loadMeter.GetAvgCpuLoad() * 100);

}

// Typical Switch case for Message Type.
void HandleMidiMessage(MidiEvent m)
{
	if (m.channel != MIDI_CHANNEL)
	{
		return;
	}
	
	switch (m.type)
	{
	case NoteOn:
		{
			//logMidiNote(&m);
			NoteOnEvent p = m.AsNoteOn();
			
    		RotateLED(2);

			// This is to avoid Max/MSP Note outs for now..
			if (m.data[1] != 0)
			{
				p = m.AsNoteOn();
				voice.NoteOn(&p); 
			}
		}
		break;

	case NoteOff:
		//logMidiNote(&m);
		if (m.data[1] != 0)
		{
			NoteOffEvent p = m.AsNoteOff();
			voice.NoteOff(&p);
		}
		break;

	case ControlChange:
		{
			ControlChangeEvent p = m.AsControlChange();
			//log("CC: %d, v: %d", p.control_number, p.value);
			
			switch (p.control_number)
			{
			case 7:
				finalGain = float(p.value) / 127.0f;
				break;
				
			case 20:
				filt.SetCC0(p.value);
				break;
				
			case 21:
				// CC res.
				filt.SetCC1(p.value);
				break;
				
			case 22:
				voice.SetCC0(p.value);
				break;

			case 23:
				voice.SetCC1(p.value);
				break;
				
			case 24:
				voice.SetCC2(p.value);
				break;
				
			case 25:
				voice.SetCC3(p.value);
				break;

				
			default: 
				break;
			}
			break;
		}
	default: break;
	}
}



// Main -- Init, and Midi Handling
int main(void)
{
	// Init
	hw.Init();
	hw.SetAudioBlockSize(AUDIO_BLOCK_SIZE);
	hw.seed.usb_handle.Init(UsbHandle::FS_INTERNAL);
	System::Delay(250);

	//log("Init start");
	
	sampleRate = hw.AudioSampleRate();
	voice.Init(&hw, sampleRate);
	
	filt.Init(sampleRate);
	
	loadMeter.Init(sampleRate, AUDIO_BLOCK_SIZE);
	
	volumePot.Init(hw.knob1, 0, 8, Parameter::LOGARITHMIC);
	
	finalGain = 1.0;
	
	log("Init end");

	// Start stuff.
	hw.StartAdc();
	hw.StartAudio(AudioCallback);
	hw.midi.StartReceive();

	uint8_t cpuLoad = 0;
	
	uint32_t now = 0;
	for (;;)
	{
		
		uint32_t tp = System::GetNow() - now;
		if (tp > 5000)
		{
			now = System::GetNow();
			loadMeter.Reset();
			cpuLoad = currentCpuLoad;
			log("Ave CPU load Peak: %d", cpuLoad);
		}
		
		hw.midi.Listen();
		// Handle MIDI Events
		while (hw.midi.HasEvents())
		{
			HandleMidiMessage(hw.midi.PopEvent());
		}
				
		voice.UpdateBackGround();
		if (outClipIndicator > 0)
		{
			hw.seed.SetLed(true);
			outClipIndicator--;
		}
		else
		{
			hw.seed.SetLed(false);
		}
		
	}
}
