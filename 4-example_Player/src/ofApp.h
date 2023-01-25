#pragma once

#include "ofMain.h"

#include "ofxSoundDevicesManager.h"
#include "ofxWindowApp.h"
#include "CircleBeat.h"

class ofApp : public ofBaseApp
{
public:

	void setup();
	void exit();
	void draw();
	void keyPressed(int key);

	ofxSoundDevicesManager audioDevices;
	void audioIn(ofSoundBuffer& input) override;
	int sampleRate;
	int bufferSize;
	int numBuffers;

	ofParameter<bool> bScene{ "Scene", false };
	ofParameter<bool> bFlipScene{ "Flip", true };
	ofParameter<bool> bShape{ "Shape", true };
	ofParameterGroup g{ "ofApp", bFlipScene, bShape, bScene };
	//CircleBeat cBeat;

	ofxWindowApp w;
};
