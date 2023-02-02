# ofxSoundDevicesManager

# OVERVIEW
**openFrameworks** add-on to provide audio basics to any ofApp. Easily handles selected system sound devices.  
Currently Windows only and Input devices only. Note that can´t be used without ofxSurfingImGui, or using ofxGui instead!
 
<details>
<summary>WIP</summary>

- ONLY MS WINDOWS YET. SHOULD ADD macOS / (MAYBE) Linux APIs ENGINES.
- NOW I'AM USING THE MS DIRECTSHOW API. ASIO OR WASAPI HAS NOT BEEN TESTED BUT SHOULD WORK.
- CURRENTLY USING ONLY INPUT YET. INTENDED TO BE USED MAINLY ON AN AUDIO ANALYZER.
- DEVICES OUTPUT SELECTOR SEEMS NOT WORKING YET. USES THE DEFAULT DEVICE.
- SOUND FILE PLAYER GOES THROUGH THE DEFAULT OUTPUT. BUT IT´S FEEDING THE DETECTOR ENGINE FINE.
- COMPATIBLE WITH ofxSurfingAudioPlots FOR STYLED WAVEFORM PLOT.
</details>

## SCREENSHOTS

### 1-example_Basic
![](/1-example_Basic/Capture.PNG)  

### 4-example_Player
![](/4-example_Player/Capture.PNG)  

### 5-example_ExtrasDetector
![](/5-example_ExtrasDetector/Capture.PNG)  

[IG VIDEO](https://www.instagram.com/reel/Cn22iKeDfV-/?utm_source=ig_web_copy_link)  

## FEATURES
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

<details>
<summary>USAGE</summary>
 
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
</details>

### DEPENDENCIES
* [ofxSurfingImGui](https://github.com/moebiussurfing/ofxSurfingImGui)
* [ofxImGui](https://github.com/Daandelange/ofxImGui/) / Fork
* [ofxSurfingHelpers](https://github.com/moebiussurfing/ofxSurfingHelpers)
* [ofxSurfingBox](https://github.com/moebiussurfing/ofxSurfingBox)
* ofxGui (oF core) / Optional
* [ofxSurfingAudioPlots](https://github.com/moebiussurfing/ofxSurfingAudioPlots/) / Optional for example 2

### TESTED SYSTEMS
- **Windows 10** / **VS 2022** / **oF 0.11+**

<details>
<summary>TODO</summary>

* Make it https://github.com/roymacdonald/ofxSoundObjects compatible. (?)
* Merge some stuff from https://github.com/roymacdonald/ofxSoundDeviceManager. (?)
* Test and make it macOS-compatible.
* Improve settings using Port names instead of Port Numbers.
</details>

<details>
<summary>NOTES</summary>

* Would crash when hot-changing the sound API **DS/ASIO/WASAPI**, but should wake up and reload fine the settings after restarting the app or by hardcoding devices. 
</details>

## AUTHOR
An add-on by **@moebiusSurfing**  
*(ManuMolina). 2020-2023.*

## LICENSE
*MIT License*
