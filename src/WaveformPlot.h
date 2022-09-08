#pragma once

/*

	fft plot example https://github.com/moebiussurfing/examplesOfxMaxim

*/


#include "ofMain.h"
#include "ofxSurfingBoxHelpText.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingImGui.h"

#include "SurfPresets.h"
#include "imgui_stdlib.h"

//--

#define SIZE_BUFFER 4096

#define SOUND_DEVICES_DISABLE_OUTPUT

//--

#define AMP_GAIN_MAX_POWER 10 /// for plots drawing

class WaveformPlot
{

public:

	WaveformPlot()
	{
		// Font label text
		size_TTF = 11;
		name_TTF = "JetBrainsMonoNL-ExtraBold.ttf";
		string pathRoot = "assets/fonts/";
		path_TTF = pathRoot + name_TTF;
		bool bLoaded = myFont.load(path_TTF, size_TTF, true, true);
		path_TTF = pathRoot + "telegrama_render.otf";
		if (!bLoaded) bLoaded = myFont.load(path_TTF, size_TTF, true, true);
		else if (!bLoaded) bLoaded = myFont.load(OF_TTF_MONO, size_TTF, true, true);
	};

	~WaveformPlot()
	{
		ofRemoveListener(params_PlotsWaveform.parameterChangedE(), this, &WaveformPlot::Changed_params_PlotsWaveform);
	};

private:

	//--

	//TODO: mesh
	ofVboMesh meshWaveform;

public:

	/*
	*
	void makeWaveformMesh(ofSoundBuffer buffer)
	{
		if (buffer.size() > 0) {

			if (meshWaveform.getNumVertices() != buffer.getNumFrames())
			{
				meshWaveform.clear();
				meshWaveform.setMode(OF_PRIMITIVE_LINE_STRIP);
				meshWaveform.setUsage(GL_DYNAMIC_DRAW);

				float h = 1.0f;

				float xInc = 1.0f / (float)(buffer.getNumFrames());
				glm::vec3 v;
				v.x = 0;
				for (int i = 0; i < buffer.getNumFrames(); i++) {
					{
						v.y = ofMap(0, -1, 1, h, h);
						meshWaveform.addVertex(v);
					}
					v.x += xInc;
				}
			}
		}
	};

	void updateWaveformMesh(ofSoundBuffer buffer) {
		auto chans = buffer.getNumChannels();
		if (chans > 0 && buffer.size() > 0) {
			float h = 1.0f / float(chans);

			h *= 500;

			auto& wv = meshWaveform.getVertices();
			for (size_t i = 0; i < wv.size(); i++) {
				wv[i].y = ofMap(buffer[i * chans], -1, 1, h, h);
			}
		}
	};

	*/

	//--

	ofxSurfingGui* ui;
	void setUiPtr(ofxSurfingGui* _ui) {
		ui = _ui;
		surfPresets.setUiPtr(_ui);
	}

	SurfPresets surfPresets;

private:

	string pathGlobal = "ofxSoundDevicesManager/";
	string pathSettings = "ofxSoundDevicesManager";

	bool bDISABLE_CALBACKS = false;//to avoid callback crashes or to enable only after setup()

	ofxSurfingBoxInteractive boxPlotIn;
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
	ofxSurfingBoxInteractive boxPlotOut;
#endif

	//--

	//TODO: WIP
	//private:
	//ofFbo fbo;
	////TODO: WIP
	//bool bUseFbo = false;
	//bool bUpdateFbo = false;
	//void initFbo(int width = 320, int height = 240)
	//{
	//	if (!bUseFbo) return;
	//	//if (bUseFbo && !fbo.isAllocated())
	//	{
	//		fbo.allocate(width, height);
	//		fbo.begin();
	//		ofClear(0, 0, 0, 0);
	//		fbo.end();
	//		bUpdateFbo = true;
	//	}
	//}

	//--

private:

	ofTrueTypeFont myFont;
	std::string path_TTF;
	std::string name_TTF;
	int size_TTF;

	//--

public:

