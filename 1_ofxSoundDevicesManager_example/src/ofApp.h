#pragma once

#include "ofMain.h"

#define USE_ofxWindowApp
#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif

#define USE_Log//can be commented to avoid ofxTextFlow dependecy 

#include "ofxSoundDevicesManager.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		void exit();
		void keyPressed(int key);
		
		//-

		void setupSoundDevicesManager();
		ofxSoundDevicesManager audioDevices;

		int sampleRate;
		int bufferSize;
		int numBuffers;

		void audioIn(ofSoundBuffer& input) override;
		void audioOut(ofSoundBuffer& output) override;
		
		//-

		////TODO:
		////for when we are not using ofSoundBuffer
		//void audioIn(float * input, int bufferSize, int nChannels) override;

#ifdef USE_Log
		void setupLogDebug();
#endif

#ifdef USE_ofxWindowApp
		ofxWindowApp windowApp;
#endif

};
