import os
import librosa
import numpy as np
import keras
from keras.models import Sequential
from keras.models import load_model

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

speech = ['Corner', 'Dropball', 'Goalkick', 'Kalibrasi', 'Kickoff', 'Stop', 'Tendang']
speaker = ['Arif', 'Azhar', 'Habib', 'Nisar', 'Revo']

model = Sequential()
model = load_model('Model_Hasil_Train/multilabel_new_V7.h5')

path = 'data_record/BARU/'
nama = os.listdir(path)
i = 0
j = 0
hasil_perintah = [0,0,0,0,0]
hasil_orang = [0,0,0,0,0]

for namas in nama:
    jaraks = [path + namas + '/' + jarak for jarak in os.listdir(path + '/' + namas)]
    for jarak in jaraks:
        rekamans = [jarak + '/' + rekaman for rekaman in os.listdir(jarak)]
        for rekaman in rekamans:
            files = [rekaman + '/' + file for file in os.listdir(rekaman)]
            for file in files:
                # print(file)
                perintah = speech[i]
                if namas=='Jauhar' or namas=='Muklis' or namas=='Mukramin' or namas=='Azza' or namas=='Yani' or namas=='Melvy':
                    namas = 'unknown'

                sample = wav2mfcc(file, max_len=820)
                sample_reshaped = sample.reshape(1, 20, 820, 1)
                prediksi = model.predict(sample_reshaped)
                prediksi_speech = prediksi[:, np.r_[2:5, 6:8, 10:12]]
                prediksi_speaker = prediksi[:, np.r_[0:2, 5, 8:10]]
                # print(prediksi)
                data_speech = speech[np.argmax(prediksi_speech)]
                data_speech_raw = prediksi_speech[:, np.argmax(prediksi_speech)]
                # if data_speech_raw < 0.8:
                #     data_speech = 'unknown'
                # print(data_speech,data_speech_raw)
                data_speaker = speaker[np.argmax(prediksi_speaker)]
                data_speaker_raw = prediksi_speaker[:, np.argmax(prediksi_speaker)]
                if data_speaker_raw < 0.95:
                    data_speaker = 'unknown'
                # print(data_speaker,data_speaker_raw)

                if data_speech == perintah:
                    hasil_perintah[j] = 1
                else:
                    hasil_perintah[j] = 0
                if data_speaker == namas:
                    hasil_orang[j] = 1
                else:
                    hasil_orang[j] = 0

                if j==4:
                    print(hasil_perintah[0],'\t',hasil_perintah[1],'\t',hasil_perintah[2],'\t',hasil_perintah[3],'\t',hasil_perintah[4],'\t',hasil_orang[0],'\t',hasil_orang[1],'\t',hasil_orang[2],'\t',hasil_orang[3],'\t',hasil_orang[4])
                    if i==6:
                        print(jarak)
                        print('\n')
                j = j+1
                if j>4:
                    i=i+1
                    j=0
                if i>6:
                    i=0


# labels = os.listdir(path)
# for label in labels:
#     wavfiles = [path + label + '/' + wavfile for wavfile in os.listdir(path + '/' + label)]
#     for wavfile in wavfiles:
#         print(wavfile)