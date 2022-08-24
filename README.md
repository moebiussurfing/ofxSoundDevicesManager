# ofxSoundDevicesManager

# OVERVIEW
**openFrameworks** add-on to easily handle selected system sound devices and plot signal waveform.  

**WORK IN PROGRESS**
- ONLY MS WINDOWS YET. SHOULD ADD macOS / Linux APIs.
- USING MS DIRECTSHOW. ASIO OR WASAPI NO TESTED BUT COULD WORK.
- CURRENTLY USING ONLY INPUT. INTENDED TO BE USED ON AN AUDIO ANALYZER.

## Screenshot

![](/example/Capture.PNG)  

## Features
- **Select** Input and Output **devices** using a **GUI**.
- Auto **Store/Recall settings** on startup/exit.
- **Enable/Bypass** & available **Gain Sliders**.
- Display **list devices** from all sound APIs: **DS/ASIO/WASAPI**. 
- **WIP** Class for waveform plotting using different styles.
- Plot preview **waveforms** and **VU meters**.

## Usage
 
### ofApp.h
```.cpp
#include "ofxSoundDevicesManager.h"

ofxSoundDevicesManager audioDevices;
```

### ofApp.cpp
```.cpp
void ofApp::setup(){
	audioDevices.setup(44100, 512); // set samplerate and buffer size
}

void ofApp::draw(){
	audioDevices.drawGui();
}

void ofApp::audioIn(ofSoundBuffer& input) {//to handle by your self. look other examples
	audioDevices.audioIn(input);
}

/*
void ofApp::audioOut(ofSoundBuffer& output) {//to handle by your self. look other examples
	audioDevices.audioOut(output);
}
*/
```

### DEPENDENCIES
- ofxGui / Optional
- ofxSurfingImGui
- ofxImGui / Fork
- ofxSurfingBox
- ofxSurfingHelpers

### Tested systems
- **Windows10** / **VS2017** / **OF 0.11+**

### TODO/IDEAS
* Make it https://github.com/roymacdonald/ofxSoundObjects compatible.  
* Make it macOS compatible.
* Improve setting using port name instead of port numbers.

### NOTES
* Would crash when hot-changing the sound API **DS/ASIO/WASAPI**, but should wake up and reload fine the settings after restarting the app or by hardcoding devices. 

## AUTHOR
An add-on by **@moebiusSurfing**  
*(ManuMolina). 2020-2022.*

## LICENSE
*MIT License*  
