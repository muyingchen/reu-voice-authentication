from watson_developer_cloud import SpeechToTextV1, WatsonException
from os.path import join, dirname, abspath
import config
import json

# Extract text from speech audio file using watson speech to text API
class SpeechToText:
    def __init__(self):
        # load API key information from config
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

    # get text result for a recognized speech
    def get_text(self, audio_file=None):
        # if no audio file is passed to the function, use default audio
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

    # retrieve transcript from the json object represented text
    def get_transcript(self, text):
        return text['results'][0]['alternatives'][0]['transcript']

    # return a list of language models that Watson speech-to-text engine support
    def get_language_model(self):
        m = self.speech_to_text.models()
        models = []
        for i in m['models']:
            models.append(i['name'])
        return models

    # write the recognized text into a txt file
    def write_text(self, text):
        with open('output.txt', 'w') as text_file:
            text_file.write(text + '\n')
#
# if __name__ == '__main__':
#     s = SpeechToText()
#     audio = open('data/wave/test10.wav', 'rb')
#     print(s.get_text())
