#pragma once

/*

	EXAMPLE

	// ofApp.h
	#include "ofxSoundDevicesManager.h"
	#include "surfingDetector.hpp"

	ofxSoundDevicesManager audioDevices;
	SurfingDetector surfingDetector;

	// setup
	surfingDetector.setAudioDevicesPtr(&audioDevices);

	// draw
	surfingDetector.draw();

	// keyPressed
	surfingDetector.keyPressed(key);

*/


#include "ofMain.h"

#include "ofxSoundDevicesManager.h"
#include "surfingNotify.h"
#include "ofxSurfingBoxInteractive.h"

class SurfingDetector
{

public:

	SurfingDetector::SurfingDetector() {
	};

	SurfingDetector::~SurfingDetector() {
		exit();
	};

	ofParameter<float> scale{ "Scale", 0.5, 0, 1 };

private:

	ofParameter<bool> bScene{ "Scene", true };
	ofParameter<bool> bFlipScene{ "Flip", true };
	ofParameter<bool> bShapeType{ "ShapeType", true };
	ofParameterGroup g{ "ParamsSurfingDetector", bFlipScene, bShapeType, bScene, scale };

	vector<ofColor> colors{ ofColor::red, ofColor::green, ofColor::blue, ofColor::yellow, ofColor::orange, ofColor::violet, ofColor::turquoise, ofColor::chocolate, ofColor::aquamarine };

	surfingNotify notifier;

	ofxSoundDevicesManager* audioDevices = nullptr;
	//ofxSoundDevicesManager* audioDevices;

	string path = "";

	ofxSurfingBoxInteractive box;

public:

	void setAudioDevicesPtr(ofxSoundDevicesManager* audioDevices_)
	{
		audioDevices = audioDevices_;

		path = audioDevices->getPathGlobal() + "/SurfingDetector.json";

		//box.setUseBorder(true);
		box.setLockAspectRatio();
		//box.setLockW();
		box.setTransparent(true);
		box.setPath(audioDevices->getPathGlobal());
		box.setName("SurfingDetectorBox");
		box.setModeLayout(ofxSurfingBox::BOX_LAYOUT::CENTER_CENTER);
		box.setup();

		setup();
	};

private:

	void setup()
	{
		if (audioDevices == nullptr) {
			ofLogError("ofxSoundDevicesManager") << "audioDevices == nullptr";
			return;
		}

		// Log
		audioDevices->getUiPtr()->ClearLogDefaultTags();
		audioDevices->getUiPtr()->AddLogTag("BANG", ofColor::white);

		// Size
		// font size that scale the boxes
		float sz = 34;

		// Notifier
		string pathFont = "assets/fonts/" + ofToString(OFX_IM_FONT_DEFAULT_FILE);
		float round = 15;
		//float round = 40;

		notifier.setup(pathFont, sz, round);
		//notifier.setMessagesLifeTime(4000);

		// Align
		//notifier.setAlignment(surfingNotify::AlignNote_LEFT);
		notifier.setAlignment(surfingNotify::AlignNote_CENTER);
		//notifier.setAlignment(surfingNotify::AlignNote_RIGHT);

		// Settings
		ofxSurfingHelpers::loadGroup(g, path);
	};

public:

