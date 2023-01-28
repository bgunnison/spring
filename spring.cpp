
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

#include <stdio.h>
#include <string.h>
#include "daisy_pod.h"
#include "daisysp.h"
#include "utilities.h"
#include "voice.h"
#include "filter.h"
#include "midimap.h"
#include "controlmap.h"

using namespace daisy;
using namespace daisysp;

#define MIDI_CHANNEL 0
#define AUDIO_BLOCK_SIZE 48

DaisyPod   hw;
CpuLoadMeter loadMeter;
Voices  voice;
Filters filt;
CCMIDIMap ccmap;
PCMIDIMap pcmap;
CCMIDINoteMap noteMap;
ControlMap standAloneController;

float sampleRate;
float finalGainLeft;
float finalGainRight;

uint32_t noteCounter = 0;
uint32_t outClipIndicator = 0;

#define LOG_CPU_LOAD 0
#if LOG_CPU_LOAD
uint8_t currentCpuLoad = 0;
#endif

void logMidiEvent(MidiEvent *m)
{	
	if (m->type == NoteOn)
	{
		NoteOnEvent p = m->AsNoteOn();
		log("%d - Note on:\t%d\t%d\t%d", noteCounter, p.channel, p.note, p.velocity);	
		noteCounter++;
	}
	
	if (m->type == NoteOff)
	{
		NoteOffEvent p = m->AsNoteOff();
		log("%d - Note off:\t%d\t%d\t%d", noteCounter, p.channel, p.note, p.velocity);	
		noteCounter--;
	}
	
	if (m->type == ControlChange)
	{
		ControlChangeEvent p = m->AsControlChange();
		 
		log("%d - CC:\t%d\t%d\t%d", noteCounter, p.channel, p.control_number, p.value);	
	}
	
	if (m->type == ProgramChange)
	{
		ProgramChangeEvent  p = m->AsProgramChange();
		 
		log("%d - PG:\t%d\t%d\t%d", noteCounter, p.channel, p.program);	
	}

	noteCounter++;
}



void UpdateControls()
{
	hw.ProcessAnalogControls();
	hw.ProcessDigitalControls();

	standAloneController.Control();
}


void AudioCallback(AudioHandle::InterleavingInputBuffer  in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
	float sig = 0;
	
#if LOG_CPU_LOAD
	loadMeter.OnBlockStart();
#endif

	UpdateControls();

	for (size_t i = 0; i < size; i += 2)
	{
		sig = filt.Process(voice.Process());
		
		out[i] = sig * finalGainLeft;
	    out[i + 1] = sig * finalGainRight;
		
		if (out[i] > 1.0 || out[i+1] > 1.0)
		{
			outClipIndicator++;
		}
	}
	
#if LOG_CPU_LOAD
	loadMeter.OnBlockEnd();	
	currentCpuLoad = (uint8_t)(loadMeter.GetAvgCpuLoad() * 100);
#endif

}

void HandleMidiMessage(MidiEvent m)
{
	//logMidiEvent(&m);
	//if (m.channel != MIDI_CHANNEL)
	//{
	//	return;
	//}
	
	switch (m.type)
	{
	case NoteOn:
		{
			NoteOnEvent p = m.AsNoteOn();
			BlinkLEDs(50, LED_OFF, LED_OFF);
			p = m.AsNoteOn();
			//log("Note in: %s", GetMidiNoteName(p.note));	
			p.note = noteMap.Process(p.note, true);
			log("Note out: %s", GetMidiNoteName(p.note));	
			if (p.note > 127)
			{
				break; // so we can use notes as control
			}

			voice.NoteOn(&p); 					
		}
		break;

	case NoteOff:
		{
			NoteOffEvent p = m.AsNoteOff();
			p.note = noteMap.Process(p.note, false);
			if (p.note > 127)
			{
				break; // so we can use notes as control
			}
			
			voice.NoteOff(&p);
		}
		break;

	case ControlChange:
		{
			ControlChangeEvent p = m.AsControlChange();
			ccmap.Change(p.control_number, p.value);
			noteMap.Change(p.control_number, p.value);		
		}
		break;
		
	default: 
		break;
	}
}

void SetCCFinalGain(uint8_t value)
{
	finalGainLeft = finalGainRight = float(value) / 127.0f;
}

// Main -- Init, and Midi Handling
int main(void)
{
	// Init
	hw.Init();
	hw.SetAudioBlockSize(AUDIO_BLOCK_SIZE);
	ComInit(&hw);

	//log("Init start");
	
	sampleRate = hw.AudioSampleRate();
	voice.Init(&hw, sampleRate);
	
	filt.Init(&hw, sampleRate);
	
	loadMeter.Init(sampleRate, AUDIO_BLOCK_SIZE);
	
	ccmap.Init();
	pcmap.Init();
	//ccmap.Add(7, SetCCFinalGain);
	// make up your mind
	//SetAlesisV125MIDIMap(&ccmap, &noteMap, &voice, &filt);
	// This setup uses the pod controls to select voice, filter and change parameters via the pots
	// the midi map is selectable via the FCB1010 switches to be major or minor pentatonic scales. 
	// A alelesis samplepad (drumpad) is setupt o play the pentatonic scale with its two upper pads
	// set to the octave up and down select notes (which are not played)
	noteMap.Init();
	noteMap.SetOctaveUpDownNotes(40, 41);
	
	standAloneController.Init(&hw, &voice, &filt, &finalGainRight, &finalGainLeft);
	SetFCB1010MIDIMap(&noteMap);

		
	log("Init end");

	// Start stuff.
	hw.StartAdc();
	hw.StartAudio(AudioCallback);
	hw.midi.StartReceive();

#if LOG_CPU_LOAD
	uint8_t cpuLoad = 0;
#endif
	
	uint32_t now = 0;
	for (;;)
	{
#if LOG_CPU_LOAD		
		uint32_t tp = System::GetNow() - now;
		if (tp > 5000)
		{
			now = System::GetNow();
			loadMeter.Reset();
			cpuLoad = currentCpuLoad;
			log("Ave CPU load Peak: %d", cpuLoad);
		}
#endif

		hw.midi.Listen();
		// Handle MIDI Events
		while (hw.midi.HasEvents())
		{
			HandleMidiMessage(hw.midi.PopEvent());
		}
				
		//voice.UpdateBackGround();
		if (outClipIndicator > 0)
		{
			hw.seed.SetLed(true);
			outClipIndicator--;
		}
		else
		{
			hw.seed.SetLed(false);
		}
		
		ResetLEDs();
	}
}
