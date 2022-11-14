#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	sampleRate = 48000;
	bufferSize = 512;
	numBuffers = 4;

	audioDevices.setup(sampleRate, bufferSize, numBuffers);

	//wave.setup(0, 0, ofGetWidth(), ofGetHeight());
	////(audioDevices.getSoundStream()).setInput(input);
	////(audioDevices.getSoundStream()).setOutput(output);
	//audioDevices.inStream.setInput(input);
	////audioDevices.inStream.setOutput(output);
	////input.connectTo(wave).connectTo(output);
	//input.connectTo(wave);


	//streamSettings.setApi(ofSoundDevice::Api::MS_DS);

	auto ds = audioDevices.inStream.getDeviceList(ofSoundDevice::Api::MS_DS);
	ofLog() << ofToString(ds);
	ofSoundStreamSettings streamSettings;
	streamSettings.setInDevice(ds[6]);
	//streamSettings.setOutDevice(ds[0]);//tv out

	wave.setup(0, 0, ofGetWidth(), ofGetHeight());
	//(audioDevices.getSoundStream()).setInput(input);
	//(audioDevices.getSoundStream()).setOutput(output);
	audioDevices.inStream.setInput(input);
	//audioDevices.inStream.setOutput(output);
	//input.connectTo(wave).connectTo(output);
	input.connectTo(wave);
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(128);

	audioDevices.drawGui();
	wave.draw();
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input) {
	audioDevices.audioIn(input);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'g') audioDevices.setVisibleToggle();

	if (key == ' ') {

	}
}