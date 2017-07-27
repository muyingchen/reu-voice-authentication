from client.SpeechToText import SpeechToText
from os.path import join, dirname, abspath
import pyaudio
import audioop
import wave

class SpeechRecognition:
    def __init__(self):
        self.stt = SpeechToText()
        self.CHUNK = 1024
        self.FORMAT = pyaudio.paInt16
        self.CHANNELS = 1
        self.RATE = 44100
        self.RECORD_SECONDS = 5
        self.AUDIO_OUTPUT_FILENAME = 'output.wav'

    def save_transcript(self, transcript=None):
        # write the recognized text into a txt file
        if transcript != -1:
            with open(join(dirname(abspath(__file__)), 'transcript.txt'), 'w') as text_file:
                text_file.write(transcript + '\n')

    def save_audio(self, frames=None, p=None):
        wf = wave.open(join(dirname(abspath(__file__)), self.AUDIO_OUTPUT_FILENAME), 'wb')
        wf.setnchannels(self.CHANNELS)
        wf.setsampwidth(p.get_sample_size(self.FORMAT))
        wf.setframerate(self.RATE)
        wf.writeframes(b''.join(frames))
        wf.close()

    def recognize(self):
        while True:
            p = pyaudio.PyAudio()
            stream = p.open(format=self.FORMAT,
                            channels=self.CHANNELS,
                            rate=self.RATE,
                            input=True,
                            frames_per_buffer=self.CHUNK)

            print('\n* Please start your command')

            try:
                frames = []
                for i in range(0, int(self.RATE / self.CHUNK * self.RECORD_SECONDS)):
                    try:
                        data = stream.read(self.CHUNK)
                        frames.append(data)
                    except IOError:
                        pass
            # print("* Done listening ")
            except KeyboardInterrupt:
                print('\n' + '* Done recording')
                pass
            # save audio
            self.save_audio(frames, p)
            #open the recorded audio
            audio = open(join(dirname(abspath(__file__)), self.AUDIO_OUTPUT_FILENAME), 'rb')
            #get transcript from audio using SpeechToText
            print('* Processing the audio...')
            transcript = self.stt.get_transcript(audio)
            if transcript != -1:
                # print('You said: ' + transcript)
                # save the transcript
                self.save_transcript(transcript)
                stream.stop_stream()
                stream.close()
                p.terminate()
                break
            else:
                print('Sorry, I did not get that. Please try again')
                stream.stop_stream()
                stream.close()
                p.terminate()
