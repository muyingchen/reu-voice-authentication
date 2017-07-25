from client.SpeechRecognition import SpeechRecognition
from client.VoiceVerification import VoiceVerification

class AlexaPi:
    def __init__(self):
        self.s = SpeechRecognition()
        self.v = VoiceVerification()

    def run(self):
        t = input("Press 'y' to delete all previous enrollment, 'n' to continue: ")
        if t == 'Y' or t == 'y':
            self.v.delete_all_enrollments()

        while True:
            # step 1: create enrollment first if no enrollment is available
            enrollment_response = self.v.get_enrollments()['Result']
            if not enrollment_response:
                self.v.create_enrollment()

            # step 2: say the command
            self.s.recognize()

            #step 3: verify the command
            authentication = self.v.authenticate()

            # step 4: see if authentication pass or fail
            if authentication['ResponseCode'] == 'SUC':
                print(authentication['Result'])
                repeat = input("\nPress 'y' to continue and 'n' to quit: ")
            else:
                print(authentication['Result'])
                break

            if repeat == 'n' or repeat == 'N':
                break

if __name__ == '__main__':
    alexa_pi = AlexaPi()
    alexa_pi.run()
