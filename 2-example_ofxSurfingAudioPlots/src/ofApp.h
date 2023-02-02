#pragma once

#include "ofMain.h"

/*

	Requires to un comment the line:
	#define USE_WAVEFORM_PLOTS

*/

//--

#include "ofxSoundDevicesManager.h"
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
		
		ofxWindowApp w;
};
