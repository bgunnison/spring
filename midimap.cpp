
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
	noteMap->Add(50, &DMajorPentNoteMap);
	
	ccmap->Add(0, 20, filt); // freq to CC 20
	ccmap->Add(1, 21, filt); // resonance to CC 21
	
	/*		
				
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
*/
}
