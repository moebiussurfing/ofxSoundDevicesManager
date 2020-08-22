# ofxSoundDevicesManager

# Overview
**ofxSoundDevicesManager** is an **openFrameworks** addon to handle system sound devices.

## Screenshot
![image](/readme_images/Capture1.PNG?raw=true "image")

## Features
- Select input and output devices by GUI.
- Store/Recall settings.
- Plot preview wavefroms.
- Enable bypass channels.
- Display list devices. 

## Usage
 
### ofApp.h
```.cpp
#include "ofxSoundDevicesManager.h"
ofxSoundDevicesManager myAddon;
```

### ofApp.cpp
```.cpp
ofApp::setup(){
	ofxSoundDevicesManager.setup();
}

ofApp::update(){
	ofxSoundDevicesManager.update();
}

ofApp::draw(){
	ofxSoundDevicesManager.draw();
	ofxSoundDevicesManager.drawGui();
}
```

## Dependencies
- 

## Tested systems
- **Windows10** / **VS2017** / **OF 0.11**

### TODO/IDEAS
* 

### Notes
*

## Author
Addon by **@moebiusSurfing**  
*(ManuMolina). 2020.*

## License