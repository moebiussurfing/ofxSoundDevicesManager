#pragma once

#include "ofMain.h"
#include "ofxSurfingImGui.h"
#define AMP_GAIN_MAX_POWER2 4 /// for plots drawing

class WaveformObject
{

public:

	WaveformObject::WaveformObject()
	{
		setup();
	}

	WaveformObject::~WaveformObject()
	{
	}

	ofParameterGroup params{ "3D Object" };
	ofParameter<int> stroke{ "Stroke", 2, 0, 10 };
	ofParameter<bool> bExpand{ "Expand", false };
	ofParameter<bool> bGui{ "Object Gui", false };
	ofParameter<bool> bA{ "A", false };
	ofParameter<bool> bB{ "B", false };
	ofParameter<bool> bC{ "C", false };
	ofParameter<bool> bD{ "D", false };
	ofParameter<bool> bDraw{ "3D Object", false };
	ofParameter<float> gain{ "Gain", 0, -1, 1 };
	ofParameter<float> g1{ "g1", 0.75, 0, 1 };
	ofParameter<float> g2{ "g2", 0.75, 0, 1 };
	ofParameter<float> g3{ "g3", 0.75, 0, 1 };
	ofParameter<float> g4{ "g4", 0.75, 0, 1 };
	//ofParameter<float> control{ "Control", 0.5, 0, 1 };

	ofParameter<float> threshold{ "Threshold", 0.5, 0, 1 };
	ofParameter<ofColor> cFill{ "c Fill", ofColor(0, 225), ofColor(0), ofColor(0) };
	ofParameter<ofColor> cStroke{ "c Stroke", ofColor(128, 225), ofColor(0), ofColor(0) };

	void setUiPtr(ofxSurfingGui* _ui) {
		ui = _ui;
	}

	ofRectangle r;//box

private:

	ofxSurfingGui* ui;

private:

	ofEasyCam cam;
	ofMesh face, frame;

	int R = 300;
	int v_span = 8;
	int u_span = 16;

	//float g1 = 1.f;
	//float g2 = 1.f;
	//float g3 = 1.f;
	//float g4 = 1.f;
	//float control = 0.f;

	float speed = 1.f;
	bool bRotate = false;

	//-

private:

	void setup() {

		this->frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);

		params.add(bA);
		params.add(bB);
		params.add(bC);
		params.add(bD);
		params.add(bGui);
		params.add(bExpand);
		params.add(bDraw);
		params.add(gain);
		params.add(g1);
		params.add(g2);
		params.add(g3);
		params.add(g4);
		params.add(threshold);
		params.add(cFill);
		params.add(cStroke);
		params.add(stroke);
		//params.add(control);

