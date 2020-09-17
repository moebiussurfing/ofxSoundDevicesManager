#pragma once

#include "ofMain.h"

//----
// OPTIONAL
//#define USE_ofxWindowApp	//handle window framerate, vertical sync, position and size
#define USE_ofxTextFlow//enable log. can be commented to avoid ofxTextFlow dependecy 
//----

#ifdef USE_ofxWindowApp
#include "ofxWindowApp.h"
#endif


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

#ifdef USE_ofxTextFlow
		void setupLogDebug();
#endif

#ifdef USE_ofxWindowApp
		ofxWindowApp windowApp;
#endif

};
