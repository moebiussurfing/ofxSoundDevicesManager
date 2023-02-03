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
	scene.amount = 50;

	// On bang change the color
	if (audioDevices.getIsBangDelta())
	{
		ofLogNotice() << "BANG";
		ic++;
		ic = ic % colors.size();
		scene.c1 = ofColor(colors[ic], 200);
	}

	ofColor c = scene.c1;
	ofClear(c);

	bool b = audioDevices.getIsBangState();

	scene.c2 = ofColor(24, b ? 200 : 16);//dark

#ifdef USE_WAVEFORM_PLOTS
	audioDevices.waveformPlot.bGui_Plots = !b;
#endif

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