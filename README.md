# ofxSoundDevicesManager

# OVERVIEW
**openFrameworks** add-on to easily handle selected system sound devices.  
Currently Input devices only.  

**WIP**
- ONLY MS WINDOWS YET. SHOULD ADD macOS / Linux APIs ENGINES.
- USING MS DIRECTSHOW. ASIO OR WASAPI HAS NOT BEEN TESTED BUT SHOULD WORK.
- CURRENTLY USING ONLY INPUT YET. INTENDED TO BE USED ON AN AUDIO ANALYZER.
- DEVICES OUTPUT SELECTOR SEEMS NOT WORKING YET. USES THE DEFAULT DEVICE.
- COMPATIBLE WITH ofxSurfingAudioPlots FOR STYLED WAVEFORM PLOT.

## Screenshots

![](/1-example_Basic/Capture.PNG)  
![](/4-example_Player/Capture.PNG)  
![](/5-example_ExtrasDetector/Capture.PNG)  

IG VIDEO: https://www.instagram.com/reel/Cn22iKeDfV-/?utm_source=ig_web_copy_link  

## Features
- Device selector: 
	- Input and Output (WIP) **devices**.
	- All (Windows yet) sound APIs: **DS/ASIO/WASAPI**. 
- **ImGui** based **GUI**.
- Provide audio basics to any ofApp.
- WIP Smoother module.
- Intuitive VU / RMS.
- Bangs detector using a threshold.
- Gate with release timer.
- New audio widgets.
- Testing scene.

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
* Merge some stuff from https://github.com/roymacdonald/ofxSoundDeviceManager. (?)
* Test and make it macOS-compatible.
* Improve settings using Port names instead of Port Numbers.

### NOTES
* Would crash when hot-changing the sound API **DS/ASIO/WASAPI**, but should wake up and reload fine the settings after restarting the app or by hardcoding devices. 

## AUTHOR
An add-on by **@moebiusSurfing**  
*(ManuMolina). 2020-2022.*

## LICENSE
*MIT License*