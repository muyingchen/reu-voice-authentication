import pyaudio
import wave
import sys
import os
import config
from watson_developer_cloud import SpeechToTextV1

class SpeechRecognition:
    def __init__(self):
