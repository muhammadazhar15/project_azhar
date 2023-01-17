#The library is introduced
import pyaudio
import wave
import sys

#Data flow block
chunk = 1024

#Open the wav file in read-only mode
f = wave.open(r"output.wav","rb")

p = pyaudio.PyAudio()

#Open the data stream
stream = p.open(format = p.get_format_from_width(f.getsampwidth()),
                channels = f.getnchannels(),
                rate = f.getframerate(),
                output = True)

#Read data
data = f.readframes(chunk)

#Play
while data !="":
    stream.write(data)
    data = f.readframes(chunk)


#Stop data flow
stream.stop_stream()
stream.close()

#Close PyAudio
p.terminate()