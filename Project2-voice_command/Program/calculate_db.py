import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy.io.wavfile import write
import wave
from scipy.fftpack import fft,fftfreq
import math
import os

path = 'data_record/dataz/'
nama = os.listdir(path)

for namas in nama:
    filename = path + namas

    # filename = 'data_record/dataz/arif_5.wav'
    print(filename)
    samplingFrequency, signalData = wavfile.read(filename)
    # print(np.average(abs(signalData)))
    amplitude = np.average(abs(signalData)) / 32767
    db = 20 * math.log(10, amplitude)
    print(db)

    samplerate1, data1 = wavfile.read(filename)
    samples1 = data1.shape[0]
    datafft1 = fft(data1)
    fftabs1 = abs(datafft1)
    freqs1 = fftfreq(samples1, 1 / samplerate1)

    key = np.argmax(fftabs1)
    print(freqs1[key])