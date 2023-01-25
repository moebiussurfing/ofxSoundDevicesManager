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
		float t = audioDevices.getThreshold();

		// Bg
		if (bFlipScene) ofClear(v * 255);
		else ofClear((1 - v) * 255);

		float x = ofGetWidth() / 2;
		float y = ofGetHeight() / 2;

		// Shape
		ofPushStyle();
		ofSetCircleResolution(400);

		ofRectangle r = ofGetCurrentViewport();
		float h = ofGetHeight() * v;
		r.setHeight(h);
		r.setPosition(0, y - h / 2);

		ofRectangle rThreshold = ofGetCurrentViewport();
		float ht = ofGetHeight() * t;
		rThreshold.setHeight(ht);
		rThreshold.setPosition(0, y - ht / 2);

		if (bFlipScene) ofSetColor(255);
		else ofSetColor(0);

		// circle
		float radius = r.getHeight() / 2;
		if (bShape) {
			ofFill();
			ofDrawCircle(r.getCenter(), radius);
		}
		// rectangle
		else {
			ofFill();
			ofDrawRectangle(r);
		}

		//--

		// Threshold
		ofSetColor(ofColor::red, 170 * ofxSurfingHelpers::getFadeBlink(0.40, 1.0, 0.15));
		ofSetLineWidth(3);
		// circle
		float radiusTh = rThreshold.getHeight() / 2;
		if (bShape) {
			ofNoFill();
			ofDrawCircle(rThreshold.getCenter(), radiusTh);
		}
		// rectangle
		else {
			ofNoFill();
			ofDrawRectangle(rThreshold);
		}

		// Bang
		if (v > t) {
			ofFill();
			ofRectangle rBg = ofGetCurrentViewport();
			ofDrawRectangle(rBg);

			static ofBitmapFont f;
			string s = " >THRESHOLD  BANG! ";
			float w = f.getBoundingBox(s, 0, 0).getWidth();
			float x = ofGetWidth() / 2 - w / 2;
			ofDrawBitmapStringHighlight(s, glm::vec2(x, 30));
		}

		ofPopStyle();

		//// widgetBeat
		//cBeat.setName("SENSING");
		//if (v >= 1.f) {
		//	cBeat.bang();
		//	cBeat.setName("BANG!");
		//}
		//cBeat.setRadius(radius * 1.F);
		//cBeat.setPosition(x, y);
		//cBeat.draw();
	}

	//--

	audioDevices.drawGui();

	//TODO: WIP: trick to expose ui..
	//auto ui = audioDevices.getUiPtr();
	//ui->Begin();
	//if (ui->BeginWindow("ofApp")) {
	//	ui->Add(bFlipScene, OFX_IM_TOGGLE);
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

	if (key == ' ') {
		bFlipScene = !bFlipScene;

		//if (bFlipScene) {
		//	cBeat.setColorBackground(0);
		//	cBeat.setColorText(255);
		//}
		//else {
		//	cBeat.setColorBackground(255);
		//	cBeat.setColorText(0);
		//}
	}

	if (key == OF_KEY_RETURN) bShape = !bShape;

	if (key == OF_KEY_DOWN) {
		audioDevices.threshold -= 0.02;
		ofClamp(audioDevices.threshold, 0, 1);
	}
	if (key == OF_KEY_UP) {
		audioDevices.threshold += 0.02;
		ofClamp(audioDevices.threshold, 0, 1);
	}
}