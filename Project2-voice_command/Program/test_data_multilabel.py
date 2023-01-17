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
MESSAGE = '1234567890'
sock = socket.socket(socket.AF_INET,  # Internet
        socket.SOCK_DGRAM)  # UDP

def wav2mfcc(file_path, max_len=11):
    wave, sr = librosa.load(file_path, mono=True, sr=None)
    wave = wave[::3]
    mfcc = librosa.feature.mfcc(wave, sr=16000)

    # If maximum length exceeds mfcc lengths then pad the remaining ones
    if (max_len > mfcc.shape[1]):
        pad_width = max_len - mfcc.shape[1]
        mfcc = np.pad(mfcc, pad_width=((0, 0), (0, pad_width)), mode='constant')

    # Else cutoff the remaining parts
    else:
        mfcc = mfcc[:, :max_len]

    return mfcc

speech = ['1','2','3']
voice  = ['Azhar','Danier','Habib']

sample = wav2mfcc('data_new/1_0.wav')
sample_reshaped = sample.reshape(1, 20, 11, 1)
# print(get_labels()[0][np.argmax(model.predict(sample_reshaped))])

model = Sequential()
model = load_model('model_multilabel.h5')

prediksi = model.predict(sample_reshaped)
# print(prediksi)

prediksi_speech = prediksi[0][0:3]
prediksi_voice  = prediksi[0][3:6]

index = np.argmax(prediksi_speech)
print(speech[index])
sock.sendto(speech[index].encode(), (UDP_IP, UDP_PORT))
index = np.argmax(prediksi_voice)
print(voice[index])
sock.sendto(voice[index].encode(), (UDP_IP, UDP_PORT))