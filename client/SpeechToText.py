from watson_developer_cloud import SpeechToTextV1, WatsonException
from os.path import join, dirname, abspath
import config
import json

class SpeechToText:
    def __init__(self):
        self.config = config.speech_to_text_config ;
        if not self.config['username']:
            raise ValueError('No IBM Watson credential username found.')
        elif not self.config['password']:
            raise ValueError('No IBM Watson credential password found.')
        else:
            self.speech_to_text = SpeechToTextV1(
            username = self.config['username'],
            password = self.config['password'],
            x_watson_learning_opt_out=False)

    def get_text(self, audio_file=None):
        # get a text result for recognized speech
        if not audio_file:
            audio_file = open(join(dirname(abspath(__file__)), 'data/wave/test4.wav'), 'rb')
        try:
            # print(json.dumps(self.speech_to_text.models(), indent=2))
            # print(json.dumps(self.speech_to_text.get_model('en-US_BroadbandModel'), indent=2))

            # get response in json string format
            response = json.dumps(self.speech_to_text.recognize(
                      audio_file, content_type='audio/wav', timestamps=True,
                      word_confidence=True))
            # extract the correct transcript from response json string
            text = self.get_transcript(json.loads(response))
            # save the transcript to a text file
            self.write_text(text)
            return text
        except WatsonException as err:
            print(err)
            pass

    def get_transcript(self, text):
        # retrieve transcript from the json object represented text
        return text['results'][0]['alternatives'][0]['transcript']

    def get_language_model(self):
        # return a list of language models that Watson speech-to-text engine support
        m = self.speech_to_text.models()
        models = []
        for i in m['models']:
            models.append(i['name'])
        return models

    def write_text(self, text):
        with open('output.txt', 'w') as text_file:
            text_file.write(text + '\n')
#
# if __name__ == '__main__':
#     s = SpeechToText()
#     audio = open('data/wave/test10.wav', 'rb')
#     print(s.get_text())
