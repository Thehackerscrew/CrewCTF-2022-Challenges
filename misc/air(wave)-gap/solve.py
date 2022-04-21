import librosa
import numpy as np
import librosa.display
from bitstring import BitArray
import matplotlib.pyplot as plt
import subprocess

# Extract audio from video file
print('Extracting audio')
subprocess.call("ffmpeg -i CCTV.mp4 -ab 160k -ac 2 -ar 44100 -vn audio.wav", shell=True)

audio_path = './audio.wav'
print('Loading audio')
x , sr = librosa.load(audio_path, sr=44100)

# Overcomplicating smh volume is probably enough
print('Computing mfccs')
mfccs = librosa.feature.mfcc(x, sr=sr, n_mfcc=2)

# Convert seconds to the relevant time scale
def sec(s):
    return (s/(len(x) // sr)) * mfccs.shape[1]

start = sec(60+20)
span = sec(6)
b_length = sec(10)
bits = ''
i = 0
while True:
    c = mfccs[1][int(start+i*b_length): int(start+i*b_length + span)]

    # 117 is the average average, assuming there are roughly as many zeroes as ones,
    # fiddling a little back and fourth results in no corrupt characters
    bits += '0' if np.average(c) < 116.5 else '1'
    if len(bits) % 8 == 0:
        subflag = BitArray(bin=bits).bytes
        if subflag.endswith(b'}'):
            print(subflag)
            break
    i += 1