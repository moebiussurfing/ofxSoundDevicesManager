# ofxSoundDevicesManager

# OVERVIEW
An **openFrameworks** add-on to provide audio basics to any ofApp.  
Easily handles and selects system sound devices.  
Currently working on Windows only, and for Input devices only.  
 
<details>
<summary>NOTE</summary>

It will not work _out-of-the-box_ without [ofxSurfingImGui](https://github.com/moebiussurfing/ofxSurfingImGui), or using **ofxGui** instead. But could be fixed for that. Look on **USE_OFXGUI_INTERNAL** directive. 
For similar basic purposes (maybe for devices selector only) you can use [ofxSoundDeviceManager](https://github.com/roymacdonald/ofxSoundDeviceManager) from **@roymacdonald** instead of this add-on.
</details>
 
<details>
<summary>WIP</summary>

- ONLY MS WINDOWS YET. SHOULD ADD macOS AND MAYBE LINUX APIs ENGINES.
- NOW I'AM USING THE MS DIRECTSHOW API. ASIO OR WASAPI HAS NOT BEEN HARDLY TESTED BUT IT SHOULD WORK.
- CURRENTLY ONLY USING INPUTS YET. INTENDED TO BE USED MAINLY ON AN AUDIO ANALYZER.
- DEVICES OUTPUT SELECTOR SEEMS NOT WORKING YET. IT USES THE DEFAULT DEVICE.
- SOUND FILE PLAYER GOES THROUGH THE DEFAULT OUTPUT. BUT IT´S FEEDING THE DETECTOR ENGINE FINE.
- COMPATIBLE WITH **ofxSurfingAudioPlots** FOR STYLED WAVEFORM PLOT.
</details>

<details>
<summary>SCREENSHOTS</summary>

### 1-example_Basic
![](/1-example_Basic/Capture.PNG)  

### 4-example_Player
![](/4-example_Player/Capture.PNG)  

### 5-example_ExtrasDetector
![](/5-example_ExtrasDetector/Capture.PNG)  

[IG VIDEO](https://www.instagram.com/reel/Cn22iKeDfV-/?utm_source=ig_web_copy_link)  
</details>

## FEATURES
- Provides audio basics to any ofApp.
- Device selector: 
	- Input and Output (WIP) **audio devices**.
	- All (MS Windows yet) sound APIs: **DS/ASIO/WASAPI**. 
- **ImGui** based **GUI**.
- Smoother module.
- Intuitive VU / RMS.
- Bangs detector using a threshold.
- Gate with release timer.
- New audio widgets.
- Custom auto pilot **AWENGINE**: Auto callibrate threshold to last peaks history.
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
 
<details>
<summary>Optional</summary>
	
* [ofxSurfingWaveplots](https://github.com/moebiussurfing/ofxSurfingWaveplots) / Optional for the example 2
* ofxGui (oF core) / Optional
</details>

<details>
<summary>TESTED SYSTEMS</summary>
 
- **Windows 10** / **VS 2022** / **oF 0.11+**
</details>

<details>
<summary>TODO</summary>

* Improve settings using Port names instead of Port Numbers.
* Test and make it macOS-compatible.
* Merge some stuff from [ofxSoundDeviceManager](https://github.com/roymacdonald/ofxSoundDeviceManager) from **@roymacdonald**. (?)
* Make it [ofxSoundObjects](https://github.com/roymacdonald/ofxSoundObjects) compatible. (?)
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
