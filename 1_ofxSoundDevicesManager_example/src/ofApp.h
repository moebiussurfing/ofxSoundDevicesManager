#pragma once

#include "ofMain.h"
#include "ofxSoundDevicesManager.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void drawWaveform();

		void exit() {
			audioDevices.close();
		}

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofxSoundDevicesManager audioDevices;

		int sampleRate;
		int bufferSize;
		int numBuffers;

		void audioIn(ofSoundBuffer& input) override; // not used in this example
		void audioOut(ofSoundBuffer& output) override;

		float waveformInput[4096]; //make this bigger, just in case
		int waveInputIndex;

		float waveformOutput[4096]; //make this bigger, just in case
		int waveOutputIndex;

		void setupLogDebug();


};
