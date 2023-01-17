try:
    import pyaudio
    import numpy as np
    import pylab
    import matplotlib.pyplot as plt
    from scipy.io import wavfile
    import time
    import sys
    import seaborn as sns
    import wave
    import audioop
    from collections import deque
    import math
except:
    print ("Something didn't import")


i=0
f,ax = plt.subplots(2)

# Prepare the Plotting Environment with random starting values
x = np.arange(80000)
y = np.random.randn(80000)

# Plot 0 is for raw audio data
li, = ax[0].plot(x, y)
ax[0].set_xlim(0,80000)
ax[0].set_ylim(-10000,10000)
ax[0].set_title("Raw Audio Signal")
# Plot 1 is for the FFT of the audio
li2, = ax[1].plot(x, y)
ax[1].set_xlim(0,50000)
ax[1].set_ylim(-10000,10000)
ax[1].set_title("Fast Fourier Transform")
# Show the plot, but without blocking updates
plt.pause(0.01)
plt.tight_layout()

FORMAT = pyaudio.paInt16 # We use 16bit format per sample
CHANNELS = 1
RATE = 16000
CHUNK = 1600 # 1024bytes of data red from a buffer
# CHUNK = 44100# 1024bytes of data red from a buffer
RECORD_SECONDS = 0.1
WAVE_OUTPUT_FILENAME = "file.wav"

x_update = x
y_update = y
x_update_fft = x
y_update_fft = y
awal  = 0
akhir = CHUNK
awal_fft  = 0
akhir_fft = 800

audio = pyaudio.PyAudio()

# start Recording
stream = audio.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True)#,
                    #frames_per_buffer=CHUNK)

global keep_going
keep_going = True


def plot_data(in_data):
    # get and convert the data to float
    audio_data = np.fromstring(in_data, np.int16)
    # Fast Fourier Transform, 10*log10(abs) is to scale it to dB
    # and make sure it's not imaginary
    dfft = 10.*np.log10(abs(np.fft.rfft(audio_data)))

    # Force the new data into the plot, but without redrawing axes.
    # If uses plt.draw(), axes are re-drawn every time
    #print (audio_data[0:10])
    # print(len(audio_data))
    #print dfft[0:10]
    #print
    y_update[awal:akhir] = audio_data
    li.set_xdata(x_update)
    li.set_ydata(y_update)

    # y_update_fft[awal_fft:akhir_fft] = dfft[:len(dfft)-1]
    # li2.set_xdata(x_update_fft)
    # li2.set_ydata(y_update_fft)

    # li.set_xdata(np.arange(len(audio_data)))
    # li.set_ydata(audio_data)
    # li2.set_xdata(np.arange(len(dfft))*10.)
    # li2.set_ydata(dfft)

    # Show the updated plot, but without blocking
    plt.pause(0.01)
    if keep_going:
        return True
    else:
        return False

# Open the connection and start streaming the data
stream.start_stream()
# Loop so program doesn't end while the stream callback's
# itself for new data
while keep_going:
    try:
        if akhir > 80000:
            awal  = 0
            akhir = CHUNK
        if akhir_fft > 40000:
            awal_fft  = 0
            akhir_fft = 800
        plot_data(stream.read(CHUNK))
        awal  += CHUNK
        akhir += CHUNK
        awal_fft  += 800
        akhir_fft += 800
    except KeyboardInterrupt:
        keep_going=False
    except:
        pass

# Close up shop (currently not used because KeyboardInterrupt
# is the only way to close)
stream.stop_stream()
stream.close()

audio.terminate()