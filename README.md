# CISE REU Project: Adding Voice Authentication on Amazon Echo Using Raspberry Pi
## Introduction
The goal of this project is to implement a [voice authentication(recognition)](https://en.wikipedia.org/wiki/Speaker_recognition) system on a Raspberry Pi in order to add an extra layer of security when using popular voice-controlled devices such as Amazon Echo. Voice-controlled products bring convenience to our lives, but they are also very vulnerable to security attacks. 

In Amazon Echo's case, it has to be initialized by connecting it to a user's account through its mobile app. However, anyone could use voice to command Alexa for this user's sensitive information such as calendar, contacts, etc. Therefore, in order to fix this problem, I am investigating potential solutions by implementing a voice authentication system on Raspberry Pi. Since it is difficult to manipulate the hardware side of Amazon Echo, I will emulate Alexa with a Raspbery Pi using the [Alexa Voice Service](https://developer.amazon.com/alexa-voice-service) that Amazon provides for developers.

## Table of Contents
1. [Speech Recognition](#sr)
2. [Voice Authentication](#va)
3. [Alexa Voice Service](#avs)
4. [Raspberry Pi Setup and Miscellaneous](#rp)

## Speech Recognition <a name="sr"></a>
The purpose of using speech recognition softwares in this project is to transcribe audio files to get the text from the speech. This *could be* useful for the later part of the project when trying to emulate Amazon Echo with a Raspberry Pi.

Speech recognition engine or API used/experimented in this project:
1. CMU Sphinx
2. IBM Watson Speech to Text API

### CMU Sphinx Toolkit 
CMU Sphinx is an open source speech recognition toolkit click developed by Carnegie Mellon University. Click [here](https://github.com/muyingchen/reu-voice-authentication/tree/master/cmusphinx) for detailed information.

### IBM Watson Speech To Text API 
This is the API that the project is **currently using**. See this [README.md](https://github.com/muyingchen/reu-voice-authentication/blob/master/client/README.md) for setup instructions. 

## Voice Authentication <a name="va"></a>
Voice authentication in this project is the process of verifying a user based on the characteristics of a given audio stream. 

Voice Authentication engine or API used/experimented in this project:
1. ALIZE 
2. VoiceIt API

### ALIZE
[ALIZE](http://alize.univ-avignon.fr/) is an opensource platform for speaker recognition developed by LIA (Laboratoire Informatique d’Avignon, France) and done in collaboration with several academic and industrial partners. 

You can view its GitHub [here](https://github.com/ALIZE-Speaker-Recognition). 

Since this project did not end up using ALIZE for voice authentication purpose, please research on how to install and use ALIZE on Google.

### VoiceIt API
This project is currently using [VoiceIt](https://siv.voiceprintportal.com/API) for voice verification verification (authentication). See this [README.md](https://github.com/muyingchen/reu-voice-authentication/blob/master/client/README.md) for setup instructions. 

## Alexa Voice Service <a name="avs"></a>
See my partner Luke's [GitHub repo](https://github.com/LB316/Raspberry-Pi-Alexa) on how to add Alexa Voice Service on Raspberry Pi.

## Raspberry Pi Setup and Miscellaneous <a name="rp"></a>
Some useful links to configure your Raspberry Pi for this project (in case you really need it)
* [General guide for Raspbeery Pi hardware & software setup](https://www.raspberrypi.org/help/)
* Install Raspbian Operating System on Raspberry Pi
  * Option 1: Installing with [NOOBS](https://www.raspberrypi.org/documentation/installation/noobs.md) 
  * Option 2: Installing with [Raspbian](https://www.raspberrypi.org/documentation/installation/installing-images/README.md) image (what I used)
* [Install Python packages](https://www.raspberrypi.org/documentation/linux/software/python.md)
* [Setup USB microphone as default on the system](https://raspberrypi.stackexchange.com/questions/37177/best-way-to-setup-usb-mic-as-system-default-on-raspbian-jessie)
* Python libraries
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
