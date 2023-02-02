#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	sampleRate = 48000;
	bufferSize = 512;
	numBuffers = 4;

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
void ofApp::keyPressed(int key)
{
	if (key == 'g') audioDevices.setVisibleToggle();

	if (key == ' ')
	{
		static bool bTheme = false;
		bTheme = !bTheme;
		if (bTheme) ofxImGuiSurfing::ImGui_ThemeImStyle();
		else ofxImGuiSurfing::ImGui_ThemeMoebiusSurfingV2();
	}
}