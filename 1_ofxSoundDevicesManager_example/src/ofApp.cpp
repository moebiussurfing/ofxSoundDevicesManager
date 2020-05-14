#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

#ifdef USE_ofxWindowApp
	windowApp.setSettingsFps(60);
	windowApp.setSettingsVsync(true);
#else
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
#endif

	//-
	
#ifdef USE_Log
	setupLogDebug();
#endif

	//-

	//these are the default settings
	sampleRate = 44100;
	bufferSize = 512;
	//default numBuffers is 4;

	audioDevices.setup(this, sampleRate, bufferSize);

	//default devices are
	//api: Windows DS
	//input device: #8
	//output device: #4
}

//--------------------------------------------------------------
void ofApp::update() {
	audioDevices.update();

}

//--------------------------------------------------------------
void ofApp::draw() {
	audioDevices.draw();
}

//--------------------------------------------------------------
void ofApp::exit() {
	audioDevices.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'g')
	{
		audioDevices.toggleVisibleGui();
	}
	if (key == 'l')
	{
		audioDevices.toggleVisibleLog();
	}
	if (key == 'a')
	{
		audioDevices.toggleActive();
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

//log
#ifdef USE_Log
//--------------------------------------------------------------
void ofApp::setupLogDebug() {
	string pathFont = "fonts/telegrama_render.otf";
	float fontSize = 8;
	ofFile file(pathFont);
	if (file.exists())//must check this font file is found there
	{
		ofxGuiSetFont(pathFont, fontSize);
	}
	else
	{
		ofLogError(__FUNCTION__) << "Font file not found: " << pathFont;
	}

	float w = 600;
	ofxTextFlow::loadFont(pathFont, 6);
	ofxTextFlow::setBackgroundColor(ofColor(0, 0, 0, 200));
	ofxTextFlow::setBBoxWidth(w);
	ofxTextFlow::setMaxLineNum(40);
	ofxTextFlow::setPosition(10, 400);//left
	ofxTextFlow::setShowingBBox(true);
	ofxTextFlow::setMarginBorders(10);
	ofxTextFlow::setRounded(true, 5);
	ofxTextFlow::setShowing(false);
	ofxTextFlow::setTitle("ofxSoundDevicesManager");
}
#endif
