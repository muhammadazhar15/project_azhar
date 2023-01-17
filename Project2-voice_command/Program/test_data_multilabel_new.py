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

sample = wav2mfcc('data_test_new/Diam_Azhar.wav', max_len=50)
sample_reshaped = sample.reshape(1, 15, 50, 1)
# print(get_labels()[0][np.argmax(model.predict(sample_reshaped))])

model = Sequential()
model = load_model('Model_Hasil_Train/model_multilabel_new_V2.h5')

prediksi = model.predict(sample_reshaped)
print(prediksi)