	// Draws scene testing
	void draw()
	{
		//TODO: scale
		//const float MAX_SCALE = 5.f;
		//float _scale = 1;
		//if (scale.get() == 0.5) _scale = 1;
		//else if (scale.get() > 0.5) _scale = ofMap(scale.get(), 0.5, 1, 1, MAX_SCALE, true);
		//else if (scale.get() < 0.5) _scale = ofMap(scale.get(), 0, 0.5, 1, 1.f / MAX_SCALE, true);

		//ofPushMatrix();
		////ofTranslate();
		//ofScale(scale);

		//--

		if (audioDevices == nullptr) {
			ofLogError("ofxSoundDevicesManager") << "audioDevices == nullptr";
			ofLogError("ofxSoundDevicesManager") << "On ofApp::setup(), you need to call something like surfingDetector.setAudioDevicesPtr(&audioDevices);";
			return;
		}

		//--

		if (!bScene) ofClear(64);
		else
		{
			ofRectangle rBox = box.getRectangle();

			static int ic = 0;
			static ofColor cNew = colors[ic];
			static ofColor cPre;

			// Bg

			//ofClear(61);

			//if (bFlipScene) ofClear(v * 255);
			//else ofClear((1 - v) * 255);

			float cgap = 32;
			char cmin = 0;
			char cmax = 225;
			ofColor c1 = cmax - cgap;//white
			ofColor c2 = cmin + cgap + 48;//black
			if (!bFlipScene) ofClear(c1);
			else ofClear(c2);

			//--

			// Draws a scene 
			// related to input vu meter

			float v = audioDevices->getVumeterValue();
			float t_ = audioDevices->getThreshold();

			//TODO: tween threshold circle
			static float t = t_;
			float diff = t_ - t;
			bool bIsShapeChanging = (abs(diff) > 0.01f);
			//cout << bIsShapeChanging << ":" << diff << endl;
			t = t + diff * 0.05;

			//--

			// 1. Signal / VU rectangle 

			// Shape
			ofPushStyle();
			ofSetCircleResolution(400);

			ofRectangle r = rBox;
			float h = r.getHeight() * v;

			//r.translateY(rBox.getHeight() / 2.f - h / 2.f);//box
			//r.translateY(- h / 2.f);//box

			// Forced center mode
			//float x = ofGetWidth() / 2;
			//float y = ofGetHeight() / 2;
			//ofRectangle r = ofGetCurrentViewport();
			//float h = ofGetHeight() * v;
			//r.setPosition(0, y - h / 2);

			r.setHeight(h);

			//--

			// 2. Threshold rectangle 

			ofRectangle rThreshold = rBox;

			//ofRectangle rThreshold = ofGetCurrentViewport();
			//float ht = ofGetHeight() * t;
			float ht = rThreshold.getHeight() * t;

			rThreshold.translateY(rBox.getHeight() / 2.f - ht / 2.f);//box

			rThreshold.setHeight(ht);
			//rThreshold.setPosition(0, y - ht / 2);

			// B&W
			if (bFlipScene) ofSetColor(c1);
			else ofSetColor(c2);
			ofSetColor(cPre);

			float radius = r.getHeight() / 2;

			// A. Circle
			if (bShapeType)
			{
				//r.translateY(rBox.getHeight() / 2.f);
				//r.translateY(rBox.getHeight() / 2.f - ht / 2.f);
				//r.translateY(rBox.getHeight() / 2.f - radius / 2.f);

				ofFill();
				ofDrawCircle(rBox.getCenter(), radius);
				//ofDrawCircle(r.getCenter(), radius);
			}
			// B. Rectangle
			else
			{
				r.translateY(rBox.getHeight() / 2.f - h / 2.f);

				ofFill();
				ofDrawRectangle(r);
			}

			//--

			// Detector
			// Threshold

			// Bang!
			static bool bDo = false;
			static int count = 0;//bangs
			if (audioDevices->getIsBangState())//will be true until gate is closed!
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

					// Log ui
					string s;
					s += "#" + ofToString(count);
					audioDevices->getUiPtr()->AddToLog(s, "BANG");

					// Notify
					s = "BANG #" + ofToString(count);

					notifier.addNotification(s, cPre, ofColor(0));

					audioDevices->setColorSliderGrab(&cPre);
				}

				//--

				// Fill flash!
				// full screen / viewport rectangle

				float a = (audioDevices->getGateProgress());
				// log
				a = ofxSurfingHelpers::reversedExponentialFunction(a * 10.f);//slower
				//a = ofxSurfingHelpers::squaredFunction(a);//faster
				ofSetColor(cPre, 255 * a);
				ofFill();

				ofRectangle rBg = ofGetCurrentViewport();
				//ofRectangle rBg = rBox;

				ofDrawRectangle(rBg);
			}
			else bDo = false;

			//--

			// make slider grab blink
			if (audioDevices->getIsAwengineEnabled()) {
				float a = ofxSurfingHelpers::getFadeBlink(0.60, 1.0, 0.15);
				static ofColor cGrab;
				cGrab = ofColor(cPre.r, cPre.g, cPre.b, cPre.a * a);
				audioDevices->setColorSliderGrab(&cGrab);
			}

