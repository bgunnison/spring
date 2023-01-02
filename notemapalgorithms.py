"""
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

Note maps algorithms
"""

chromatic =   ['C',	'C#',	'D',	'D#',	'E',	'F',	'F#',	'G',	'G#',	'A',	'A#',	'B']

 #                   0                                                                                       11     
c_chromatic =      ['C',	'C#',   'D',	'D#',	'E',	'F',	'F#',	'G',	'G#',	'A',	'A#',	'B',    ]
c_maj_pent =       ['C',	'C',	'D',	'D',	'E',	'E',	'E',	'G',	'G',    'A',	'A',    'C1']
c_maj_pentshift =  [0,	    -1,	    0,	    -1,	    0,	    -1,	    -2,	    0,	    -1,	    0,      -1,	    +1]
c_min_pent =       ['C',	'C',	'D#',	'D#',	'F',	'F',	'F',	'G',	'G',    'A#',	'A#',    'C1']
c_min_pentshift =  [0,	    -1,	    +1,	    0,	    +1,	    0,	    -1,	    0,	    -1,	    +1,      0,	    +1]

        #             1       2      3                                                                     0  
cs_chromatic =      ['C#',   'D',	'D#',	'E',	'F',	'F#',	'G',	'G#',	'A',	'A#',	'B',   'C'  ]
cs_maj_pent =       ['C#',	'C#',	'D#',	'D#',	'F',	'F',	'F',	'G#',	'G#',   'A#',   'A#',  'A#']
cs_maj_pentshift =  ['0',	'-1',	'0',	'-1',	'0',	'-1',	'-2',	'0',	'-1',	'0',    '-1',	'-2']

#                   2                                                                               0        1
d_chromatic =      ['D',	'D#',	'E',	'F',	'F#',	'G',	'G#',	'A',	'A#',	'B',    'C',	'C#']
d_maj_pent =       ['D',	'D',	'E',	'E',	'F#',	'F#',	'F#',	'A',	'A',    'B',	'B',    'B']
d_maj_pentshift =  ['0',	'-1',	'0',	'-1',	'0',	'-1',	'-2',	'0',	'-1',	'0',    '-1',	'-2']
 

c_maj_pentshift =  [0,	    -1,	    0,	    -1,	    0,	    -1,	    -2,	    0,	    -1,	    0,      -1,	    +1]
root = 2

def run_major_pent():
    
    for mn in range(127):
        i = (mn - root) % 12
        if i < 0:
            i = 0

        o = mn + c_min_pentshift[i]
        if o < 0:
            o = root

        lni = chromatic[mn % 12]
        lno = chromatic[o % 12]
        
        print(f'NoteIn: {mn} {lni}, NoteOut: {o}, {lno}')




def run_maps():
    print('Major pentatonic')
    map = [0,2,4,7,9]
    l = len(map)
    root = 2
    for mn in range(127):
        base = int(mn/len(map))
        i = mn - (base * l) 
        o = map[i] + root + (base * 12)
        n = chromatic[o % 12]
        print(f'NoteIn: {mn}, b: {base}, i: {i}, NoteOut: {o}, {n}')





if __name__ == "__main__":
    run_major_pent()


