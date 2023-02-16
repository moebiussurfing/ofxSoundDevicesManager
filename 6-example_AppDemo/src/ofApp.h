#pragma once


/*

	NOTE: 
	To be used out-of-the-box,
	You should un comment in ofxSoundDevicesManager.h
	#define USE_WAVEFORM_PLOTS

*/


#include "ofMain.h"

#include "ofxSoundDevicesManager.h"
#include "surfingDetector.hpp"

#include "ofxWindowApp.h"

class ofApp : public ofBaseApp
{
public:

	void setup();
	void draw();
	void keyPressed(int key);

	ofxSoundDevicesManager audioDevices;
	void audioIn(ofSoundBuffer& input) override;
	int sampleRate;
	int bufferSize;
	int numBuffers;

	SurfingDetector surfingDetector;
	//ofParameter<float> scale{ "Scale", 0.5, 0, 1 };

	ofxWindowApp w;
};
