import pyaudio
import wave
import audioop
from collections import deque
import math
import time
import librosa
import os

from scipy.stats._multivariate import special_ortho_group_frozen
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

THRESHOLD = 9000  # The threshold intensity that defines silence
                  # and noise signal (an int. lower than THRESHOLD is silence).
LAST_THRESHOLD = 2500


SILENCE_LIMIT = 0.8  # Silence limit in seconds. The max ammount of seconds where
                   # only silence is recorded. When this time passes the
                   # recording finishes and the file is delivered.

LAST_SILENCE_LIMIT = 0.25

PREV_AUDIO = 0.25 # Previous audio (in seconds) to prepend. When noise
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
    # wave = wave[::3]
    mfcc = librosa.feature.mfcc(wave, sr=44100, n_mfcc=20, hop_length= 64)
    #print(mfcc.shape)
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
    # speech = ['Corner', 'Dropball', 'Goalkick', 'Kalibrasi', 'Kickoff', 'Stop', 'Tendang']
    speech = ['a', 'b', 'c', 'd', 'e', 'f', 'g']
    # speaker = ['Arif', 'Azhar', 'Nisar']
    # speaker = ['7', '8', '9']
    speaker = ['Arif', 'Azhar', 'Habib', 'Nisar', 'Revo']

    model = Sequential()
    model = load_model('Model_Hasil_Train/multilabel_new_V7.h5')


    threshold = THRESHOLD
    num_phrases = -1

    #Open stream
    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK)

    record = []
    last_record = []
    cur_data = ''  # current chunk  of audio data
    last_cur_data = []
    rel = RATE/CHUNK
    slid_win = deque(maxlen=int(SILENCE_LIMIT * rel))
    #Prepend audio from 0.5 seconds before noise was detected
    prev_audio = deque(maxlen=int(PREV_AUDIO * rel))
    last_rel = int(SILENCE_LIMIT * rel)
    last_slid_win = deque(maxlen=int(LAST_SILENCE_LIMIT * rel))
    last_started = False
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

    print ("* Listening mic. ")
    while (num_phrases == -1 or n > 0):
        cur_data = stream.read(CHUNK)
        # print(len(cur_data))
        slid_win.append(math.sqrt(abs(audioop.avgpp(cur_data, 4))))
        # print (slid_win[0])
        if(sum([x > THRESHOLD for x in slid_win]) > 0):
            if(not started):
                print ("Starting record of phrase")
                started = True
            record.append(cur_data)
        elif (started is True):
            last_record = record[(len(record)-1)-last_rel:]
            # print(len(last_record))
            # print(len(record))
            for i in range(len(last_record)-1):
                record.pop(len(record)-last_rel+i)
            for i in range(len(last_record)-1):
                last_cur_data = last_record[i]
                last_slid_win.append(math.sqrt(abs(audioop.avg(np.asarray(last_cur_data), 4))))
                if (sum([x > LAST_THRESHOLD for x in last_slid_win]) > 0):
                    record.append(last_cur_data)
            print ("Finished")
            print (cnt)
            cnt += 1
            save_speech(list(prev_audio) + record, p)
            sample = wav2mfcc('data_record/output.wav', max_len=820)
            sample_reshaped = sample.reshape(1, 20, 820, 1)
            prediksi = model.predict(sample_reshaped)
            prediksi_speech  = prediksi[:,np.r_[2:5, 6:8, 10:12]]
            prediksi_speaker = prediksi[:,np.r_[0:2, 5, 8:10]]
            print(prediksi)
            data_speech = speech[np.argmax(prediksi_speech)]
            # data_speech_raw = prediksi_speech[:,np.argmax(prediksi_speech)]
            # if data_speech_raw < 0.8:
            #     data_speech = 'unkonwn'
            print(data_speech)
            # print(data_speech_raw)
            data_speaker = speaker[np.argmax(prediksi_speaker)]
            data_speaker_raw = prediksi_speaker[:,np.argmax(prediksi_speaker)]
            if data_speaker_raw < 0.4:
                data_speaker = 'unkonwn'
            print(data_speaker)
            print(data_speaker_raw)
            # if data_speaker == 'Azhar':
            #     sock.sendto(data_speech.encode(), (UDP_IP, UDP_PORT))
            sock.sendto(data_speech.encode(), (UDP_IP, UDP_PORT))
            # Reset all
            record = []
            last_record = []
            started = False
            slid_win = deque(maxlen=int(SILENCE_LIMIT * rel))
            prev_audio = deque(maxlen=int(PREV_AUDIO * rel))
            last_slid_win = deque(maxlen=int(LAST_SILENCE_LIMIT * rel))
            n -= 1
            print ("Listening ...")
        else:
            prev_audio.append(cur_data)
