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
	scene.speed = 1;
	scene.amount = 100;

	//if (audioDevices.getIsBangDelta())
	//{
	//	ofLogNotice() << "BANG";
	//}

	ofClear(scene.c1);
	//bool b = audioDevices.getIsBangState();
	//if (b) scene.draw();
	scene.draw();

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

	if (key == OF_KEY_F1)
	{
		static bool bTheme = false;
		bTheme = !bTheme;
		if (bTheme) ofxImGuiSurfing::ImGui_ThemeImStyle();
		else ofxImGuiSurfing::ImGui_ThemeMoebiusSurfingV2();
	}
}