#include "ofApp.h"

//#define USE_ofBaseApp_Pointer

//--------------------------------------------------------------
void ofApp::setup() 
{
	windowApp.setFrameRate(60);

	setupSoundDevicesManager();
}

//--------------------------------------------------------------
void ofApp::setupSoundDevicesManager() {

	sampleRate = 44100;
	bufferSize = 512;
	numBuffers = 4;

	audioDevices.setup(sampleRate, bufferSize, numBuffers);
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofClear(ofColor::orange);

	audioDevices.drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) 
{
	if (key == 'g')
	{
		audioDevices.setVisibleToggle();
	}
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input) {
	audioDevices.audioIn(input);
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer& output) {
	audioDevices.audioOut(output);
}
