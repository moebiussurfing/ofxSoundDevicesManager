# ofxSoundDevicesManager

# Overview
**openFrameworks** addon to easily handle selected system sound devices.  

**WORK IN PROGRESS**
- ONLY MS WINDOWS YET. SHOULD ADD macOS APIs.
- USING MS DIRECTSHOW. ASIO OR WASAPI NO TESTED BUT COULD WORK.
- 

## Screenshot

![](/example/Capture.PNG)  

## Features
- **Select** Input and Output **devices** using a **GUI**.
- Auto **Store/Recall settings** on startup/exit.
- Plot preview **waveforms** and **VU meters**.
- **Enable/Bypass** channels & **gain sliders**.
- Display **list devices** from all sound APIs: **DS/ASIO/WASAPI**. 
- **Windows only** for the moment!

## Usage
 
### ofApp.h
```.cpp
#include "ofxSoundDevicesManager.h"

ofxSoundDevicesManager audioDevices;
```

### ofApp.cpp
```.cpp
void ofApp::setup(){
	audioDevices.setup(44100, 512);//set samplerate and buffer size
}

void ofApp::draw(){
	audioDevices.drawGui();
}

void ofApp::audioIn(ofSoundBuffer& input) {//to handle by your self. look other examples
	audioDevices.audioIn(input);
}

void ofApp::audioOut(ofSoundBuffer& output) {//to handle by your self. look other examples
	audioDevices.audioOut(output);
}
```

### Dependencies
- ofxGui
- ofxSurfingImGui
- ofxImGui / Fork
- ofxSurfingBox
- ofxSurfingHelpers

### Tested systems
- **Windows10** / **VS2017** / **OF 0.11+**
  ONLY WORKS ON WINDOWS YET!

### TODO/IDEAS
* Make it https://github.com/roymacdonald/ofxSoundObjects compatible.  
* Make it macOS compatible. 

### Notes
* Would crash when hot-changing the sound API **DS/ASIO/WASAPI**, but should wake up and reload fine the settings after restarting the app or by hardcoding devices. 

## Author
An addon by **@moebiusSurfing**  
*(ManuMolina). 2020-2022.*

## License
*MIT License*  