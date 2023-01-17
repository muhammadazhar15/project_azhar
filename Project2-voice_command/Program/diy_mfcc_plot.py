import numpy as np
from scipy.io import wavfile
from scipy.io.wavfile import write
import wave
import librosa
import librosa.display as ld
import matplotlib.pyplot as plt

filename = 'data_record/dataz/arif_5/corner.wav'

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
plot_rate = 1/framerate
interval = frames/(time/plot_rate)
samplingFrequency, signalData = wavfile.read(filename)

x = np.arange(0.0, time, plot_rate)
y = signalData[0:int(frames):int(interval)]
if len(y)>len(x):
    y = y[:len(x)]

wave, sr = librosa.load(filename, mono=True, sr=None)
mfcc = librosa.feature.mfcc(wave, sr=44100, n_mfcc=20, hop_length= 64)


plt.figure(1)

plt.subplot(121)
plt.title('Spectrogram Amplitude')
plt.plot(x,y)
plt.axis([ 0, int(time)+1, -28000, 28000])
plt.xlabel('Sample')
plt.ylabel('Amplitude')

plt.subplot(122)
plt.title('MFCC')
ld.specshow(mfcc, sr=sr, x_axis='time', y_axis='frames', hop_length=64)

plt.show()
