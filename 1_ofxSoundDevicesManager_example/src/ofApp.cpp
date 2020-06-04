#include "ofApp.h"

//#define USE_ofBaseApp_Pointer

//--------------------------------------------------------------
void ofApp::setupSoundDevicesManager() {

	//these are the default settings
	sampleRate = 44100;
	bufferSize = 512;
	//default numBuffers is 4;

#ifdef USE_ofBaseApp_Pointer
	audioDevices.setup(this, sampleRate, bufferSize);
#else
	audioDevices.setup(sampleRate, bufferSize);
#endif

	//default devices are
	//api: Windows DS
	//input device: #8
	//output device: #4
}

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetBackgroundColor(32);

#ifdef USE_ofxWindowApp
	//windowApp.setFrameRate(60);
	//windowApp.setSetVerticalSync(true);
#else
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
#endif

	//-
	
#ifdef USE_Log
	setupLogDebug();
#endif

	//-
	
	setupSoundDevicesManager();
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
#ifdef USE_Log
	if (key == 'l')
	{
		audioDevices.toggleVisibleLog();
	}
#endif
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

////TODO:
////must update all hanlding to ofsoundBuffer !
////for when we are not using ofSoundBuffer?
////to waveforms plot 
////--------------------------------------------------------------
//void ofApp::audioIn(float * input, int bufferSize, int nChannels)
//{
//	//ofSoundBuffer _soundBuffer;
//	ofSoundBuffer _soundBuffer;
//
//	//ofSoundBuffer(short *shortBuffer, size_t numFrames, size_t numChannels, unsigned int sampleRate);
//	//copyFrom(...)void ofSoundBuffer::copyFrom(const float *floatBuffer, size_t numFrames, size_t numChannels, unsigned int sampleRate)
//	_soundBuffer.copyTo(input, bufferSize, nChannels, sampleRate);//?
//
//	audioDevices.audioIn(_soundBuffer);
//
//
//	_soundBuffer = input.getBuffer().data();
//
//	//_soundBuffer.allocate(bufferSize, nChannels);
//	//*float out of an ofSoundBuffer just call:
//	//*float myBuffer = buffer.getBuffer().data();
//	//I did tried using buffer.getBuffer() but didn’t realized I had to dig to .data() to get the raw float!
//	//gist->processAudio(output.getBuffer().data(), output.getNumFrames(), output.getNumChannels(), output.getSampleRate());
//}

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
