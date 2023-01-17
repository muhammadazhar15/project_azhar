import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy.fftpack import fft,fftfreq

samplerate1, data1 = wavfile.read('data_record/dataz/azza.wav')
samples1 = data1.shape[0]
datafft1 = fft(data1)
fftabs1 = abs(datafft1)
freqs1 = fftfreq(samples1,1/samplerate1)

samplerate2, data2 = wavfile.read('data_record/dataz/melvy.wav')
samples2 = data2.shape[0]
datafft2 = fft(data2)
fftabs2 = abs(datafft2)
freqs2 = fftfreq(samples2,1/samplerate2)

samplerate3, data3 = wavfile.read('data_record/dataz/yani.wav')
samples3 = data3.shape[0]
datafft3 = fft(data3)
fftabs3 = abs(datafft3)
freqs3 = fftfreq(samples3,1/samplerate3)

# samplerate4, data4 = wavfile.read('data_record/database/nisar.wav')
# samples4 = data4.shape[0]
# datafft4 = fft(data4)
# fftabs4 = abs(datafft4)
# freqs4 = fftfreq(samples4,1/samplerate4)
#
# samplerate5, data5 = wavfile.read('data_record/database/revo.wav')
# samples5 = data5.shape[0]
# datafft5 = fft(data5)
# fftabs5 = abs(datafft5)
# freqs5 = fftfreq(samples5,1/samplerate5)

plt.xlim( [10, samplerate1/2] )
plt.xscale( 'log' )
plt.grid( True )
plt.xlabel( 'Frequency (Hz)' )
plt.ylim(0,50000000)
plt.plot(freqs1[:int(freqs1.size/2)],fftabs1[:int(freqs1.size/2)])
plt.plot(freqs2[:int(freqs2.size/2)],fftabs2[:int(freqs2.size/2)])
plt.plot(freqs3[:int(freqs3.size/2)],fftabs3[:int(freqs3.size/2)])
# plt.plot(freqs4[:int(freqs4.size/2)],fftabs4[:int(freqs4.size/2)])
# plt.plot(freqs5[:int(freqs5.size/2)],fftabs5[:int(freqs5.size/2)])
plt.legend(['Azza', 'Melvy','Yani'], loc='upper right')
plt.show()
