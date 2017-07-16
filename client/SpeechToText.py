from watson_developer_cloud import SpeechToTextV1
import config
import request

class SpeechToText:
    def __init__(self):
        self.config = config.config;
        if not self.config['username']:
            raise ValueError('No IBM Watson credential username found.')
        else if not self.config['password']:
            raise ValueError('No IBM Watson credential password found.')
        else:
            self.speech_to_text = SpeechToTextV1(
            username = self.config['username'],
            password = self.config['password'],
            x_watson_learning_opt_out=False

    def speech_to_text(self):
        return self.speech_to_text
