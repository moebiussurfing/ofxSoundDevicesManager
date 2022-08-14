#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup()
{
	sampleRate = 48000;
	bufferSize = 512;
	numBuffers = 4;

	audioDevices.setPath("ofApp/");
	audioDevices.setup(sampleRate, bufferSize, numBuffers);
}

//--------------------------------------------------------------
void ofApp::draw() {

	audioDevices.drawGui();
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input) {
	audioDevices.audioIn(input);
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer& output) {
	audioDevices.audioOut(output);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'g')
	{
		audioDevices.setVisibleToggle();
	}
}