# ofxSoundDevicesManager

# Overview
**ofxSoundDevicesManager** is an **openFrameworks** addon to easy handle selected system sound devices.  
**[WORK IN PROGRESS / ALPHA STATE]**

## Screenshot
**exampleBasic**  
![image](/readme_images/Capture1.PNG?raw=true "image")  

## Features
- Select input and output devices by GUI.
- Auto store/Recall settings on startup/exit.
- Plot preview waveforms.
- Enable bypass channels.
- Display list devices. 
- Windows only!

## Usage
 
### ofApp.h
```.cpp
#include "ofxSoundDevicesManager.h"
ofxSoundDevicesManager audioDevices;
```

### ofApp.cpp
```.cpp
ofApp::setup(){
	audioDevices.setup(44100, 512);//set samplerate and buffer size
}

ofApp::update(){
	audioDevices.update();
}

ofApp::draw(){
	audioDevices.draw();//show gui
}

void ofApp::exit() {
	audioDevices.close();//to close devices. auto store session settings on class destructor
}

void ofApp::keyPressed(int key) {
	if (key == 'g') audioDevices.toggleVisibleGui();
	if (key == 'l') audioDevices.toggleVisibleLog();
	if (key == 'a') audioDevices.toggleActive();
}

void ofApp::audioIn(ofSoundBuffer& input) {//to handle by your self
	audioDevices.audioIn(input);
}

void ofApp::audioOut(ofSoundBuffer& output) {//to handle by your self
	audioDevices.audioOut(output);
}
```

## Dependencies
- Already included into **/libs**. You don't need to add them manually to your project.  
- https://github.com/Feliszt/ofxSimpleFloatingGui from **@Feliszt**. Thanks!  
- https://github.com/tettou771/ofxTextFlow from **@tettou771**, Thanks!  

## Tested systems
- **Windows10** / **VS2017** / **OF 0.11**
- ONLY WORKS ON WINDOWS.

### TODO/IDEAS
* Make it https://github.com/roymacdonald/ofxSoundObjects compatible.  
* Make macOS compatible.  

### Notes
* *ALPHA STATE / WORK IN PROGRESS*  
* Would crash sometimes, like when changing **DS/ASIO/WASAPI** but should wake up and reload settings after restarting the app. 

## Author
An addon by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## License
*MIT License*