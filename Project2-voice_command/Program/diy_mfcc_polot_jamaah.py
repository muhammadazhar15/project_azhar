import numpy as np
from scipy.io import wavfile
from scipy.io.wavfile import write
import wave
import librosa
import librosa.display as ld
import matplotlib.pyplot as plt
import os

data_x = []
data_y = []
data_time = []
data_mfcc = []

path = 'data_record/dataz/'
subpath = os.listdir(path)

for subpaths in subpath:
    subpathss = path + subpaths
    subsubpath = os.listdir(subpathss)
    for subsubpaths in subsubpath:
        filename = path + subpaths + '/' + subsubpaths
        print(filename)
        with wave.open(filename) as w:
            framerate = w.getframerate()
            frames = w.getnframes()
            channels = w.getnchannels()
            width = w.getsampwidth()
            # print('sampling rate:', framerate, 'Hz')
            # print('length:', frames, 'samples')
            # print('channels:', channels)
            # print('sample width:', width, 'bytes')

        time = frames / framerate
        plot_rate = 1 / framerate
        interval = frames / (time / plot_rate)
        samplingFrequency, signalData = wavfile.read(filename)

        x = np.arange(0.0, time, plot_rate)
        y = signalData[0:int(frames):int(interval)]
        if len(y) > len(x):
            y = y[:len(x)]
        data_x.append(x)
        data_y.append(y)
        data_time.append((time))

        waver, sr = librosa.load(filename, mono=True, sr=None)
        mfcc = librosa.feature.mfcc(waver, sr=44100, n_mfcc=20, hop_length=64)
        data_mfcc.append(mfcc)

for i in range(0,35,7):
    plt.figure(1)
    plt.subplots_adjust(hspace=0.4)

    plt.subplot(7, 2, 1)
    plt.title('Spectrogram Amplitude')
    plt.plot(data_x[0 + i], data_y[0 + i])
    plt.axis([0, int(data_time[0 + i]) + 1, -28000, 28000])
    # plt.xlabel('Sample')
    plt.ylabel('Corner')

    plt.subplot(7, 2, 2)
    plt.title('MFCC')
    ld.specshow(data_mfcc[0 + i], sr=sr, x_axis='time', y_axis='frames', hop_length=64)

    plt.subplot(7, 2, 3)
    # plt.title('Spectrogram Amplitude')
    plt.plot(data_x[1 + i], data_y[1 + i])
    plt.axis([0, int(data_time[1 + i]) + 1, -28000, 28000])
    # plt.xlabel('Sample')
    plt.ylabel('Dropball')

    plt.subplot(7, 2, 4)
    # plt.title('MFCC')
    ld.specshow(data_mfcc[1 + i], sr=sr, x_axis='time', y_axis='frames', hop_length=64)

    plt.subplot(7, 2, 5)
    # plt.title('Spectrogram Amplitude')
    plt.plot(data_x[2 + i], data_y[2 + i])
    plt.axis([0, int(data_time[2 + i]) + 1, -28000, 28000])
    # plt.xlabel('Sample')
    plt.ylabel('Goalkick')

    plt.subplot(7, 2, 6)
    # plt.title('MFCC')
    ld.specshow(data_mfcc[2 + i], sr=sr, x_axis='time', y_axis='frames', hop_length=64)

    plt.subplot(7, 2, 7)
    # plt.title('Spectrogram Amplitude')
    plt.plot(data_x[3 + i], data_y[3 + i])
    plt.axis([0, int(data_time[3 + i]) + 1, -28000, 28000])
    # plt.xlabel('Sample')
    plt.ylabel('Kalibrasi')

    plt.subplot(7, 2, 8)
    # plt.title('MFCC')
    ld.specshow(data_mfcc[3 + i], sr=sr, x_axis='time', y_axis='frames', hop_length=64)

    plt.subplot(7, 2, 9)
    # plt.title('Spectrogram Amplitude')
    plt.plot(data_x[4 + i], data_y[4 + i])
    plt.axis([0, int(data_time[4 + i]) + 1, -28000, 28000])
    # plt.xlabel('Sample')
    plt.ylabel('Kickoff')

    plt.subplot(7, 2, 10)
    # plt.title('MFCC')
    ld.specshow(data_mfcc[4 + i], sr=sr, x_axis='time', y_axis='frames', hop_length=64)

    plt.subplot(7, 2, 11)
    # plt.title('Spectrogram Amplitude')
    plt.plot(data_x[5 + i], data_y[5 + i])
    plt.axis([0, int(data_time[5 + i]) + 1, -28000, 28000])
    # plt.xlabel('Sample')
    plt.ylabel('Stop')

    plt.subplot(7, 2, 12)
    # plt.title('MFCC')
    ld.specshow(data_mfcc[5 + i], sr=sr, x_axis='time', y_axis='frames', hop_length=64)

    plt.subplot(7, 2, 13)
    # plt.title('Spectrogram Amplitude')
    plt.plot(data_x[6 + i], data_y[6 + i])
    plt.axis([0, int(data_time[6 + i]) + 1, -28000, 28000])
    # plt.xlabel('Sample')
    plt.ylabel('Tendang')

    plt.subplot(7, 2, 14)
    # plt.title('MFCC')
    ld.specshow(data_mfcc[6 + i], sr=sr, x_axis='time', y_axis='frames', hop_length=64)

    plt.show()