			//--

			// AWENGINE 
			// Get notified when AWENGINE done!
			if (audioDevices->isDoneAwengine())
			{
				// Apply engine

				string s = "THRS ";
				//string s = "AWE! THR>" + ofToString(audioDevices->getThreshold(), 1);

				static float thrPre = 0;
				float thr = audioDevices->getThreshold();
				float diff = 0;
				if (thr != thrPre) {
					diff = thr - thrPre;
					thrPre = thr;
					s += ofToString(diff > 0 ? "+" : "");
					s += ofToString(diff, 2);
				}

				// notify
				if (audioDevices->isDebug()) notifier.addNotification(s, 255, ofColor(0));
			}

			//--

			// Anticipate next flash color!
			//ofColor c = cNew;
			// Use same color
			//ofColor c = cPre;
			ofColor c = bFlipScene ? c1 : c2;
			float a = 1;
			if (bIsShapeChanging) a = ofxSurfingHelpers::getFadeBlink(0.0, 1.0, 0.5);
			ofSetColor(ofColor(c, 255 * a));
			ofSetLineWidth(2.f);
			ofNoFill();

			bool bExtra = false;//draw a thin black shape

			// circle
			float radiusTh = rThreshold.getHeight() / 2;

			//if(radiusTh == 0) radiusTh= ofxSurfingHelpers::getFadeBlink(0, 3);//mark if 0

			if (bShapeType) {
				ofDrawCircle(rThreshold.getCenter(), radiusTh);

				if (bExtra) {
					ofSetLineWidth(1.f);
					ofSetColor(0, 64);
					ofDrawCircle(rThreshold.getCenter(), radiusTh - 1);
				}
			}

			// rectangle
			else {
				ofDrawRectangle(rThreshold);

				if (bExtra) {
					ofSetLineWidth(1.f);
					ofSetColor(0, 64);
					ofDrawRectangle(
						ofRectangle(rThreshold.x + 1, rThreshold.y + 1, rThreshold.width - 2, rThreshold.height - 2));
				}
			}

			//--

			box.draw();

			ofPopStyle();

			//--

			notifier.draw();
		}
		
		//ofPopMatrix();
	};

	void keyPressed(int key)
	{
		if (audioDevices == nullptr) return;

		// Disable keyboard 
		// when typing into any ui widget.
		if (audioDevices->getUiPtr()->isMouseOverInputText()) return;

		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << " " << char(key);


		if (key == 'g') {
			audioDevices->setVisibleToggle();

			// Notify
			string s = "GUI " + ofToString(audioDevices->getIsVisibleGui() ? "ON" : "OFF");
			notifier.addNotification(s);
		}
		if (key == 's') {
			bScene = !bScene;

			// Notify
			string s = "SCENE " + ofToString(bScene ? "ON" : "OFF");
			notifier.addNotification(s);
		}

		if (key == ' ') {
			bFlipScene = !bFlipScene;

			// Notify
			string s = "COLORS " + ofToString(bFlipScene ? "BLACK/WHITE" : "WHITE/BLACK");
			notifier.addNotification(s);
		}

		if (key == OF_KEY_RETURN) {
			bShapeType = !bShapeType;

			// Notify
			string s = "SCENE " + ofToString(bShapeType ? "CIRCLE" : "RECTANGLE");
			notifier.addNotification(s);
		}

		if (key == OF_KEY_DOWN) {
			audioDevices->threshold -= 0.007;
			ofClamp(audioDevices->threshold.get(), 0, 1);

			// Notify
			string s = "THRS " + ofToString(audioDevices->threshold.get(), 2);
			notifier.addNotification(s);
		}
		if (key == OF_KEY_UP) {
			audioDevices->threshold += 0.007;
			ofClamp(audioDevices->threshold.get(), 0, 1);

			// Notify
			string s = "THRS " + ofToString(audioDevices->threshold.get(), 2);
			notifier.addNotification(s);
		}
	};

private:

	void exit()
	{
		ofxSurfingHelpers::saveGroup(g, path);
	};
};
