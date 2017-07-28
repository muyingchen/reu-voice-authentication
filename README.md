# CISE REU Project: Adding Voice Authentication on Amazon Echo Using Raspberry Pi
## Introduction
The goal of this project is to implement a [voice authentication(recognition)](https://en.wikipedia.org/wiki/Speaker_recognition) system on a Raspberry Pi in order to add an extra layer of security when using popular voice-controlled devices such as Amazon Echo. Voice-controlled products bring convenience to our lives, but they are also very vulnerable to security attacks. 

In Amazon Echo's case, it has to be initialized by connecting it to a user's account through its mobile app. However, anyone could use voice to command Alexa for this user's sensitive information such as calendar, contacts, etc. Therefore, in order to fix this problem, I am investigating potential solutions by implementing a voice authentication system on Raspberry Pi. Since it is difficult to manipulate the hardware side of Amazon Echo, I will emulate Alexa with a Raspbery Pi using the [Alexa Voice Service](https://developer.amazon.com/alexa-voice-service) that Amazon provides for developers.

## Table of Contents
1. Speech Recognition
2. Voice Authentication
3. Alexa Voice Service
4. Raspberry Pi Miscellaneous

## Speech Recognition
The purpose of using speech recognition softwares in this project is to transcribe audio files to get

## Voice Authentication

## Alexa Voice Service

## Raspberry Pi Setup/Miscellaneuous
Some useful links to configure your Raspberry Pi for this project (in case you really it)
* [Install Python packages](https://www.raspberrypi.org/documentation/linux/software/python.md)
* [Setup USB microphone as default on the system](https://raspberrypi.stackexchange.com/questions/37177/best-way-to-setup-usb-mic-as-system-default-on-raspbian-jessie)
* Python packages
  * [Requests](http://docs.python-requests.org/en/master/user/install/#install)
   ```
   pip3 install requests
   ```
  * [PyAudio](https://people.csail.mit.edu/hubert/pyaudio/)
   ```
   sudo apt-get install python-pyaudio python3-pyaudio
   ```
   and 
   ```
   pip3 install pyaudio
   ```
