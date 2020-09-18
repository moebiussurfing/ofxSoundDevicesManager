# ofxSoundDevicesManager

# Overview
**openFrameworks** addon to easy handle selected system sound devices.  
**[WORK IN PROGRESS / ALPHA STATE]**

## Screenshot
**exampleBasic**  
![image](/readme_images/Capture1.PNG?raw=true "image")  

## Features
- **Select** input and output **devices** using a **nice GUI**.
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
	audioDevices.close();//to close devices. (will auto store session settings on class destructor)
}

void ofApp::keyPressed(int key) {
	if (key == 'g') audioDevices.toggleVisibleGui();
	if (key == 'l') audioDevices.toggleVisibleLog();
	if (key == 'a') audioDevices.toggleActive();
}

void ofApp::audioIn(ofSoundBuffer& input) {//to handle by your self. look other examples
	audioDevices.audioIn(input);
}

void ofApp::audioOut(ofSoundBuffer& output) {//to handle by your self. look other examples
	audioDevices.audioOut(output);
}
```

### Dependencies
- Already included into **/libs**. You don't need to add them manually to your project:  
	https://github.com/Feliszt/ofxSimpleFloatingGui from **@Feliszt**. Thanks!  
	https://github.com/tettou771/ofxTextFlow from **@tettou771**, Thanks!  

### Tested systems
- **Windows10** / **VS2017** / **OF 0.11**
  ONLY WORKS ON WINDOWS YET.

### TODO/IDEAS
* Make it https://github.com/roymacdonald/ofxSoundObjects compatible.  
* Make it macOS compatible. Is not so hard to add. Help/collaboration is wellcome!  

### Notes
* *ALPHA STATE / WORK IN PROGRESS*  
* Would crash when hot-changing the sound API **DS/ASIO/WASAPI**, but should wake up and reload fine the settings after restarting the app. 

## Author
An addon by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## License
*MIT License*  

**_PLEASE FEEL FREE TO ADD MODIFICATIONS, EXAMPLES OR FEATURES AND TO SEND ME PULL REQUESTS OR ISSUES!_**