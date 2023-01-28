#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	sampleRate = 48000;
	bufferSize = 512;
	numBuffers = 4;

	audioDevices.setup(sampleRate, bufferSize, numBuffers);

	// Log
	audioDevices.getUiPtr()->ClearLogDefaultTags();
	audioDevices.getUiPtr()->AddLogTag("BANG", ofColor::white);

	// Notifier
	string path = "assets/fonts/" + ofToString(FONT_DEFAULT_FILE);
	float sz = 37;
	float round = 15;
	//float round = 40;
	notifier.setup(path, sz, round);
	//notifier.setMessagesLifeTime(4000);

	// Align
	//notifier.setAlignment(surfingNotify::AlignNote_LEFT);
	notifier.setAlignment(surfingNotify::AlignNote_CENTER);
	//notifier.setAlignment(surfingNotify::AlignNote_RIGHT);

	// Settings
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

		//ofClear(61);

		//if (bFlipScene) ofClear(v * 255);
		//else ofClear((1 - v) * 255);

		float gap = 32;
		ofColor c1 = 255 - gap;//white
		ofColor c2 = 0 + gap + 48;//black
		if (!bFlipScene) ofClear(c1);
		else ofClear(c2);

		//--

		// Draws a scene 
		// related to input vu meter

		float v = audioDevices.getVumeterValue();
		float t_ = audioDevices.getThreshold();

		//TODO: tween threshold circle
		static float t = 0;
		float diff = t_ - t;
		t = t + diff * 0.1;

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

		if (bFlipScene) ofSetColor(c1);
		else ofSetColor(c2);

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
		static ofColor cNew = colors[ic];
		static int count = 0;//bangs
		static ofColor cPre;

		// Bang!
		static bool bDo = false;
		if (audioDevices.getIsBang())//will be true until gate is closed!
		{
			// Get bang delta flag only!
			if (!bDo) {
				bDo = true;
				count++;//count bangs

				// Switch color
				cPre = cNew;
				ic++;
				if (ic == colors.size()) ic = 0;
				cNew = colors[ic];

				// Log
				string s;
				s += "#" + ofToString(count);
				audioDevices.getUiPtr()->AddToLog(s, "BANG");

				// Notify
				s = "BANG #" + ofToString(count);
				notifier.addNotification(s, cPre, ofColor(0));

				audioDevices.setColorSliderGrab(&cPre);
			}

			//--

			// Fill flash!
			float a = (audioDevices.getGateProgress());
			// log
			a = ofxSurfingHelpers::reversedExponentialFunction(a * 10.f);//slower
			//a = ofxSurfingHelpers::squaredFunction(a);//faster
			ofSetColor(cPre, 255 * a);
			ofFill();
			ofRectangle rBg = ofGetCurrentViewport();
			ofDrawRectangle(rBg);
		}
		else bDo = false;

		// Awengine 
		// Get notified when Awengine done!
		if (audioDevices.isDoneAwengine())
		{
			// Notify
			string s = "AWE! THR>" + ofToString(audioDevices.getThreshold(), 1);
			notifier.addNotification(s, 255, ofColor(0));
		}

		//--

		// Anticipate next flash color!
		//ofColor c = cNew;
		// Use same color
		ofColor c = cPre;
		ofSetColor(ofColor(c, 255 * ofxSurfingHelpers::getFadeBlink(0.30, 1.0, 0.3)));
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

		//--

		notifier.draw();
	}
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input) {
	audioDevices.audioIn(input);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	// Disable keyboard 
	// when typing into any ui widget.
	if (audioDevices.getUiPtr()->isOverInputText()) return;

	if (key == 'g') {
		audioDevices.setVisibleToggle();

		// Notify
		string s = "GUI: " + ofToString(audioDevices.getIsVisibleGui() ? "ON" : "OFF");
		notifier.addNotification(s);
	}
	if (key == 's') {
		bScene = !bScene;

		// Notify
		string s = "DRAW SCENE: " + ofToString(bScene ? "ON" : "OFF");
		notifier.addNotification(s);
	}

	if (key == ' ') {
		bFlipScene = !bFlipScene;

		// Notify
		string s = "SCENE COLORS: " + ofToString(bFlipScene ? "BLACK/WHITE" : "BLACK/WHITE");
		notifier.addNotification(s);
	}

	if (key == OF_KEY_RETURN) {
		bShape = !bShape;

		// Notify
		string s = "SCENE SHAPE: " + ofToString(bShape ? "CIRCLE" : "RECTANGLE");
		notifier.addNotification(s);
	}

	if (key == OF_KEY_DOWN) {
		audioDevices.threshold -= 0.007;
		ofClamp(audioDevices.threshold, 0, 1);

		// Notify
		string s = "THRESHOLD: " + ofToString(audioDevices.threshold.get(), 2);
		notifier.addNotification(s);
	}
	if (key == OF_KEY_UP) {
		audioDevices.threshold += 0.007;
		ofClamp(audioDevices.threshold, 0, 1);

		// Notify
		string s = "THRESHOLD: " + ofToString(audioDevices.threshold.get(), 2);
		notifier.addNotification(s);
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{
	ofxSurfingHelpers::save(g);
}