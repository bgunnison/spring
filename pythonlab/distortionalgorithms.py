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

distortion algorithms
"""
import numpy as np
import soundfile as sf
from scipy.fftpack import fft, ifft
from scipy.optimize import fsolve

def run_examples():
    
    # Load audio file
    data, fs = sf.read('440.wav')

    # Compute FFT
    spectrum = fft(data)

    # Modify spectrum (e.g., by adding harmonics or filtering)
    modified_spectrum = spectrum + 0.1 * np.resize(spectrum[1:], spectrum.shape)

    # Synthesize distorted signal using IFFT
    distorted_data = np.real(ifft(modified_spectrum))

    # Save distorted audio
    sf.write('distorted_audio.wav', distorted_data, fs)




def germ_diode(data, gain):
    """
    Distort audio signal using diode model.
    """ 
    distorted_data = []
    # Apply germanium diode model to data
    # (assume diode is in forward bias)
    for d in data:
        dt = np.tanh(d * gain)
        # Normalize data
       # try:
       #     dt /= np.max(np.abs(dt))
       # except:
       #     d = 0.0

        distorted_data.append(dt/gain)


    return distorted_data



if __name__ == "__main__":
    #run_examples()
    # Load audio file
    data, fs = sf.read('440.wav')

    # Distort audio with degree of 0.5
    distorted_data = germ_diode(data, 1.5)

    # Save distorted audio
    sf.write('germ_audio1p5.wav', distorted_data, fs)


