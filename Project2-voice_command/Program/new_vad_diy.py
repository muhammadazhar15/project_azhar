import pyaudio
import wave
import audioop
from collections import deque
import math
import time
import numpy as np

CHUNK = 1024 # CHUNKS of bytes to read each time from mic
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100

THRESHOLD = 10000  # The threshold intensity that defines silence
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



def listen_for_speech(threshold=THRESHOLD, num_phrases=-1):
    """
    Listens to Microphone, extracts phrases from it and sends it to
    Google's TTS service and returns response. a "phrase" is sound
    surrounded by silence (according to threshold). num_phrases controls
    how many phrases to process before finishing the listening process
    (-1 for infinite).
    """

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
        # print (slid_win)
        if(sum([x > THRESHOLD for x in slid_win]) > 0):
            if(not started):
                print ("Starting record of phrase")
                started = True
            record.append(cur_data)
        elif (started is True):
            last_record = record[(len(record)-1)-last_rel:]
            # print(len(last_record))
            print(len(record))
            for i in range(len(last_record)-1):
                record.pop(len(record)-last_rel+i)
            for i in range(len(last_record)-1):
                last_cur_data = last_record[i]
                last_slid_win.append(math.sqrt(abs(audioop.avg(np.asarray(last_cur_data), 4))))
                if (sum([x > LAST_THRESHOLD for x in last_slid_win]) > 0):
                    record.append(last_cur_data)
            print(len(record))
            print ("Finished")
            print (cnt)
            cnt += 1
            save_speech(list(prev_audio) + record, p)
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

    print ("* Done recording")
    stream.close()
    p.terminate()

    return response

def save_speech(data, p):
    """ Saves mic data to temporary WAV file. Returns filename of saved
        file """

    filename = 'data_record/output_'+str(int(time.time()))
    # writes data to WAV file
    data = b''.join(data)
    wf = wave.open(filename + '.wav', 'wb')
    wf.setnchannels(1)
    wf.setsampwidth(p.get_sample_size(pyaudio.paInt16))
    wf.setframerate(44100)  # TODO make this value a function parameter?
    wf.writeframes(data)
    wf.close()
    return filename + '.wav'

if(__name__ == '__main__'):
    # audio_int()
    listen_for_speech()