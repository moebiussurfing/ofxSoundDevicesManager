#pragma once

#include "ofMain.h"

#define USE_ofxWindowApp	

//--

#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

#include "ofxSoundDevicesManager.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void draw();
		void keyPressed(int key);
		
		//-

		ofxSoundDevicesManager audioDevices;

		int sampleRate;
		int bufferSize;
		int numBuffers;

		void setupSoundDevicesManager();

		void audioIn(ofSoundBuffer& input) override;
		void audioOut(ofSoundBuffer& output) override;
		
		//-

#ifdef USE_ofxWindowApp
		ofxWindowApp windowApp;
#endif

};
