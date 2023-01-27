# ofxSoundDevicesManager

# OVERVIEW
**openFrameworks** add-on to easily handle selected system sound devices.  
Currently Input devices only.  

**WORK IN PROGRESS**
- ONLY MS WINDOWS YET. SHOULD ADD macOS / Linux APIs ENGINES.
- USING MS DIRECTSHOW. ASIO OR WASAPI HAS NOT BEEN TESTED BUT SHOULD WORK.
- CURRENTLY USING ONLY INPUT YET. INTENDED TO BE USED ON AN AUDIO ANALYZER.
- COMPATIBLE WITH ofxSurfingAudioPlots FOR STYLED WAVEFORM PLOT.

## Screenshot

![](/1-example_Basic/Capture.PNG)  
![](/4-example_ExtrasDetector/Capture.PNG)  

## Features
- **Select** Input and Output (WIP) **devices** using an **ImGui** based **GUI**.
- Auto **Store/Recall settings** on startup/exit.
- **Enable/Bypass** & available **Gain Sliders** (WIP).
- Display **list devices** from all sound APIs: **DS/ASIO/WASAPI**. 

## Usage
 
### ofApp.h
```.cpp
#include "ofxSoundDevicesManager.h"

ofxSoundDevicesManager audioDevices;
```

### ofApp.cpp
```.cpp
void ofApp::setup(){
	audioDevices.setup(44100, 512); // Set samplerate and buffer size
}

void ofApp::draw(){
	audioDevices.drawGui();
}

void ofApp::audioIn(ofSoundBuffer& input) {
	audioDevices.audioIn(input);
}
```

### Dependencies
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)
* [ofxSurfingBox](https://github.com/moebiussurfing/ofxSurfingBox)
* [ofxSurfingImGui](https://github.com/moebiussurfing/ofxSurfingImGui)
* [ofxImGui](https://github.com/Daandelange/ofxImGui/) / Fork
* ofxGui / Optional
* [ofxSurfingAudioPlots](https://github.com/moebiussurfing/ofxSurfingAudioPlots/) / Optional for example 2

### Tested Systems
- **Windows 10** / **VS 2022** / **OF 0.11+**

### TODO / IDEAS
* Make it https://github.com/roymacdonald/ofxSoundObjects compatible. (?)
* Merge stuff from https://github.com/roymacdonald/ofxSoundDeviceManager. (?)
* Test / Make it macOS compatible.
* Improve settings using Port Name instead of Port Numbers.

### NOTES
* Would crash when hot-changing the sound API **DS/ASIO/WASAPI**, but should wake up and reload fine the settings after restarting the app or by hardcoding devices. 

## AUTHOR
An add-on by **@moebiusSurfing**  
*(ManuMolina). 2020-2022.*

## LICENSE
*MIT License*