	ofParameterGroup params_PlotsWaveform;
	ofParameterGroup params_PlotsEsthetics;
	ofParameterGroup params{ "WaveformPlot" };

#ifdef USE_WAVEFORM_PLOTS
	ofParameter<int> index{ "Index", 0, 0, 0 };
#endif

	ofParameter<bool> bGui{ "WAVEFORM", true };;
	ofParameter<bool> bGui_PlotIn{ "Plot In", true };
	ofParameter<bool> bGui_PlotOut{ "Plot Out", false };

public:

	ofParameter<bool> bGui_Plots;
	ofParameter<bool> bGui_Main{ "WAVEFORM", true };
	ofParameter<bool> bGui_Settings{ "PLOT SETTINGS", false };
	//ofParameter<bool> bGui_Main{ "SOUND PLOTS", true };

	ofParameter<float> gain{ "Gain", 0, -1, 1 };

	// data arrays
	float plotIn[SIZE_BUFFER]; // make this bigger, just in case
	float plotOut[SIZE_BUFFER]; // make this bigger, just in case

private:

	//ofParameter<int> plotType{ "Type", 0, 0, 3 };
	//vector<string>plotTypeNames = { "Scope", "Lines", "Bars" , "Circles" };

	//--

private:

	ofParameter<bool> W_bScope{ "Scope", true };
	ofParameter<bool> W_bLine{ "Line", true };
	ofParameter<bool> W_bBars{ "Bars", false };
	ofParameter<bool> W_bCircle{ "Circle", false };
	ofParameter<float> W_Spread{ "Spread", 0, 0, 1 };
	ofParameter<float> W_WidthRad{ "W/Rad", 0.5f, 0, 1 };
	ofParameter<float> W_WidthMin{ "Min", 0.1f, 0, 1 };
	ofParameter<bool> W_bAbs{ "Abs", true };
	ofParameter<bool> W_bBottom{ "Bottom", false };
	ofParameter<float> W_Alpha{ "Alpha", 0.5, 0, 1 };
	ofParameter<bool> W_bHLine{ "H Line", true };
	ofParameter<bool> W_bTransparent{ "Transparent", true };
	ofParameter<bool> W_bClamp{ "Clamp", true };
	ofParameter<bool> W_bClampItems{ "ClampItems", true };
	ofParameter<bool> W_bMirror{ "Mirror", false };
	ofParameter<float> W_Rounded{ "Rounded", 0, 0, 1 };
	ofParameter<int> W_LineWidthScope{ "L Scope", 3, 1, 10 };
	ofParameter<int> W_LineWidthLines{ "L Lines", 3, 1, 10 };
	ofParameter<bool> W_bLabel{ "Label", true };
	ofParameter<void> W_vReset{ "Reset" };

	ofParameter<ofColor> cPlot{ "c Plot", ofColor(0, 225), ofColor(0), ofColor(0) };
	ofParameter<ofColor> cPlotBg{ "c Bg", ofColor(64, 128), ofColor(0), ofColor(0) };
	ofParameter<ofColor> cText{ "c Text", ofColor(255, 225), ofColor(0), ofColor(0) };

public:

	void setPath(string p)
	{
		pathGlobal = p;

		boxPlotIn.setPathGlobal(pathGlobal);

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		boxPlotOut.setPathGlobal(pathGlobal);
#endif
	}

	void startup()
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__);
		bDISABLE_CALBACKS = false;

		doReset();

		surfPresets.startup();
		surfPresets.doLoad();
	}

	void update()
	{
		if (boxPlotIn.isChangedShape())
		{
			//TODO:
			ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << "Box Input plot changed!";
			//initFbo(boxPlotIn.getWidth(), boxPlotIn.getHeight());
		}
	}

	void setup()
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__);

		// Plot boxes

		ofColor cBg = ofColor(0, 255);

		boxPlotIn.setUseBorder(true);
		boxPlotIn.setBorderColor(cBg);
		boxPlotIn.setPosition(400, 10);
		boxPlotIn.bGui.setName("Plot In");
		boxPlotIn.setup();

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		boxPlotOut.setUseBorder(true);
		boxPlotOut.setBorderColor(cBg);
		boxPlotOut.setPosition(400, 410);
		boxPlotOut.bGui.setName("Plot Out");
		boxPlotOut.setup();
