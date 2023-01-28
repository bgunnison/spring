
# Pine Synthesizer

**A Physical Modeling Synth based on the Daisy pod and Daisy platform**

*1/28/2023 Brian Gunnison*

[Music](https://soundcloud.com/briang-1-1)

[Virtual Robot](https://virtualrobot.net/)

## Overview

An experimental software platform based on the Daisy Pod HW and software libraries:

[Daisy Pod](https://www.electro-smith.com/daisy/pod)

[Daisy Forum](https://forum.electro-smith.com/)

[Daisy DSP Library](https://electro-smith.github.io/DaisySP/index.html)

Trying to boil down the complexity of a sound module to something I can perform with live. 
Imagine a MIDI drum pad with a MIDI foot controller. No screen. 
The Pine module can be (for example) configured to play only a D major pentatonic scale, then a E minor pentatonic scale etc. 

## User Interface
### Midi Control
I'm using a Alesis SamplePad connected to the midi port of a FCB1010 midi footcontroller. The Daisy Pod controls are mapped to modify the sound. 
The Samplepad pad notes are as follows: (top left to right) 40, 41, 48, 50, 52, 55, 57, 60. 40 is down octave and 41 is up octave. 48 - 60 play the chosen pentatonic scale. The scale is set by the FCB1010 switches 1 = AMaj, 2 = BMaj... 5=Emaj, 6 = Amin... 10 = Emin. (Sorry F). The FCB outputs CC #s 1 = 22, 2 = 21... 10 = 13. 
So simply footpress SW1 and you are in Amaj, hit top left to go octave down, top right octave up. 

### Pod controls
The pod buttons select what the selector knob and the pots control. The current selection is indicated by the two RGB LEDs. 

Button 1 (left button)	picks what the selector knob rotates through. The LEDs (RGB) will briefly show what the selection is. 

| Selector | LEDs    |	selects |	
| -------- | ------- | -------- |
| 0		     | temp BR |	voices  |
| 1		     | temp BG |	filters | 

Voices are: **spring, mallet, formant, Noise, Synth**. Filters are: **None, State Variable, Moog**

The temp led shows its color for 2 seconds then reverts to button 2 setting

For example press button 1 till the LEDS show BR for two seconds and the selector knob will rotate through the voice selections. 

Button 2 selects what the pots control as below

| Button 2 |	LEDs |	Pot 1	     |  Pot 2     |
| -------  | ----- | ----------- | ---------- |
| 0			   |  RR	 |	Gain Right | Gain Left  |
| 1			   |  RG	 |	Filter Freq| Filter Res |
| 2 			 |  RB	 |	Voice P1	 | Voice P2   |
| 3			   |  GR	 |	Voice P3	 | Voice P4   |
| 4			   |  GG	 |  Voice P5	 | Voice P6   |


Voice	parameter functions

| Voice |		P1	|   P2	|   P3	|   P4	|   P5	| P6    |
| ----- | ----- |------ | ----- | ----- |------ | ----- |
|**synth**	|	  adsrattack	|adsrdecay	|  adsrsustain	|  adsrrelease| | | |
|**formant**|		adsrattack|	adsrdecay	|  adsrsustain	|  adsrrelease	|  formantfreq	| phaseshift |
|**mallet**	|	damping	|	  structure|	  brightness	 | accent | | |
|**noise**	|	  resonance	 | drive	|	    attack	|	    release | | |
|**spring**	|	damping	|	  structure|	  brightness	|  accent | | |


**Software Features:**
1. Extensible voice selection and control
2. Extensible filter selection and control
3. MIDI mapping includes CC and note mapping. Soon to come uploadable MIDI maps to map your favorite controller. 
4. CPU usage bounded polyphony

## Development

C++ with Visual Studio 2019 and Visual GDB. [STLINK-V3MINI](https://www.st.com/resource/en/user_manual/dm00555046-stlink-v3mods-and-stlink-v3mini-mini-debuggers-programmers-for-stm32-stmicroelectronics.pdf) debugger/programmer.

**Debugging**

The Python file "serial_monitor.py" connects to the USB serial output of the software for log messages. 

**Directories**

spring <- this project adjacent to the below from Daisy

[daisyexamples](https://github.com/electro-smith/DaisyExamples) 

## Contributing
Pull requests are welcome. 

## License
[MIT](https://choosealicense.com/licenses/mit/)

