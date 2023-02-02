#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	sampleRate = 48000;
	bufferSize = 512;
	numBuffers = 4;

	audioDevices.setup(sampleRate, bufferSize, numBuffers);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// scene
	scene.c1 = ofColor::yellow;
	scene.c2 = 24;
	scene.speed = 1;
	scene.amount = 50;

	//if (audioDevices.getIsBangDelta())
	//{
	//	ofLogNotice() << "BANG";
	//}

	ofColor c = scene.c1;
	ofClear(c);
	bool b = audioDevices.getIsBangState();
	// A
	if (bMode) {
		scene.bUpdateable = true;
		if (b) scene.draw();
	}
	// B
	else {
		scene.bUpdateable = b;
		scene.draw();
	}

	audioDevices.setColorSliderGrab(&c);
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
	audioDevices.keyPressed(key);

	if (key == ' ') bMode = !bMode;

	if (key == OF_KEY_F1)
	{
		static bool bTheme = false;
		bTheme = !bTheme;
		if (bTheme) ofxImGuiSurfing::ImGui_ThemeImStyle();
		else ofxImGuiSurfing::ImGui_ThemeMoebiusSurfingV2();
	}
}