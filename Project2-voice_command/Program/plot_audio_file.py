# import the pyplot and wavfile modules

import matplotlib.pyplot as plot
from scipy.io import wavfile

# Read the wav file (mono)
samplingFrequency, signalData = wavfile.read('data_record/2.wav')

# Plot the signal read from wav file
plot.subplot(211)
plot.title('Spectrogram of a wav file with piano music')
plot.plot(signalData)
plot.xlabel('Sample')
plot.ylabel('Amplitude')

plot.subplot(212)
plot.specgram(signalData, Fs=samplingFrequency)
plot.xlabel('Time')
plot.ylabel('Frequency')

plot.subplots_adjust(hspace=0.5)

plot.show()