from client import SpeechRecognition
from os.path import join, dirname, abspath
from client import config
from client.VoiceIt import *
import json
import pyaudio
import wave
import os

class VoiceVerification:
    def __init__(self):
        self.config = config.voiceit_config;
        if not self.config['developerId']:
            raise ValueError('No VoiceIt developer ID found. Please check your config file.')
        else:
            self.myVoiceIt = VoiceIt(self.config['developerId'])
        self.developerUserId = self.config['developerUserId']
        self.password = self.config['password']
        self.phrase = 'Alexa tell Raspberry Pi to turn light on'
        self.AUDIO_OUTPUT_FILE_FILENAME = 'output.wav'
        self.ENROLLMENT_OUTPUT_FILE_FILENAME = 'enrollment.wav'


    def create_user(self, userId=None, pwd=None):
        if not userId:
            userId = input('Enter your desired developer user id: ')
        elif not pwd:
            pwd = input('Enter your desired password: ').encode('utf-8')
        response = myVoiceIt.createUser(userId, pwd)
        return response

    def get_user(self, userId=None, pwd=None):
        if not userId and not pwd:
            response = self.myVoiceIt.getUser(self.developerUserId, self.password)
            return response
        elif not pwd:
            pwd = input('Please enter the developer user password: ')
        elif not userId:
            userId = input('Please enter the developer user id: ').encode('utf-8')
        response = self.myVoiceIt.getUser(userId, pwd)
        return response

    def delete_user(self, userId=None, pwd=None):
        if not userId:
            userId = input('Enter the developer user id you wish to delete: ')
        elif not pwd:
            pwd = input('Enter the developer user id password: ').encode('utf-8')
        response = self.myVoiceIt.deleteUser(userId, pwd)
        return response

    # create enrollment with a wave file for future voice verification comparison purpose
    def create_enrollment(self, userId=None, pwd=None, audio_file_path=None, language='en-US'):
        if not userId or not pwd:
            userId = self.developerUserId
            pwd = self.password

        enrollment_count = 0
        enrollment_remaining = 3
        if not audio_file_path:
            print('****************** Creating Enrollment *******************')
            print('* Please say this phrase 3 times to register your voice')
            print(self.phrase + '\n')

            # create 3 enrollment
            while enrollment_count < 5:
                self.record_enrollemnt()
                audio_file_path = join(dirname(abspath(__file__)), self.ENROLLMENT_OUTPUT_FILE_FILENAME)
                response = json.loads(self.myVoiceIt.createEnrollment(userId, pwd, audio_file_path, language))
                if response['ResponseCode'] == 'SUC':
                    enrollment_count += 1
                    enrollment_remaining -= 1
                    print('Enrollment status: success', '\nEnrollment count: ', enrollment_count,
                        '\nRemaining enrollment', enrollment_remaining)
                else:
                    print('Enrollment status:', response['Result'], '\n Please try again. ')
            print('* Enrollment finished! ')
            os.remove(audio_file_path)
        else:
            response = self.myVoiceIt.createEnrollment(userID, pwd, audio_file_path, language)
            return response

    def delete_enrollment(self, userId=None, pwd=None, enrollmentId=None):
        if not userId and not pwd:
            userId = self.developerUserId
            pwd = self.password
        if not enrollmentId:
            enrollmentId = str(input('Please enter enrollment id: '))
        response = self.myVoiceIt.deleteEnrollment(userId, pwd, enrollmentId)
        return response

    def delete_all_enrollments(self, userId=None, pwd=None, enrollmentId=None):
        if not userId and not pwd:
            userId = self.developerUserId
            pwd = self.password
        enrollments = self.get_enrollments(userId, pwd)['Result']
        for e in enrollments:
            self.delete_enrollment(userId, pwd, str(e))

    def get_enrollments(self, userId=None, pwd=None):
        if not userId and not pwd:
            response = json.loads(self.myVoiceIt.getEnrollments(self.developerUserId, self.password))
            return response
        elif not userId:
            userId = input('Please enter your developer user Id: ')
        elif not pwd:
            pwd = input('Please enter developer user id password: ').encode('utf-8')
        response = json.loads(self.myVoiceIt.getEnrollments(userId, pwd))
        return response

    def authenticate(self, userId=None, pwd=None, audio_file_path=None, language='en-US'):
        if not userId or not pwd:
            userId = self.developerUserId
            pwd = self.password
        if not audio_file_path:
            audio_file_path = join(dirname(abspath(__file__)), self.AUDIO_OUTPUT_FILE_FILENAME)
        response = json.loads(self.myVoiceIt.authentication(userId, pwd, audio_file_path, language))
        return response

    def record_enrollemnt(self):
        CHUNK = 1024
        FORMAT = pyaudio.paInt16
        CHANNELS = 1
        RATE = 16000
        RECORD_SECONDS = 5
        p = pyaudio.PyAudio()

        stream = p.open(format=FORMAT,
                        channels=CHANNELS,
                        rate=RATE,
                        input=True,
                        frames_per_buffer=CHUNK)

        print('* Recording enrollment...')
        frames = []
        for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
            data = stream.read(CHUNK)
            frames.append(data)

        stream.stop_stream()
        stream.close()
        p.terminate()

        wf = wave.open(join(dirname(abspath(__file__)), self.ENROLLMENT_OUTPUT_FILE_FILENAME), 'wb')
        wf.setnchannels(CHANNELS)
        wf.setsampwidth(p.get_sample_size(FORMAT))
        wf.setframerate(RATE)
        wf.writeframes(b''.join(frames))
        wf.close()
