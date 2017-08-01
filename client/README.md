# Speech Recognition and Voice Verification API Guide
This is a short introduction on how to use speech recognition and voice verification REST API to run this program.

# Speech Recognition API Setup
This project uses IBM Watson Speech To Text API (available for free for first 30 days) to perform its speech recognition process. Watson is IBM's AI platform. See [here](https://www.ibm.com/watson/) to learn more about Watson.

### Documentation 
Click [here](https://www.ibm.com/watson/developercloud/doc/speech-to-text/index.html).

### How To use
Step 1: Install [Watson Developer Cloud Python SDK](https://github.com/watson-developer-cloud/python-sdk/blob/master/README.md) (See "Installation" part)

Step 2: Get a [IBM Bluemix Service Credential](https://github.com/watson-developer-cloud/python-sdk/blob/master/README.md) (See "Getting the Service Credentials" part)

Step 3: Copy and paste the `username` and `password` from step 2 to the corresponding keys in `speech_to_text_config` in `config.py`

# Voice Verification API Setup
This project uses [VoiceIt](voiceit-tech.com) API (available for free for first 30 days) to perform its voice verification (authentication) process. 

### Documentation 
Click [here](https://siv.voiceprintportal.com/apidocs).

### How To use
Step 1: Make sure you have Python [Requests](http://docs.python-requests.org/en/latest/user/install/) library installed and working properly.

Step 2: Sign up for a VoiceIt account [here](https://siv.voiceprintportal.com/signup).

Step 3: Log in to the service [console](https://siv.voiceprintportal.com/console). Copy and paste the `DeveloperID` to the corresponding key in `voiceit_config` in `config.py`

Step 4: Create a user ID under the **User Details** inside the **User Management
** of the console with a password of your choice. Copy and paste them to `developerUserId` and `password` in `voiceit_config` in `config.py`

Step 5: Go to **Voiceprint Phrases** of the console. Add a **phrase** that you want to use to verify your voice and press **Request New** to wait for approval. 

This project uses "Alexa tell Raspberry Pi to turn light on" for verification purpose. (Note: if you are using a different phrase, please copy and paste it to `self.phrase` inside `VoiceVerification.py`. Keep in mind that you and/or other speakers must say this registered phrase to enroll and test the voice.)
