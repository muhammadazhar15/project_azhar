import librosa
import os
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MultiLabelBinarizer
import numpy as np
import keras
from keras.models import Sequential
from keras.layers import Dense, Dropout, Flatten, Conv2D, MaxPooling2D
from keras.utils import to_categorical
import matplotlib.pyplot as plt

DATA_PATH = 'data_percobaan/'

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

def get_labels(path=DATA_PATH):

    labels = os.listdir(path)
    return labels


def save_data_to_array(path=DATA_PATH, max_len=11):
    labels = get_labels(path)

    for label in labels:
        # Init mfcc vectors
        mfcc_vectors = []

        wavfiles = [path + label + '/' + wavfile for wavfile in os.listdir(path + '/' + label)]
        #for wavfile in tqdm(wavfiles, "Saving vectors of label - '{}'".format(label)):
        for wavfile in wavfiles:
            mfcc = wav2mfcc(wavfile, max_len=max_len)
            mfcc_vectors.append(mfcc)
        np.save(label + '.npy', mfcc_vectors)

def get_train_test(split_ratio=0.8, random_state=5):
    labels = get_labels(DATA_PATH)
    label_all = []
    for label in labels:
        data_label = label.split("_")
        label_all.append(data_label)
    mlb = MultiLabelBinarizer()
    mlb.fit(label_all)
    label_data = mlb.classes_

    # Getting first arrays
    X = np.load(labels[0] + '.npy')
    y = mlb.transform([labels[0].split("_")])
    for i in range(len(X)-1):
        y = np.vstack((y, mlb.transform([labels[0].split("_")])))

    # Append all of the dataset into one single array, same goes for y
    for label in labels[1:]:
        x = np.load(label + '.npy')
        X = np.vstack((X, x))
        for i in range(len(x)):
            y = np.vstack((y, mlb.transform([label.split("_")])))

    assert X.shape[0] == len(y)

    return train_test_split(X, y, test_size=(1 - split_ratio), random_state=random_state, shuffle=True)

save_data_to_array(path=DATA_PATH, max_len=11)
X_train, X_test, y_train, y_test = get_train_test()

X_train = X_train.reshape(X_train.shape[0], 20, 11, 1)
X_test = X_test.reshape(X_test.shape[0], 20, 11, 1)

model = Sequential()
model.add(Conv2D(32, kernel_size=(2, 2), activation='relu', input_shape=(20, 11, 1)))
model.add(MaxPooling2D(pool_size=(2, 2)))
model.add(Dropout(0.25))
model.add(Flatten())
model.add(Dense(128, activation='relu'))
model.add(Dropout(0.25))
model.add(Dense(6, activation='sigmoid'))
model.compile(loss=keras.losses.categorical_crossentropy,
              optimizer=keras.optimizers.Adadelta(),
              metrics=['accuracy'])

print(model.summary())

history = model.fit(X_train, y_train, batch_size=100, epochs=10000, verbose=1, validation_data=(X_test, y_test))

model.save('model_multilabel.h5')

# list all data in history
print(history.history.keys())
# summarize history for accuracy
plt.plot(history.history['acc'])
plt.plot(history.history['val_acc'])
plt.title('model accuracy')
plt.ylabel('accuracy')
plt.xlabel('epoch')
plt.legend(['train', 'test'], loc='upper left')
plt.show()
# summarize history for loss
plt.plot(history.history['loss'])
plt.plot(history.history['val_loss'])
plt.title('model loss')
plt.ylabel('loss')
plt.xlabel('epoch')
plt.legend(['train', 'test'], loc='upper left')
plt.show()



# label_ku = []
# labels = os.listdir(DATA_PATH)
# for label in labels:
#     data = label.split("_")
#     label_ku.append(data)
#     # for i in data:
#     #     label_ku.append(i)
#
# print(label_ku)
#
# mlb = MultiLabelBinarizer()
# mlb.fit(label_ku)
#
# print(mlb.classes_)
#
# print(mlb.transform([['3','Danier']]))
# print(type(mlb.transform([['3','Luhung']])))