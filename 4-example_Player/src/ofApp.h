#pragma once

#include "ofMain.h"

#include "ofxSoundDevicesManager.h"
#include "ofxWindowApp.h"

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

	ofxWindowApp w;
};