		//cam.disableMouseInput();
		cam.enableMouseInput();
		//cam.setupPerspective();
	};

	//public:

	//void update() {
	void update(float _plotIn[], int size) {
		// fix workaround
		if (ofGetFrameNum() == 1) cam.disableMouseInput();

		ofSeedRandom(39);

		this->face.clear();
		this->frame.clear();

		for (int v = 0; v < 360; v += v_span) {

			for (auto u = 0; u < 360; u += u_span) {

				int i = ofMap(u, 0, 360, 0, size - 1, true);
				float _gain = ofMap(gain, gain.getMin(), gain.getMax(), 0, AMP_GAIN_MAX_POWER2, true);
				float _value = _plotIn[i] * _gain;

				float noise_seed;
				float noise_value;
				float random_noise_value;

				auto r = 80.f;//default unaffected radium
				auto noise_r = r;
				auto noise_location = this->make_point(R, 90, u + u_span * 0.5, v);

				// hardcoded mode
				// for a less use of ofNoise
				if (bD) {
					noise_seed = ofMap(g1 * _value, 0, AMP_GAIN_MAX_POWER2, 0, 1000, true);
					noise_value = g2 * noise_location.y * 0.002 + _value;

					if (noise_value > threshold)
					{
						random_noise_value = g3 * _value;
						noise_r = g4 * 80.f +
							ofMap(random_noise_value, 0, 1, 0, 270) *
							ofMap(noise_value, 0.5, 1, 0, 1);
					}
				}
				else {
					if (bA) {
						noise_seed = g1 * ofRandom(1000);
					}
					else {
						noise_seed = ofMap(g1 * _value, 0, AMP_GAIN_MAX_POWER2, 0, 1, true);
					}

					if (bB) {
						noise_value = g2 * ofNoise(
							noise_location.x * 0.001,
							noise_location.z * 0.001,
							noise_location.y * 0.002 + ofGetFrameNum() * 0.02);
					}
					else {
						noise_value = g2 * ofNoise(
							noise_location.x * 0.001,
							noise_location.z * 0.001,
							noise_location.y * 0.002 + _value);
					}

					//if (noise_value > 0.5)
					if (noise_value > threshold)
					{
						if (bC) {
							random_noise_value = g3 * ofNoise(noise_seed, ofGetFrameNum() * 0.005);
						}
						else {
							//random_noise_value = g3 * _value * ofGetFrameNum() * 0.005;
							random_noise_value = g3 * _value;
						}

						noise_r = g4 * 80.f +
							ofMap(random_noise_value, 0, 1, 0, 270) *
							ofMap(noise_value, 0.5, 1, 0, 1);
					}
				}

				//--

				vector<glm::vec3> vertices;
				vertices.push_back(this->make_point(R, r, u, v - v_span * 0.5));
				vertices.push_back(this->make_point(R, r, u + u_span, v - v_span * 0.5));
				vertices.push_back(this->make_point(R, r, u + u_span, v + v_span * 0.5));
				vertices.push_back(this->make_point(R, r, u, v + v_span * 0.5));

				vertices.push_back(this->make_point(R, noise_r, u + u_span * 0.5, v));

				int index = face.getNumVertices();
				this->face.addVertices(vertices);

				this->face.addIndex(index + 0); face.addIndex(index + 1); face.addIndex(index + 4);
				this->face.addIndex(index + 1); face.addIndex(index + 2); face.addIndex(index + 4);
				this->face.addIndex(index + 2); face.addIndex(index + 3); face.addIndex(index + 4);
				this->face.addIndex(index + 3); face.addIndex(index + 0); face.addIndex(index + 4);

				this->frame.addVertices(vertices);

				this->frame.addIndex(index + 0); this->frame.addIndex(index + 1);
				this->frame.addIndex(index + 1); this->frame.addIndex(index + 2);
				this->frame.addIndex(index + 2); this->frame.addIndex(index + 3);
				this->frame.addIndex(index + 3); this->frame.addIndex(index + 0);

				this->frame.addIndex(index + 0); this->frame.addIndex(index + 4);
				this->frame.addIndex(index + 1); this->frame.addIndex(index + 4);
				this->frame.addIndex(index + 2); this->frame.addIndex(index + 4);
				this->frame.addIndex(index + 3); this->frame.addIndex(index + 4);
			}
		}
	};

