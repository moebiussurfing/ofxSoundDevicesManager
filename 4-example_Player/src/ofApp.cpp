#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	sampleRate = 48000;
	bufferSize = 512;
	numBuffers = 4;

	audioDevices.setup(sampleRate, bufferSize, numBuffers);

	ofxSurfingHelpers::load(g);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	if (!bScene) ofClear(64);
	else 
	{
		// Draws a scene 
		// related to input vu meter

		float v = audioDevices.getVumeterValue();

		// Bg
		if (bFlip) ofClear(v * 255);
		else ofClear((1 - v) * 255);

		// Shape
		ofPushStyle();
		ofSetCircleResolution(200);
		ofRectangle r = ofGetCurrentViewport();
		float h = ofGetHeight() * v;
		r.setHeight(h);
		r.setPosition(0, ofGetHeight() / 2 - h / 2);
		ofFill();
		if (bFlip) ofSetColor(255);
		else ofSetColor(0);
		// circle
		if (bShape) ofDrawCircle(r.getCenter(), r.getHeight() / 2);
		// rectangle
		else ofDrawRectangle(r);
		ofPopStyle();
	}

	//--

	audioDevices.drawGui();

	//TODO: WIP: trick to expose ui..
	//auto ui = audioDevices.getUiPtr();
	//ui->Begin();
	//if (ui->BeginWindow("ofApp")) {
	//	ui->Add(bFlip, OFX_IM_TOGGLE);
	//	ui->EndWindow();
	//};
	//ui->End();
}

//--------------------------------------------------------------
void ofApp::exit()
{
	ofxSurfingHelpers::save(g);
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input) {
	audioDevices.audioIn(input);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'g') audioDevices.setVisibleToggle();

	if (key == 's') bScene = !bScene;
	if (key == ' ') bFlip = !bFlip;
	if (key == OF_KEY_RETURN) bShape = !bShape;
}