#endif
		//--

		// Waveform

		bGui_Plots.set("Plot Signals", true);

		// for presets
		params_PlotsWaveform.setName("PLOTS WAVEFORM");
		params_PlotsWaveform.add(gain);
		params_PlotsWaveform.add(W_bScope);
		params_PlotsWaveform.add(W_bLine);
		params_PlotsWaveform.add(W_bBars);
		params_PlotsWaveform.add(W_bCircle);
		params_PlotsWaveform.add(W_Spread);
		params_PlotsWaveform.add(W_bAbs);
		params_PlotsWaveform.add(W_bHLine);
		params_PlotsWaveform.add(boxPlotIn.bUseBorder);
		params_PlotsWaveform.add(W_bClamp);
		params_PlotsWaveform.add(W_bClampItems);
		params_PlotsWaveform.add(W_Rounded);
		params_PlotsWaveform.add(W_bMirror);
		params_PlotsWaveform.add(W_WidthRad);
		params_PlotsWaveform.add(W_WidthMin);
		params_PlotsWaveform.add(W_bBottom);
		params_PlotsWaveform.add(W_bLabel);

		params_PlotsEsthetics.setName("ESTHETICS");
		params_PlotsEsthetics.add(W_bTransparent);
		params_PlotsEsthetics.add(W_Alpha);
		params_PlotsEsthetics.add(W_LineWidthScope);
		params_PlotsEsthetics.add(W_LineWidthLines);
		params_PlotsEsthetics.add(cPlot);
		params_PlotsEsthetics.add(cPlotBg);
		params_PlotsEsthetics.add(cText);
		params_PlotsWaveform.add(params_PlotsEsthetics);

		ofAddListener(params_PlotsWaveform.parameterChangedE(), this, &WaveformPlot::Changed_params_PlotsWaveform);

		//--

		//TODO: Presets
		surfPresets.setPath(pathGlobal);
		surfPresets.AddGroup(params_PlotsWaveform);

		W_vReset.makeReferenceTo(surfPresets.vReset);

		//--

		// Gui
		/*
		ui.setName("WaveformPlot");
		ui.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
		ui.setup();

		ui.addWindowSpecial(bGui_Main);
		ui.addWindowSpecial(bGui_Settings);

		ui.startup();
		*/

		//--


#ifdef USE_WAVEFORM_PLOTS
		index.makeReferenceTo(surfPresets.index);
