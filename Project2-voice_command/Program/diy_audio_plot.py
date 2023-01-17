import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy.io.wavfile import write
import wave

filename = 'data_record/3.wav'

with wave.open(filename) as w:
    framerate = w.getframerate()
    frames = w.getnframes()
    channels = w.getnchannels()
    width = w.getsampwidth()
    print('sampling rate:', framerate, 'Hz')
    print('length:', frames, 'samples')
    print('channels:', channels)
    print('sample width:', width, 'bytes')

time = frames/framerate
# time=1
plot_rate = 1/framerate
interval = frames/(time/plot_rate)

samplingFrequency, signalData = wavfile.read(filename)

# plot with various axes scales
plt.figure(1)
x = np.arange(0.0, time, plot_rate)
y = signalData[0:int(frames):int(interval)]
if len(y)>len(x):
    y = y[:len(x)]

# linear
# plt.subplot(211)
plt.title('Spectrogram Amplitude')
plt.plot(x,y)
plt.axis([ 0, int(time)+1, -28000, 28000])
plt.xlabel('Sample')
plt.ylabel('Amplitude')


# log
# plt.subplot(212)
# plt.title('Spectrogram Frequency')
# plt.specgram(signalData, Fs=samplingFrequency)
# plt.xlabel('Time')
# plt.ylabel('Frequency')
#
# plt.subplots_adjust(hspace=0.5)

# audio_rate = 1/plot_rate
# write('test.wav', int(audio_rate), y)
# print(len(y))

plt.show()
