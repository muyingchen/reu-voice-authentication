import pyaudio
import wave
import sys
import os
from client import SpeechToText

class SpeechRecognition:
    def __init__(self):
        self.stt = SpeechToText()
        
