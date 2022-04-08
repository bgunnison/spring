
#include <stdint.h>
#include "midimap.h"


MIDINoteMap DMajorPentNoteMap(MIDINoteMap::NOTE_MAP_MAJOR_PENTATONIC, D_ROOT_NOTE);

void SetFCB1010MIDIMap(CCMIDINoteMap *noteMap, CCMIDIMapable *voice, CCMIDIMapable *filt)
{
	
	// via the FCB-UNO Control Center
	// page 00, maps notes to pentatonic major scale
	// CC50 - CC56 : A - G major pentatonic
	// page 01, maps notes to pentatonic major scale
	// CC60 - CC66 :  A - G minor pentatonic
	
}


void SetAlesisV125MIDIMap(CCMIDIMap *ccmap, CCMIDINoteMap *noteMap, CCMIDIMapable *voice, CCMIDIMapable *filt)
{
	// buttons
	
	// 48 voice select
	ccmap->Add(10, 48, voice); // voice 1
	ccmap->Add(11, 49, voice);
	ccmap->Add(12, 50, voice);
	
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
}
