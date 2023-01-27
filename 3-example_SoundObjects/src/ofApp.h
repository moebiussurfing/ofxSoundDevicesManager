#pragma once

/*

TODO: WIP:

*/


#include "ofMain.h"

//--

#include "ofxSoundDevicesManager.h"
#include "ofxWindowApp.h"

#include "ofxSoundObjects.h"
#include "waveformDraw.h"

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

		waveformDraw wave;
		ofxSoundInput input;
		//ofxSoundOutput output;
		
		ofxWindowApp w;
};
