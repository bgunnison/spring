
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
#include <stdint.h>
#include "midimap.h"

MIDINoteMap AMajorPentNoteMap(MIDINoteMap::NOTE_MAP_MAJOR_PENTATONIC, MIDINoteMap::A_ROOT_NOTE);
MIDINoteMap BMajorPentNoteMap(MIDINoteMap::NOTE_MAP_MAJOR_PENTATONIC, MIDINoteMap::B_ROOT_NOTE);
MIDINoteMap CMajorPentNoteMap(MIDINoteMap::NOTE_MAP_MAJOR_PENTATONIC, MIDINoteMap::C_ROOT_NOTE);
MIDINoteMap DMajorPentNoteMap(MIDINoteMap::NOTE_MAP_MAJOR_PENTATONIC, MIDINoteMap::D_ROOT_NOTE);
MIDINoteMap EMajorPentNoteMap(MIDINoteMap::NOTE_MAP_MAJOR_PENTATONIC, MIDINoteMap::E_ROOT_NOTE);

MIDINoteMap AMinorPentNoteMap(MIDINoteMap::NOTE_MAP_MINOR_PENTATONIC, MIDINoteMap::A_ROOT_NOTE);
MIDINoteMap BMinorPentNoteMap(MIDINoteMap::NOTE_MAP_MINOR_PENTATONIC, MIDINoteMap::B_ROOT_NOTE);
MIDINoteMap CMinorPentNoteMap(MIDINoteMap::NOTE_MAP_MINOR_PENTATONIC, MIDINoteMap::C_ROOT_NOTE);
MIDINoteMap DMinorPentNoteMap(MIDINoteMap::NOTE_MAP_MINOR_PENTATONIC, MIDINoteMap::D_ROOT_NOTE);
MIDINoteMap EMinorPentNoteMap(MIDINoteMap::NOTE_MAP_MINOR_PENTATONIC, MIDINoteMap::E_ROOT_NOTE);

void SetFCB1010MIDIMap(CCMIDINoteMap *noteMap)
{
	
	// via the FCB-UNO Control Center
	// page 00, maps notes to pentatonic major scale
	noteMap->Add(22, &AMajorPentNoteMap); 
	noteMap->Add(21, &BMajorPentNoteMap); 
	noteMap->Add(20, &CMajorPentNoteMap); 
	noteMap->Add(19, &DMajorPentNoteMap); 
	noteMap->Add(18, &EMajorPentNoteMap); 
	
	noteMap->Add(17, &AMinorPentNoteMap); 
	noteMap->Add(16, &BMinorPentNoteMap); 
	noteMap->Add(15, &CMinorPentNoteMap); 
	noteMap->Add(14, &DMinorPentNoteMap); 
	noteMap->Add(13, &EMinorPentNoteMap); 

}


void SetAlesisV125MIDIMap(CCMIDIMap *ccmap, CCMIDINoteMap *noteMap, CCMIDIMapable *voice, CCMIDIMapable *filt)
{
	// buttons
	
	// 48 voice select
	ccmap->Add(10, 48, voice); // voice 1
	ccmap->Add(11, 49, voice);
	ccmap->Add(12, 50, voice);
	ccmap->Add(13, 51, voice);
	ccmap->Add(14, 52, voice);
	
	// 56 filter select
	ccmap->Add(10, 56, filt); // no filt
	ccmap->Add(11, 57, filt); // filt 1
	ccmap->Add(12, 58, filt); // filt 2
	
	// 64 note map select
	noteMap->Add(64, &DMajorPentNoteMap); //button CC 50 to D major pentatonic
	
	// knobs
	ccmap->Add(0, 20, filt); // freq to CC 20
	ccmap->Add(1, 21, filt); // resonance to CC 21
	
	// voice parameter change
	ccmap->Add(0, 22, voice); //voice 0 to CC 22
	ccmap->Add(1, 23, voice); 
	ccmap->Add(2, 24, voice); 
	ccmap->Add(3, 25, voice); 
	ccmap->Add(4, 26, voice); 
	ccmap->Add(5, 27, voice); 
}
