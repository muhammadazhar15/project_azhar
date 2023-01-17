import pyaudio
import wave
import audioop
from collections import deque
import math
import time

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

def audio_int(num_samples=50):
    print("Getting intensity values from mic.")
    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK)

    values = [math.sqrt(abs(audioop.avg(stream.read(CHUNK), 4))) for x in range(num_samples)]
    values = sorted(values, reverse=True)
    r = sum(values[:int(num_samples * 0.2)]) / int(num_samples * 0.2)
    print(" Finished ")
    print(" Average audio intensity is ", r)
    stream.close()
    p.terminate()
    return r

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
    cur_data = ''  # current chunk  of audio data
    rel = RATE/CHUNK
    slid_win = deque(maxlen=int(SILENCE_LIMIT * rel))
    #Prepend audio from 0.5 seconds before noise was detected
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

    print ("* Listening mic. ")
    while (num_phrases == -1 or n > 0):
        cur_data = stream.read(CHUNK)
        slid_win.append(math.sqrt(abs(audioop.avg(cur_data, 4))))
        # print (slid_win)
        if(sum([x > THRESHOLD for x in slid_win]) > 0):
            if(not started):
                print ("Starting record of phrase")
                started = True
            record.append(cur_data)
        elif (started is True):
            print ("Finished")
            print (cnt)
            cnt += 1
            save_speech(list(prev_audio) + record, p)
            # Reset all
            record = []
            started = False
            slid_win = deque(maxlen=int(SILENCE_LIMIT * rel))
            prev_audio = deque(maxlen=int(PREV_AUDIO * rel))
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