#endif
		//TODO:
		params.add(bGui);
		params.add(bGui_Plots);
		params.add(bGui_Main);
		params.add(bGui_Settings);
		params.add(bGui_PlotIn);
		params.add(bGui_PlotOut);
		params.add(W_vReset);

		ofAddListener(params.parameterChangedE(), this, &WaveformPlot::Changed_params_PlotsWaveform);

		//TODO:
		boxPlotIn.bGui.makeReferenceTo(bGui_PlotIn);

		//--

		//TODO:
		//initFbo();
	};

	void drawImGui()
	{
		if (!bGui) return;

		//ui->Begin();
		{
			//--

			// 1. Plots

			//if(bGui_Main)IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_SMALL;

			if (ui->BeginWindowSpecial(bGui_Main))
			{
				ui->Add(ui->bMinimize, OFX_IM_TOGGLE_ROUNDED);
				ui->AddSpacingSeparated();
				ui->Add(bGui_Settings, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				ui->AddSpacingSeparated();

				ui->Add(bGui_PlotIn, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				if (bGui_PlotIn)
					if (!ui->bMinimize) {
						ui->Indent();
						ui->Add(boxPlotIn.bEdit, OFX_IM_TOGGLE_ROUNDED_MINI);
						ui->Add(boxPlotIn.bUseBorder, OFX_IM_TOGGLE_ROUNDED_MINI);
						ui->Unindent();
					}

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
				ui->AddSpacingSeparated();
				ui->Add(bGui_PlotOut, OFX_IM_TOGGLE_ROUNDED);
				if (bGui_PlotOut)
					if (!ui->bMinimize) {
						ui->Indent();
						ui->Add(boxPlotOut.bEdit, OFX_IM_TOGGLE_ROUNDED_MINI);
						ui->Add(boxPlotOut.bUseBorder, OFX_IM_TOGGLE_ROUNDED_MINI);
						ui->Unindent();
					}
#endif

				ui->AddSpacingSeparated();

				ui->AddLabelHuge("Style", true);
				//ui->AddSpacing();

				////--

				//// 2. Presets
				
				surfPresets.drawImGui(false);
				
				//{
				//	//TODO:
				//	static string _namePreset = "";
				//	//static bool bTyping = false;
				//	static bool bInputText = false;
				//	//string s = "presetName";
				//	string s = surfPresets.filename;


				//	ui->AddLabelBig("Presets", true, true);
				//	if (!ui->bMinimize) {
				//		ui->Add(surfPresets.vLoad, OFX_IM_BUTTON_SMALL, 2, true);

				//		if (ui->Add(surfPresets.vSave, OFX_IM_BUTTON_SMALL, 2))
				//		{
				//			bInputText = false;
				//			_namePreset = s;
				//		};

				//		if (ui->Add(surfPresets.vNew, OFX_IM_BUTTON_SMALL, 2, true))
				//		{
				//			if (!bInputText) bInputText = true;
				//			_namePreset = "";
				//			surfPresets.setFilename(_namePreset);
				//		};
				//		ui->Add(surfPresets.vReset, OFX_IM_BUTTON_SMALL, 2);

				//		ui->Add(surfPresets.vScan, OFX_IM_BUTTON_SMALL, 2, true);
				//		ui->Add(surfPresets.vDelete, OFX_IM_BUTTON_SMALL, 2);

				//		//--

				//		ui->AddSpacing();

				//		if (bInputText)
				//		{
				//			int _w = ui->getWidgetsWidth() * 0.9f;
				//			ImGui::PushItemWidth(_w);
				//			{
				//				bool b = ImGui::InputText("##NAME", &s);
				//				if (b) {
				//					ofLogNotice("WaveformPlot") << "InputText:" << s.c_str();
				//					surfPresets.setFilename(s);
				//				}
				//			}

				//			ImGui::PopItemWidth();
				//		}
				//	}

				//	//--

				//	// Combo
				//	ui->AddComboButtonDual(surfPresets.index, surfPresets.filenames);

				//	/*
				//	if (!ui->bMinimize)
				//	{
				//		// preset name
				//		if (_namePreset != "") ui->AddLabel(_namePreset.c_str());
				//	}
				//	*/
				//}

				////--

				//TODO:
				// Files

				/*
				guiManager.Add(bFiles, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
				if (bFiles)
				{
					guiManager.Indent();
					{
						// Paths
						{
							bool bOpen = false;
							ImGuiTreeNodeFlags _flagt = (bOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None);
							//_flagt |= ImGuiTreeNodeFlags_Framed;

							if (ImGui::TreeNodeEx("Path", _flagt))
							{
								ImGui::TextWrapped(path_Presets.data()); // -> show path
								ImGui::TreePop();
							}
						}

						// Files
						// Buttons Selector for each file
						if (ofxImGuiSurfing::filesPicker(path_Presets, nameSelected, index, { "json" }))
						{
							// Buttons Matrix

							//TODO:
							// Index back not working
							// this is a workaround
							// could fail on macOS/Linux -> requires fix paths slashes

							for (int i = 0; i < dir.size(); i++)
							{
								std::string si = ofToString(i);
								if (i < 10) si = "0" + si;
								std::string ss = name_Root + "_" + si;
								fileName = ss;

								auto s0 = ofSplitString(nameSelected, "\\", true);
								std::string s1 = s0[s0.size() - 1]; // filename
								auto s = ofSplitString(s1, ".json");

								std::string _nameSelected = s[0];

								if (_nameSelected == fileName)
								{
									index = i;
								}
							}

							ofLogNotice(__FUNCTION__) << "Picked file " << nameSelected << " > " << index;
						}
					}
					guiManager.Unindent();
				}
				*/

				//--

				/*
				ui->AddSpacingSeparated();

				ui->AddLabelBig("Predefined", true);

				// Predefined Styles type
				if (ui->AddButton("<", OFX_IM_BUTTON_SMALL, 2, true)) {
					if (plotType == plotType.getMin()) plotType = plotType.getMax();
					else plotType = plotType - 1;
				}
				if (ui->AddButton(">", OFX_IM_BUTTON_SMALL, 2)) {
					if (plotType == plotType.getMax()) plotType = plotType.getMin();
					else plotType = plotType + 1;
				}

				ui->AddCombo(plotType, plotTypeNames);
				//ui->AddSpacing();
				*/

				ui->EndWindowSpecial();
			}

			//--

			// Settings

			//if (!ui->bMinimize)
			{

				//if(bGui_Settings) IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_MEDIUM;

				if (ui->BeginWindowSpecial(bGui_Settings))
				{
					//if (!bGui_Main)
					{
						ui->Add(ui->bMinimize, OFX_IM_TOGGLE_ROUNDED);
						ui->AddSpacingSeparated();
					}

					ui->Add(bGui_Main, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					ui->AddSpacingSeparated();

					ui->Add(gain, OFX_IM_HSLIDER_MINI);
					ui->AddSpacingSeparated();
					ui->AddSpacing();
					ui->Add(W_bScope, OFX_IM_TOGGLE_SMALL);
					ui->Add(W_bLine, OFX_IM_TOGGLE_SMALL);
					ui->Add(W_bBars, OFX_IM_TOGGLE_SMALL);
					ui->Add(W_bCircle, OFX_IM_TOGGLE_SMALL);
					ui->AddSpacingSeparated();

					ui->Add(W_Spread);
					if (W_bCircle || W_bBars) ui->Add(W_WidthRad);
					if (W_bCircle || W_bBars || W_bLine) ui->Add(W_WidthMin);
					ui->AddSpacing();
					if (W_bBars) ui->Add(W_Rounded);

					if (!ui->bMinimize) {
						ui->AddSpacing();
						if (ui->BeginTree("EXTRA"))
						{
							ui->Add(W_bAbs, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->Add(W_bMirror, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->Add(W_bBottom, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->AddSpacing();
							ui->Add(W_bClamp, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->Add(W_bClampItems, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->AddSpacing();
							ui->Add(W_bHLine, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->AddSpacingSeparated();

							if (ui->BeginTree("ESTHETIC"))
							{
								ui->Add(W_Alpha);
								ui->Add(boxPlotIn.bUseBorder, OFX_IM_TOGGLE_ROUNDED_MINI);
								if (W_bScope) ui->Add(W_LineWidthScope, OFX_IM_STEPPER);
								if (W_bLine) ui->Add(W_LineWidthLines, OFX_IM_STEPPER);
								ui->Add(cPlot, OFX_IM_COLOR_INPUT);
								ui->Add(W_bTransparent, OFX_IM_TOGGLE_ROUNDED_MINI);
								if(!W_bTransparent) ui->Add(cPlotBg, OFX_IM_COLOR_INPUT);
								ui->AddSpacing();
								ui->Add(W_bLabel, OFX_IM_TOGGLE_ROUNDED_MINI);
								if (W_bLabel) ui->Add(cText, OFX_IM_COLOR_INPUT);
								ui->EndTree();
							}

							ui->EndTree();
						}
					}

					ui->EndWindowSpecial();
				}
			}
		}
		//ui->End();
	}

	//void draw()
	//{
	//	//if (!bGui) return;
	//	//if (!bGui_Plots) return;
	//	drawImGui();
	//	//--
	//	if (bGui_Plots) drawPlots();
	//};


	//--------------------------------------------------------------
	void drawPlots()
	{
		if (!bGui_Plots) return;

		//TODO: multiple modes make weird height..
		// must not overwrite y.
		// use absolute vars

		//if (bUseFbo) fbo.draw(300, 300, 320, 240);

		if (!bGui_PlotIn && !bGui_PlotOut) return;

		//ofColor cPlot = ofColor(0, 225);
		//ofColor cPlotBg = ofColor(64, 128);
		//ofColor cText = ofColor(255, 225);

		float g = ofMap(gain, gain.getMin(), gain.getMax(), 0, AMP_GAIN_MAX_POWER, true);

		//--

		ofPushStyle();
		{
			//--

			// In

			if (bGui_PlotIn)
			{
				boxPlotIn.draw();

				int __x = boxPlotIn.getX();
				int __w = boxPlotIn.getWidth();
				int __h = boxPlotIn.getHeight();
				int __y = boxPlotIn.getY() + __h / 2;

				// Bg plot
				if (!W_bTransparent)
				{
					ofSetColor(cPlotBg);
					ofFill();
					ofDrawRectangle(boxPlotIn.getRectangle());
				}

				//--

				//ofSetColor(cPlot);

				// color
				int _a = ofMap(W_Alpha, 0, 1, 0, cPlot.get().a);
				ofColor _c = ofColor(cPlot, _a);
				ofSetColor(_c);

				ofPushMatrix();
				ofTranslate(__x, __y);

				//--

				////TODO:WIP
				//if (bUseFbo)
				//	if (bUpdateFbo)
				//	{
				//		//bUpdateFbo = false;
				//		fbo.begin();
				//		ofClear(0, 0, 0, 0);
				//	}


				//--

				// 0. Scope

				if (W_bScope)
				{
					ofNoFill();
					ofSetLineWidth(W_LineWidthScope);

					//// color
					//int _a = ofMap(W_Alpha, 0, 1, 100, 255);
					//ofColor _c = ofColor(cPlot, _a);
					//ofSetColor(_c);

					float a = __h * g;//amp gain

					for (int i = 0; i < SIZE_BUFFER; ++i)
					{
						float x1 = ofMap(i, 0, SIZE_BUFFER - 1, 0, __w);
						float x2 = ofMap(i + 1, 0, SIZE_BUFFER - 1, 0, __w);

						float y1 = plotIn[i] * a;
						float y2 = plotIn[i + 1] * a;

						//clamp
						float px = 2;
						x1 = ofClamp(x1, px, __w - px);
						x2 = ofClamp(x2, px, __w - px);
						y1 = ofClamp(y1, -__h / 2, __h / 2);
						y2 = ofClamp(y2, -__h / 2, __h / 2);

						ofDrawLine(x1, y1, x1, y2);
					}


					//TODO:
					meshWaveform.draw();
				}

				//--

				// Lines / Bars / Circles types 
				{
					ofNoFill();
					ofSetLineWidth(W_LineWidthLines);

					float a = __h * g;//amp gain

					// amount of desired lines or rectangles
					int amount = (int)ofMap(W_Spread, 0, 1, 200, 6, true);

					int stepi = SIZE_BUFFER / amount;//it step
					float stepw = (__w / (float)amount);//width of each

					int ii = -1;

					for (int i = 0; i < SIZE_BUFFER; i++)
					{
						/*
						if (i > amount) continue;//skip
						ii = i;
						*/

						// will discard some samples
						if (i % stepi == 0)
						{
							ii++;
						}
						else continue;//skip it


						//--

						ofPushMatrix();

						// The raw point to draw
						int x = ii * stepw;
						int y = plotIn[ii] * a;

						// apply min size
						float hMin = 10;
						y = MAX(y, W_WidthMin * hMin);

						float __hf = __h / 2;

						//----

						// fix clamp 
						//if (W_bMirror)
						if (W_bClamp)
						{
							float __g = 2.f;
							if (y < 0) y = (int)ofClamp(y, -__hf * __g, 0);
							else y = (int)ofClamp(y, 0, __hf * __g);
						}

						if (W_bAbs) y = abs(y);

						if (W_bBottom)
						{
							ofTranslate(0, __hf);
						}

						// translate for mirror mode. center to x axis
						if (W_bMirror && !W_bCircle)
						{
							ofTranslate(0, y / 2.f);
						}

						//--

						// Clamp x

						int px = 2;//pad

						//TODO:
						//bool bskip = false;//discard object that could be out of the box
						//if (x<px || x>(__w - 2 * px)) bskip = true;

						//x = (int)ofClamp(x, px, __w - 2 * px);

						//--

						//// clamp y
						//y = (int)ofClamp(y, -__hf, __hf);

						//--

						//// color
						//int _a = ofMap(abs(y), 0, __hf, MIN(70, W_Alpha * 100), MIN(70, W_Alpha * 255));
						//ofColor _c = ofColor(cPlot, _a);
						//ofSetColor(_c);

						// A. Line
						if (W_bLine)
						{
							//x = (int)ofClamp(x, px, __w - px);
							// do or skip if it's outside 
							if (!W_bClampItems || (x > px && x < __w - px))
							{
								ofNoFill();

								int h = y;

								//float hMin = 10;
								//h = MAX(h, W_WidthMin * hMin);

								ofDrawLine(x, 0, x, -h);
							}
						}

						// B. Bars
						if (W_bBars)
						{
							ofFill();

							int gap = 1;
							int wr = ofMap(W_WidthRad, 0, 1.f, 1, stepw - gap, true);
							int xr = x - wr / 2;

							int h = y;

							//float hMin = 10;
							//h = MAX(h, W_WidthMin * hMin);

							//xr = (int)ofClamp(xr, px, __w - wr - px);
							// do or skip if it's outside 
							if (!W_bClampItems || (x > px && x < __w - px))
							{
								float round = ofMap(W_Rounded, 0, 1, 0, (wr / 2));
								if (W_Rounded != 0) ofDrawRectRounded(xr, 0, wr, -h, round);
								else ofDrawRectangle(xr, 0, wr, -h);
							}
						}

						// C. Circle 
						if (W_bCircle)
						{
							float r = ofMap(W_WidthRad / 2.f, 0, 1, 4, y * 1.f, true);
							float rMin = 5;
							r = MAX(r, W_WidthMin * rMin);

							// do or skip if it's outside
							if (!W_bClampItems || (x > r && x < (__w - r)))
							{
								ofFill();

								ofDrawCircle(x, 0, r);
							}
						}

						//if (W_bMirror && !W_bCircle)
						//{
						//}

						ofPopMatrix();
					}
				}

				//--

				// Horizontal line

				if (W_bHLine)
				{
					ofSetLineWidth(1);
					ofNoFill();
					ofDrawLine(0, 0, __w, 0);
				}

				//--

				// Label

				if (W_bLabel)
				{
					string s = "INPUT";

					float hh = boxPlotIn.getRectangle().getHeight();

					int w = ofxSurfingHelpers::getWidthBBtextBoxedMini(myFont, s);
					int h = ofxSurfingHelpers::getHeightBBtextBoxedMini(myFont, s);

					//center right
					//int xx = __w - w - 10;
					//int yy = h / 2;

					//bottom right
					int pad = 12;
					int xx = __w - w - 0 - pad;
					int yy = hh / 2 - pad;

					//int xx = p.x - w;
					//int yy = p.y - h;
					ofxSurfingHelpers::drawTextBoxedMini(myFont, s, xx, yy, cText);
				}

				//--

				//if (bUseFbo)
				//	if (bUpdateFbo)
				//	{
				//		fbo.end();
				//		bUpdateFbo = true;
				//	}

				//--

				ofPopMatrix();
			}

			//----

			// Out

			//TODO: WIP:
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
			if (bGui_PlotOut)
			{
				boxPlotOut.draw();

				int __x = boxPlotOut.getX();
				int __w = boxPlotOut.getWidth();
				int __h = boxPlotOut.getHeight();
				int __y = boxPlotOut.getY() + __h / 2;

				ofPushMatrix();
				ofTranslate(__x, __y);

				ofSetLineWidth(W_LineWidthScope);
				ofNoFill();

				ofDrawLine(0, 0, 1, plotOut[1] * __h * g); // first line

				for (int i = 1; i < (__w - 1); ++i)
				{
					ofDrawLine(
						i, plotOut[i] * __h * g,
						i + 1, plotOut[i + 1] * __h * g);
				}

				//--

				// horizontal line
				if (W_bHLine)
				{
					ofSetLineWidth(1);
					ofNoFill();

					ofDrawLine(0, 0, __w, 0);
				}

				//--

				// label
				if (W_bLabel)
				{
					string s = "OUT";
					int w = ofxSurfingHelpers::getWidthBBtextBoxedMini(myFont, s);
					int h = ofxSurfingHelpers::getHeightBBtextBoxedMini(myFont, s);
					ofxSurfingHelpers::drawTextBoxedMini(myFont, s, __w - w, h / 2);
				}

				ofPopMatrix();
		}
#endif
	}
		ofPopStyle();
}

	//--------------------------------------------------------------
	void doReset()
	{
		W_Spread = 0.5;
		W_WidthRad = 0.5;
		gain = 0;
		W_bScope = false;
		W_bHLine = false;
		W_bLine = true;
		W_bBars = false;
		W_bCircle = false;
		W_bAbs = true;
		W_bMirror = true;
		W_Rounded = 0.5;
		W_bBottom = false;
		W_bLabel = true;

		//plotType = 3;

		W_LineWidthScope = 2;
		W_LineWidthLines = 3;

		cPlot = ofColor(0, 225);
		cPlotBg = ofColor(64, 128);
		cText = ofColor(255, 225);
	}

	//--------------------------------------------------------------
	void Changed_params_PlotsWaveform(ofAbstractParameter& e)
	{
		if (bDISABLE_CALBACKS) return;

		string name = e.getName();

		if (0) {}

		else if (name == W_vReset.getName())
		{
			doReset();
		}

		/*
		else if (name == bGui.getName())
		{
			//workflow
			if (bGui) bGui_Main.set(true);

			return;
		}
		*/

		/*
		else if (name == bGui.getName())
		{
			//workflow
			if (!bGui_Main && !bGui_Settings)
				bGui_Main.setWithoutEventNotifications(true);

			return;
		}
		*/

		else if (name == bGui_Plots.getName() && bGui_Plots)
		{
			//workflow
			if (!bGui_PlotIn && !bGui_PlotOut)
				bGui_PlotIn.setWithoutEventNotifications(true);

			return;
		}


		//else if (name == W_Reset.getName() && W_bReset)
		//{
		//	W_bReset = false;
		//	doReset();
		//}

		//--

		// predefined styles
		/*
		// Plot waveform
		else if (name == plotType.getName())
		{
			switch (plotType)
			{
			case 0: // scope
				W_bLine = false;
				W_bBars = false;
				W_bCircle = false;
				break;

			case 1: // lines
				W_bLine = true;
				W_bBars = false;
				W_bCircle = false;
				break;

			case 2: // bar
				W_bLine = false;
				W_bBars = true;
				W_bCircle = false;
				break;

			case 3: // circle
				W_bLine = false;
				W_bBars = false;
				W_bCircle = true;
				break;
			}

			return;
		}
		*/

		//--

		/*
		else if (name == W_bLine.getName())
		{
			if (W_bLine)
			{
				W_bBars = false;
				W_bCircle = false;
			}
		}

		else if (name == W_bBars.getName())
		{
			if (W_bBars)
			{
				W_bLine = false;
				W_bCircle = false;
			}
		}

		else if (name == W_bCircle.getName())
		{
			if (W_bCircle)
			{
				W_bLine = false;
				W_bBars = false;
			}
		}
		*/
	}
};
