import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy.io.wavfile import write
import wave
from statistics import mean
from functools import reduce
import glob

# filename = 'data_record/6.wav'
#
# with wave.open(filename) as w:
#     framerate = w.getframerate()
#     frames = w.getnframes()
#     channels = w.getnchannels()
#     width = w.getsampwidth()
#     print('sampling rate:', framerate, 'Hz')
#     print('length:', frames, 'samples')
#     print('channels:', channels)
#     print('sample width:', width, 'bytes')
i = 1
j = 0
for filename in glob.glob('data_new/*.wav'):
    frequency, amplitude = wavfile.read(filename)
    print(filename)
    n = 0
    interval = 44100
    batas_awal = 5000
    batas_akhir = 391000
    result = 0
    index = 0
    result = 0
    save_index = 0

    print(len(amplitude))
    while n + interval - 1 != len(amplitude):
        x = mean(np.absolute(amplitude[batas_awal:batas_akhir]))
        print(x, n)
        if x > result:
            result = x
            save_index = index
        n += 441
        batas_awal = batas_awal + 441
        batas_akhir = batas_akhir + 441
        index += 1
        if batas_awal > len(amplitude):
            break

    print(result, save_index)

    write('data_new/%s_%s.wav'%(i,j), 44100, amplitude[save_index * 441:save_index * 441 + 44100])
    j += 1
    if j>9:
        i += 1
        j = 0



