#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//w.doReset();

	sampleRate = 48000;
	bufferSize = 512;
	numBuffers = 4;

	audioDevices.setup(sampleRate, bufferSize, numBuffers);

	surfingDetector.setAudioDevicesPtr(&audioDevices);
	//surfingDetector.scale.makeReferenceTo(scale);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	surfingDetector.draw();

	audioDevices.drawGui();
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input)
{
	audioDevices.audioIn(input);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	surfingDetector.keyPressed(key);
}