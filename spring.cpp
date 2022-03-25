#include "daisy_pod.h"
#include "daisysp.h"
#include <stdio.h>
#include <string.h>

#include "utilities.h"
#include "voice.h"

using namespace daisy;
using namespace daisysp;

DaisyPod   hw;

Voice voice;
Svf        filt;

float sampleRate;
float finalGain;

uint32_t noteCounter = 0;


void logMidiNote(MidiEvent *m)
{	
	log("%d - Note Received:\t%d\t%d\t%d\r\n", noteCounter, m->channel, m->data[0], m->data[1]);
	noteCounter++;
}


void UpdateControls()
{
	hw.ProcessAnalogControls();
	hw.ProcessDigitalControls();

	// returns 0 - 1.0
	finalGain = hw.knob1.Process();
	//float max_f = sampleRate / 3.0;
	//filt.SetFreq(k1 * max_f);
	
	//float k2 = hw.knob2.Process();
	//filt.SetRes(k2);
	
	//voice.SetAccent(k2);
	
	//Returns + 1 if the encoder was turned clockwise, -1 if it was turned counter - clockwise, or 0 if it was not just turned.
	voice.Select(hw.encoder.Increment());	
}


void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
	AudioHandle::InterleavingOutputBuffer out,
	size_t                                size)
{
	float sig = 0;
	
	UpdateControls();

	for (size_t i = 0; i < size; i += 2)
	{
		sig = voice.Process();		
		filt.Process(sig); 
		out[i] = out[i + 1] = filt.Low() * finalGain;
	}
}

// Typical Switch case for Message Type.
void HandleMidiMessage(MidiEvent m)
{
	
	switch (m.type)
	{
	case NoteOn:
		{
			logMidiNote(&m);
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
				{
					// CC cutoff.
					//f must be between 0.0 and sample_rate / 3
					float f = (float)p.value / 127.0f * sampleRate / 3.0f;
					filt.SetFreq(f);
					break;
				}
				
			case 21:
				// CC res.
				filt.SetRes(((float)p.value / 127.0f));
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
	hw.SetAudioBlockSize(4);
	hw.seed.usb_handle.Init(UsbHandle::FS_INTERNAL);
	System::Delay(250);

	//log("Init start");
	
	sampleRate = hw.AudioSampleRate();
	voice.Init(&hw, sampleRate);
	
	filt.Init(sampleRate);
	filt.SetFreq(sampleRate / 3.0f);
	filt.SetRes(0.2);
	
	finalGain = 1.0;
	
	//log("Init end");

	// Start stuff.
	hw.StartAdc();
	hw.StartAudio(AudioCallback);
	hw.midi.StartReceive();

	
	for (;;)
	{
		hw.midi.Listen();
		// Handle MIDI Events
		while (hw.midi.HasEvents())
		{
			HandleMidiMessage(hw.midi.PopEvent());
		}
				
		voice.UpdateBackGround();
	}
}
