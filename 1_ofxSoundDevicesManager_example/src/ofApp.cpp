#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	//these are the default settings
	sampleRate = 44100;
	bufferSize = 512;
	//numBuffers = 2;

	//audioDevices.setup();//setup with default settings
	audioDevices.setup(sampleRate, bufferSize);
	//audioDevices.setup(sampleRate, bufferSize, numBuffers);

	//default devices are
	//Windows DS api
	//input device #8
	//output device #4

	//-

	setupLogDebug();
}

//--------------------------------------------------------------
void ofApp::update() {
	audioDevices.update();

}

//--------------------------------------------------------------
void ofApp::draw() {
	audioDevices.draw();

	drawWaveform();
}

//--------------------------------------------------------------
void ofApp::exit() {
	audioDevices.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

//log
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
	ofxTextFlow::setShowing(true);
	ofxTextFlow::setShowingBBox(true);
	ofxTextFlow::setMarginBorders(10);
	ofxTextFlow::setRounded(true, 5);
	ofxTextFlow::setTitle("ofxSoundDevicesManager");
}

//--------------------------------------------------------------
void ofApp::drawWaveform() {
	ofPushStyle();
	ofPushMatrix();

	ofFill();
	ofSetColor(0, 225);
	ofSetLineWidth(3.0f);
	float _max = 200;

	ofTranslate(0, ofGetHeight() / 4.f);
	ofDrawBitmapStringHighlight("INPUT", ofGetWidth() - 70, -20);
	ofDrawLine(0, 0, 1, waveformInput[1] * _max); //first line
	for (int i = 1; i < (ofGetWidth() - 1); ++i) {
		ofDrawLine(i, waveformInput[i] * _max, i + 1, waveformInput[i + 1] * _max);
	}

	ofTranslate(0, 2 * ofGetHeight() / 4);
	ofDrawBitmapStringHighlight("OUTPUT", ofGetWidth() - 70, -20);
	ofDrawLine(0, 0, 1, waveformOutput[1] * _max); //first line
	for (int i = 1; i < (ofGetWidth() - 1); ++i) {
		ofDrawLine(i, waveformOutput[i] * _max, i + 1, waveformOutput[i + 1] * _max);
	}

	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input) {
	std::size_t nChannels = input.getNumChannels();
	for (size_t i = 0; i < input.getNumFrames(); i++)
	{
		// handle input here
		//Hold the values so the draw method can draw them
		waveformInput[waveInputIndex] = input[i * nChannels];
		if (waveInputIndex < (ofGetWidth() - 1)) {
			++waveInputIndex;
		}
		else {
			waveInputIndex = 0;
		}
	}
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer& output) {
	float volume = 0.1f;
	std::size_t outChannels = output.getNumChannels();
	for (int i = 0; i < output.getNumFrames(); ++i) {

		//output[i * outChannels] = oscillator1.square(532) * 0.5;
		//output[i * outChannels + 1] = output[i * outChannels];

		output[i * outChannels] = volume * ofRandom(-1, 1);
		output[i * outChannels + 1] = output[i * outChannels];

		//Hold the values so the draw method can draw them
		waveformOutput[waveOutputIndex] = output[i * outChannels];
		if (waveOutputIndex < (ofGetWidth() - 1)) {
			++waveOutputIndex;
		}
		else {
			waveOutputIndex = 0;
		}
	}
}