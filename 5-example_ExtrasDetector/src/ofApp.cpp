#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	sampleRate = 48000;
	bufferSize = 512;
	numBuffers = 4;

	audioDevices.setup(sampleRate, bufferSize, numBuffers);

	textBox.setFontSize(65);
	textBox.setup();
	textBox.setMode(TextBoxWidget::BOX_LAYOUT::CENTER);
	textBox.setTheme(true);

	ofxSurfingHelpers::load(g);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	//--

	audioDevices.drawGui();

	//--

	if (!bScene) ofClear(64);
	else
	{
		// Bg
		ofClear(61);
		//if (bFlipScene) ofClear(v * 255);
		//else ofClear((1 - v) * 255);
		//if (!bFlipScene) ofClear(255);
		//else ofClear(0);

		//--

		// Draws a scene 
		// related to input vu meter

		float v = audioDevices.getVumeterValue();
		float t_ = audioDevices.getThreshold();

		//TODO: tween
		static float t = 0;
		//t = (t_ - t) * 0.25;
		//t = t_;
		float diff = t_ - t;
		t = t + diff * 0.5;

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

		// Detector
		// Threshold

		static int ic = 0;
		static ofColor c = colors[ic];
		static int count = 0;//bangs
		static ofColor cPre;

		// Bang!
		static bool bDo = false;
		if (audioDevices.getIsBang())
		{
			// Get bang delta flag only!
			if (!bDo) {
				bDo = true;
				count++;//count bangs

				//switch color
				cPre = c;
				ic++;
				if (ic == colors.size()) ic = 0;
				c = colors[ic];
			}

			//--

			// Fill flash!
			ofSetColor(cPre, 255 * (audioDevices.getGateProgress()));
			ofFill();
			ofRectangle rBg = ofGetCurrentViewport();
			ofDrawRectangle(rBg);

			//string s = "         \n";
			//s += "  BANG !  \n";
			//s += "  #" + ofToString(count) + "\n";
			//s += "         ";
			//static ofBitmapFont f;
			//float w = f.getBoundingBox(s, 0, 0).getWidth();
			//float h = f.getBoundingBox(s, 0, 0).getHeight();
			//float x = ofGetWidth() / 2 - w / 2;
			//float y = ofGetHeight() / 2 - h / 2;
			//////top center
			////ofDrawBitmapStringHighlight(s, glm::vec2(x, 60));
			////center 
			//ofDrawBitmapStringHighlight(s, glm::vec2(x, y));

			// Draw
			string s;
			s += "\n";
			s += "BANG!\n";
			s += " #" + ofToString(count);
			s += "\n";
			textBox.setText(s);
			textBox.draw();
		}
		else bDo = false;

		//--

		ofSetColor(c, 255);
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

		ofPopStyle();
	}
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
	// disable keyboard when typing 
	if (audioDevices.getUiPtr()->isOverInputText()) return;

	if (key == 'g') audioDevices.setVisibleToggle();

	if (key == 's') bScene = !bScene;

	if (key == ' ') bFlipScene = !bFlipScene;

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