public:

	/*
		void audioReceived(float* input, int bufferSize, int nChannels) {
		// store input in audioInput buffer
		memcpy(audioInput, input, sizeof(float) * bufferSize);
	*/

	//void draw() {
	void draw(float _plotIn[], int size) {
		//void draw(float* _plotIn) {
		//	int size = sizeof(float) * _plotIn;

		if (!bDraw) return;

		//update();
		update(_plotIn, size);

		ofEnableDepthTest();

		ofPushMatrix();
		ofPushStyle();

		this->cam.begin(r);
		{
			if (bRotate) ofRotateY(ofGetFrameNum() * 0.666666 * speed);

			ofSetColor(cFill);
			this->face.draw();

			if (stroke != 0.f) {
				ofSetColor(cStroke);
				ofSetLineWidth(stroke);
				this->frame.drawWireframe();
			}
		}
		this->cam.end();

		ofPopStyle();
		ofPopMatrix();

		ofDisableDepthTest();
	};

	void drawGui() {
		if (!bDraw) return;
		if (!bGui) return;

		//ofDisableDepthTest();

		//ui->Begin();
		bool b;
		if (ui->isThereSpecialWindowFor(bGui)) b = ui->BeginWindowSpecial(bGui);
		else b = ui->BeginWindow(bGui);

		if (b)
		{
			ui->Add(ui->bMinimize, OFX_IM_TOGGLE_ROUNDED);
			ui->Add(bExpand, OFX_IM_TOGGLE_ROUNDED_MINI);
			ui->AddSpacingSeparated();

			ui->Add(gain, OFX_IM_HSLIDER_MINI);
			ui->Add(threshold, OFX_IM_HSLIDER_MINI);
			ui->AddSpacing();

			if (bExpand) {
				ui->Add(cFill, OFX_IM_COLOR_INPUT);
				ui->Add(stroke, OFX_IM_STEPPER);
				if (stroke != 0.f) ui->Add(cStroke, OFX_IM_COLOR_INPUT);
				ui->AddSpacing();
			}

			ui->AddLabelBig("MODES");
			SurfingGuiTypes t = OFX_IM_TOGGLE_ROUNDED_MINI;
			if (!bD) {
				ui->Add(bA, t);
				ui->SameLine();
				ui->Add(bB, t);
				ui->SameLine();
				ui->Add(bC, t);
				ui->SameLine();
			}
			ui->Add(bD, t);

			//ui->Add(control, OFX_IM_HSLIDER);
			//if (ImGui::SliderFloat("control", &control, 0, 1))
			/*
			if (ui->Add(control, OFX_IM_HSLIDER_MINI))
			{
				ui->AddToLog("control: " + ofToString(control, 1));

				//g1 = ofMap(control, 0, 1, 0, 1, true);
				g2 = ofMap(control, 0, 1, 0.75, 1, true);
				g3 = ofMap(control, 0, 1, 0.75, 1, true);
				g4 = ofMap(control, 0, 1, 0.75, 1, true);
			};

			if (!ui->bMinimize && bExpand) {
				ImGui::SliderFloat("g1", &g1, 0, 1);
				ImGui::SliderFloat("g2", &g2, 0, 1);
				ImGui::SliderFloat("g3", &g3, 0, 1);
				ImGui::SliderFloat("g4", &g4, 0, 1);
			}
			*/

			ui->AddSpacingSeparated();

			ui->AddLabel("MODIFIERS");
			ui->Add(g1, OFX_IM_SLIDER);
			ui->Add(g2, OFX_IM_SLIDER);
			ui->Add(g3, OFX_IM_SLIDER);
			ui->Add(g4, OFX_IM_SLIDER);

			if (!ui->bMinimize && bExpand) {
				ui->AddSpacingSeparated();

				if (ui->BeginTree("GEOMETRY")) {
					ImGui::SliderInt("R", &R, 10, 1000);
					//ui->AddSpacing();
					ImGui::SliderInt("v_span", &v_span, 1, 50);
					ImGui::SliderInt("u_span", &u_span, 1, 50);
					ui->EndTree();
				}

				ui->AddSpacingSeparated();

				if (ui->BeginTree("CAMERA")) {
					if (ui->AddToggle("Rotate", bRotate, OFX_IM_TOGGLE)) {
					};
					if (bRotate) ImGui::SliderFloat("Speed", &speed, 0, 1);
					static bool b;
					b = cam.getMouseInputEnabled();
					if (ui->AddToggle("MouseCam", b, OFX_IM_TOGGLE)) {
						b ? cam.enableMouseInput() : cam.disableMouseInput();
					};
					ui->EndTree();
				}
			}

			//ui->Add(ui->bAdvanced, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
			ui->AddSpacingSeparated();

			if (ui->AddButton("Reset")) {
				doReset();
			}
		}

		if (ui->isThereSpecialWindowFor(bGui)) ui->EndWindowSpecial();
		else ui->EndWindow();

		//ui->DrawWindowAdvanced();
		//ui->drawWindowAlignHelpers();
		//ui->DrawWindowLog();

		//ui->End();
	};

private:
	void doReset() {
		bA = true;
		bB = false;
		bC = false;
		bD = false;
		g1 = 0.75;
		g2 = 1.;
		g3 = 0.75;
		g4 = 1.;
		threshold = .5;
		gain = 0;
	};

	//--------------------------------------------------------------
	glm::vec3 make_point(float R, float r, float u, float v)
	{
		// 数学デッサン教室 描いて楽しむ数学たち　P.31

		u *= DEG_TO_RAD;
		v *= DEG_TO_RAD;

		auto x = (R + r * cos(u)) * cos(v);
		auto y = (R + r * cos(u)) * sin(v);
		auto z = r * sin(u);

		return glm::vec3(x, y, z);
	};

};

