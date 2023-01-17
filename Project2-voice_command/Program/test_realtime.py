import pyaudio
import wave
import audioop
from collections import deque
import math
import time
import librosa
import os
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MultiLabelBinarizer
import numpy as np
import keras
from keras.models import Sequential
from keras.models import load_model
from keras.layers import Dense, Dropout, Flatten, Conv2D, MaxPooling2D
from keras.utils import to_categorical
import socket

#UNTUK KIRIM DATA UDP
UDP_IP = "127.0.0.1"
UDP_PORT = 5005
sock = socket.socket(socket.AF_INET,  # Internet
        socket.SOCK_DGRAM)  # UDP

CHUNK = 1024 # CHUNKS of bytes to read each time from mic
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100

THRESHOLD = 4300  # The threshold intensity that defines silence
                  # and noise signal (an int. lower than THRESHOLD is silence).

SILENCE_LIMIT = 2  # Silence limit in seconds. The max ammount of seconds where
                   # only silence is recorded. When this time passes the
                   # recording finishes and the file is delivered.

PREV_AUDIO = 2 # Previous audio (in seconds) to prepend. When noise
                  # is detected, how much of previously recorded audio is
                  # prepended. This helps to prevent chopping the beggining
                  # of the phrase.


def save_speech(data, p):
    """ Saves mic data to temporary WAV file. Returns filename of saved
        file """

    filename = 'data_record/output'
    # writes data to WAV file
    data = b''.join(data)
    wf = wave.open(filename + '.wav', 'wb')
    wf.setnchannels(1)
    wf.setsampwidth(p.get_sample_size(pyaudio.paInt16))
    wf.setframerate(44100)  # TODO make this value a function parameter?
    wf.writeframes(data)
    wf.close()
    return filename + '.wav'

def wav2mfcc(file_path, max_len=11):
    wave, sr = librosa.load(file_path, mono=True, sr=None)
    wave = wave[::3]
    mfcc = librosa.feature.mfcc(wave, sr=44100, n_mfcc=15, hop_length= 1024)
    # print(mfcc.shape)
    # If maximum length exceeds mfcc lengths then pad the remaining ones
    if (max_len > mfcc.shape[1]):
        pad_width = max_len - mfcc.shape[1]
        mfcc = np.pad(mfcc, pad_width=((0, 0), (0, pad_width)), mode='constant')

    # Else cutoff the remaining parts
    else:
        mfcc = mfcc[:, :max_len]

    return mfcc



if(__name__ == '__main__'):
    # speech = ['Diam', 'Kalibrasi', 'Kanan', 'Kiri', 'Maju', 'Mundur','Tendang']
    speech = ['a', 'b', 'c', 'd', 'e', 'f', 'g']
    speaker = ['Arif', 'Azhar', 'Nisar']
    # speaker = ['7', '8', '9']

    model = Sequential()
    model = load_model('Model_Hasil_Train/model_multilabel_new_V2.h5')

    threshold = THRESHOLD
    num_phrases = -1

    # Open stream
    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK)

    record = []
    cur_data = ''  # current chunk  of audio data
    rel = RATE / CHUNK
    slid_win = deque(maxlen=int(SILENCE_LIMIT * rel))
    # Prepend audio from 0.5 seconds before noise was detected
    prev_audio = deque(maxlen=int(PREV_AUDIO * rel))
    started = False
    n = num_phrases
    response = []

    samples = int(5 * RATE)
    count = 0
    while count < samples:
        stream.read(CHUNK)
        count += CHUNK
        time.sleep(0.01)
    cnt = 1

    print("* Listening mic. ")
    while (num_phrases == -1 or n > 0):
        cur_data = stream.read(CHUNK)
        slid_win.append(math.sqrt(abs(audioop.avg(cur_data, 4))))
        # print (slid_win)
        if (sum([x > THRESHOLD for x in slid_win]) > 0):
            if (not started):
                print("Starting record of phrase")
                started = True
            record.append(cur_data)
        elif (started is True):
            print("Finished")
            print(cnt)
            cnt += 1
            save_speech(list(prev_audio) + record, p)
            sample = wav2mfcc('data_record/output.wav', max_len=50)
            sample_reshaped = sample.reshape(1, 15, 50, 1)
            prediksi = model.predict(sample_reshaped)
            prediksi_speech  = prediksi[:,np.r_[2:8, 9]]
            prediksi_speaker = prediksi[:,np.r_[0:2, 8]]
            print(prediksi)
            index = np.argmax(prediksi_speech)
            sock.sendto(speech[index].encode(), (UDP_IP, UDP_PORT))
            print(speech[index])
            index = np.argmax(prediksi_speaker)
            # sock.sendto(speaker[index].encode(), (UDP_IP, UDP_PORT))
            print(speaker[index])
            # Reset all
            record = []
            started = False
            slid_win = deque(maxlen=int(SILENCE_LIMIT * rel))
            prev_audio = deque(maxlen=int(PREV_AUDIO * rel))
            n -= 1
            print("Listening ...")
        else:
            prev_audio.append(cur_data)

