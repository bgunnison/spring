

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

#include <stdint.h>
#include "utilities.h"

// A class inherits from this the method CCProcess which maps a enumerated CC function which is then called with the CC value
class CCMIDIMapable
{
public:
	// Pass the enumerated CC function in the mapable class and it processes the CC value
	virtual void CCProcess(uint8_t ccFuncNumber, uint8_t value) {}
};

// control change midi map
// maps CC numbers to enumerated CC functions in a class that inherits from CCMIDIMapable
class CCMIDIMap
{
public:
	// set all maps to NULL
	void Init() { for (uint8_t i = 0; i < 127; i++) map[i].mapobj = NULL; }
	// Add a map with the enumerated CC function number, the CC number to map to and the mapable class
	void Add(uint8_t ccFuncNumber, uint8_t cc, CCMIDIMapable *mapable) 
	{
		if (cc >= 127)
		{
			return;
		}
		map[cc].funcNumber = ccFuncNumber;
		map[cc].mapobj = mapable; 
	}
	
	// called with every CC event to see if its mapped and send the value to the enumerated function in the mapable class. 
	void Change(uint8_t cc, uint8_t value)
	{
		if (cc >= 127 || map[cc].mapobj == NULL) 
		{
			return;
		}
		
		// call the CCProcess that overrode the CCMIDIMapable
		map[cc].mapobj->CCProcess(map[cc].funcNumber, value);
	}
	
private:
	typedef struct
	{
		uint8_t funcNumber; // enumerated CC func in mapable class
		CCMIDIMapable *mapobj;	// mapable instantiation
	}CCMap;
	
	CCMap map[127];
	
};

// program change midi map
class PCMIDIMap
{
public:
	typedef void(*PCFunc)(void);
	void Init() { for (uint8_t i = 0; i < 127; i++) map[i] = NULL; }
	void Add(uint8_t pc, PCFunc func) {if (pc >= 127) return; map[pc] = func; }
	void Invoke(uint8_t pc)
	{
		if (pc >= 127 || map[pc] == NULL) 
		{
			return;
		}
		
		map[pc]();
	}
	
private:
	
	PCFunc map[127];
	
};




class MIDINoteMap
{
public:
	typedef enum
	{
		NOTE_MAP_NONE = 0,
		NOTE_MAP_MAJOR_PENTATONIC,
		NOTE_MAP_MINOR_PENTATONIC
	}NOTE_MAP_TYPE;
	
	typedef enum
	{
		C_ROOT_NOTE  = 0,
		CS_ROOT_NOTE,	
		D_ROOT_NOTE,	
		DS_ROOT_NOTE,
		E_ROOT_NOTE,
		F_ROOT_NOTE,
		FS_ROOT_NOTE,
		G_ROOT_NOTE,
		GS_ROOT_NOTE,
		A_ROOT_NOTE,
		AS_ROOT_NOTE,
		B_ROOT_NOTE,
	}MIDI_ROOT_NOTE;

	MIDINoteMap(NOTE_MAP_TYPE map, MIDI_ROOT_NOTE r)
	{
		Init();
		map_type = map; 
		root = r;
	}
	
	void Init() { octave = 0; map_type = NOTE_MAP_NONE; root = C_ROOT_NOTE; }	
	void SetType(NOTE_MAP_TYPE nmt) { map_type = nmt; }
	void SetRoot(MIDI_ROOT_NOTE rootNote) { root = rootNote; }
	void SetOctave(uint8_t s) { octave = s; }

		
	uint8_t Process(uint8_t noteIn)
	{
		switch (map_type)
		{
		default:
		case NOTE_MAP_NONE:
			return noteIn;
			
		case NOTE_MAP_MAJOR_PENTATONIC:
		case NOTE_MAP_MINOR_PENTATONIC:
			{
				uint8_t i = (noteIn - root) % 12;
				if (i < 0) 
				{
					i = 0;
				}
				
				uint8_t noteOut;

				if (map_type == NOTE_MAP_MAJOR_PENTATONIC)
				{
					noteOut = noteIn + major_pent_shift[i];					
				}
				
				if (map_type == NOTE_MAP_MINOR_PENTATONIC)
				{
					noteOut = noteIn + minor_pent_shift[i];					
				}
				
				if (noteOut < 0)
				{
					noteOut = root;
				}
				
				noteOut += (octave * 12);
				if (noteOut > 127)
				{
					noteOut = 127;
				}
				
				log("noteIn: %d, NoteOut: %d", noteIn, noteOut);
				return noteOut;
			}
		}
	}

private:
	NOTE_MAP_TYPE map_type;
	uint8_t root;
	int8_t octave; // 0 is no octave shift
	// major pent: root, 2nd, 3rd, 5th, and 6th intervals
    // chromatic                    C	 C#	 D	D#	E	F  F#  G  G#  A	 A#	  B
    // CMaj pentatonic              C	 C	 D	D	E	E  E   G  G  A	 A	  
	const int8_t major_pent_shift[12] = {0, -1, 0, -1, 0, -1, -2, 0, -1, 0, -1, +1 }; // semi tones shift
	const int8_t minor_pent_shift[12] = {0, -1, 1,  0, 1,  0, -1, 0, -1, 1,  0,  1 }; // semi tones shift
  
};


// maps cc to a scale map
class CCMIDINoteMap
{
public:

	void Init() { for (uint8_t i = 0; i < 127; i++) map[i] = NULL; }
	void Add(uint8_t cc, MIDINoteMap *noteMap) {if (cc >= 127) return; map[cc] = noteMap; }
	void Change(uint8_t cc, uint8_t value)
	{
		if (cc >= 127) 
		{
			return;
		}
		
		if (cc == setCC && value == 0)
		{
			setCC = 0; // turn map off
		}
		else
		{
			setCC = cc;			
		}
	}
	
	void OctaveShift(uint8_t s)
	{
		if (map[setCC] != NULL)
		{
			map[setCC]->SetOctave(s);
		}
	}
	
	uint8_t Process(uint8_t  NoteIn)
	{
		if (map[setCC] == NULL)
		{
			return NoteIn;
		}
		
		return map[setCC]->Process(NoteIn);
	}
	
private:
	
	MIDINoteMap *map[127];
	uint8_t setCC;
	
};

void SetAlesisV125MIDIMap(CCMIDIMap *ccmap, CCMIDINoteMap *noteMap, CCMIDIMapable *voice, CCMIDIMapable *filt);
void SetFCB1010MIDIMap(CCMIDINoteMap *noteMap);


