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

#ifdef USE_ofxWindowApp
		ofxWindowApp windowApp;
#endif
		
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		//void keyReleased(int key);
		//void mouseMoved(int x, int y );
		//void mouseDragged(int x, int y, int button);
		//void mousePressed(int x, int y, int button);
		//void mouseReleased(int x, int y, int button);
		//void mouseEntered(int x, int y);
		//void mouseExited(int x, int y);
		//void windowResized(int w, int h);
		//void dragEvent(ofDragInfo dragInfo);
		//void gotMessage(ofMessage msg);
		
		ofxSoundDevicesManager audioDevices;

		int sampleRate;
		int bufferSize;
		int numBuffers;

		void audioIn(ofSoundBuffer& input) override;
		void audioOut(ofSoundBuffer& output) override;

#ifdef USE_Log
		void setupLogDebug();
#endif

};
