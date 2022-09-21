#pragma once

/*
*
* TODO:
*
	fix bloom window resize
	fix select currently saved after saving and refresh

	fft plot example https://github.com/moebiussurfing/examplesOfxMaxim
	ImGui plot https://github.com/epezent/implot_demos
*/

#define USE_BLOOM
//#define USE_ROUNDED_WAVEFORM

//--

#include "ofMain.h"
#include "ofxSurfingBoxHelpText.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingImGui.h"
#include "surfingPresets.h"
#include "imgui_stdlib.h"

#ifdef USE_BLOOM
#include "ofxBloom.h"
#endif

#ifdef USE_ROUNDED_WAVEFORM
#include "RoundedPlot.h"
#endif

//--

#define SIZE_BUFFER 4096

#define SOUND_DEVICES_DISABLE_OUTPUT
//#define SPLIT_WAVEFORMS

//--

#define AMP_GAIN_MAX_POWER 10 /// for plots drawing

class WaveformPlot
{
#ifdef USE_BLOOM
public:

	ofParameterGroup params_Bloom;
	ofParameter<bool> bEnable;
	ofParameter<float> scale;
	ofParameter<float> thresh;
	ofParameter<float> brightness;
	ofxBloom bloom;
	ofFbo fbo;

	void refreshBloom()
	{
		ofLogNotice("WaveformPlot") << (__FUNCTION__);

		ofDisableArbTex();
		//fbo.allocate(1920, 1080);
		fbo.allocate(boxPlotIn.getWidth(), boxPlotIn.getHeight());
		ofEnableArbTex();

		//bloom.setup(1920, 1080, fbo);
		bloom.setup(boxPlotIn.getWidth(), boxPlotIn.getHeight(), fbo);
	}

	void setupBloom()
	{
		refreshBloom();

		/*
		ofDisableArbTex();
		fbo.allocate(boxPlotIn.getWidth(), boxPlotIn.getHeight());
		ofEnableArbTex();
		*/

		//bloom.setup(boxPlotIn.getWidth(), boxPlotIn.getHeight(), fbo);

		bEnable.set("Bloom", false);
		scale.set("Scale", 0.1f, 0.1f, 16.f);
		thresh.set("Threshold", 0.1f, 0.1f, 2.f);
		brightness.set("Brightness", 2.5f, 0.f, 30.f);

		params_Bloom.setName("Bloom");
		params_Bloom.add(bEnable);
		params_Bloom.add(scale);
		params_Bloom.add(brightness);
		params_Bloom.add(thresh);
	}
#endif

public:

	WaveformPlot()
	{
		ofSetCircleResolution(44);

		// Font label text
		size_TTF = 11;
		name_TTF = "JetBrainsMonoNL-ExtraBold.ttf";
		string pathRoot = "assets/fonts/";
		path_TTF = pathRoot + name_TTF;
		bool bLoaded = myFont.load(path_TTF, size_TTF, true, true);
		path_TTF = pathRoot + "telegrama_render.otf";
		if (!bLoaded) bLoaded = myFont.load(path_TTF, size_TTF, true, true);
		else if (!bLoaded) bLoaded = myFont.load(OF_TTF_MONO, size_TTF, true, true);

#ifdef USE_WAVEFORM_PLOTS
		index.makeReferenceTo(surfingPresets.index);
#endif
	};

	~WaveformPlot()
	{
		ofRemoveListener(params_PlotsWaveform.parameterChangedE(), this, &WaveformPlot::Changed_params_PlotsWaveform);
	};

	//--

#ifdef USE_ROUNDED_WAVEFORM
	RoundedPlot roundedPlot;
#endif

	ofVboMesh meshWaveformA;
	ofPath pathWaveform;
	int countsamples;

public:

	ofxSurfingGui* ui;
	void setUiPtr(ofxSurfingGui* _ui) {
		ui = _ui;
		surfingPresets.setUiPtr(_ui);
	}

	SurfingPresets surfingPresets;

private:

	string pathGlobal = "Waveform/";
	//string pathSettings = "Waveform_Settings";

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

	ofParameter<bool> bGui{ "WAVEFORM PLOT", true };;
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

	ofParameter<bool> W_bScope1{ "Scope 1", false };
	ofParameter<bool> W_bScope2{ "Scope 2", false };
	ofParameter<bool> W_bLine{ "Line", true };
	ofParameter<bool> W_bBars{ "Bars", false };
	ofParameter<bool> W_bCircles{ "Circles", false };
	ofParameter<bool> W_bMesh1{ "Mesh 1", false };

	ofParameter<bool> W_bMesh{ "Mesh", false };
	ofParameter<bool> W_bMeshFill{ "Fill", true };
	ofParameter<bool> W_bMeshStroke{ "Stroke", false };

	ofParameter<float> W_Spread{ "Spread", 0, 0, 1 };
	ofParameter<float> W_RatioWidth{ "RatioWidth", 0.5f, 0, 1 };
	ofParameter<float> W_RatioRad{ "RatioRadius", 0.5f, 0, 1 };

	ofParameter<float> W_WidthMin{ "Min", 0.1f, 0, 1 };
	ofParameter<bool> W_bAbs{ "Abs", true };
	ofParameter<bool> W_bBottom{ "Bottom", false };
	ofParameter<float> W_Alpha{ "Alpha", 0.5, 0, 1 };
	ofParameter<float> W_AlphaCircle{ "AlphaCircle", 0.5, 0, 1 };
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

	ofParameter<ofColor> cPlotBoxBg{ "c Bg", ofColor(64, 128), ofColor(0), ofColor(0) };
	ofParameter<ofColor> cPlotBoxBorder{ "c Border", ofColor(64, 128), ofColor(0), ofColor(0) };
	ofParameter<ofColor> cPlotLineBars{ "c LineBars", ofColor(0, 225), ofColor(0), ofColor(0) };
	ofParameter<ofColor> cPlotFill{ "c Fill", ofColor(64, 128), ofColor(0), ofColor(0) };
	ofParameter<ofColor> cPlotStroke{ "c Stroke", ofColor(64, 128), ofColor(0), ofColor(0) };
	ofParameter<ofColor> cText{ "c Text", ofColor(255, 225), ofColor(0), ofColor(0) };

	string sTextIn = "INPUT";
	string sTextOut = "OUTPUT";

public:

	void setTextIn(string s) { sTextIn = s; }
	void setTextOut(string s) { sTextOut = s; }

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
		ofLogNotice("WaveformPlot") << (__FUNCTION__);
		bDISABLE_CALBACKS = false;

		doReset();

		//surfingPresets.startup();
		//surfingPresets.doLoad();

#ifdef USE_BLOOM
		refreshBloom();
#endif
	}

	void update()
	{
		if (boxPlotIn.isChangedShape())
		{
			//TODO:
			ofLogNotice("WaveformPlot") << (__FUNCTION__) << "Changed Box Input Plot !";

			//initFbo(boxPlotIn.getWidth(), boxPlotIn.getHeight());

#ifdef USE_BLOOM
			refreshBloom();
#endif
		}
	}

	void setup()
	{
		ofLogNotice("WaveformPlot") << (__FUNCTION__);

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
		params_PlotsWaveform.add(W_bScope1);
		params_PlotsWaveform.add(W_bLine);
		params_PlotsWaveform.add(W_bBars);
		params_PlotsWaveform.add(W_bCircles);
		params_PlotsWaveform.add(W_bMesh);
		params_PlotsWaveform.add(W_bMeshFill);
		params_PlotsWaveform.add(W_bMeshStroke);
		params_PlotsWaveform.add(W_bMesh1);
		params_PlotsWaveform.add(W_bScope2);
		params_PlotsWaveform.add(W_Spread);
		params_PlotsWaveform.add(W_bAbs);
		params_PlotsWaveform.add(W_bHLine);
		params_PlotsWaveform.add(W_bClamp);
		params_PlotsWaveform.add(W_bClampItems);
		params_PlotsWaveform.add(W_Rounded);
		params_PlotsWaveform.add(W_bMirror);
		params_PlotsWaveform.add(W_RatioWidth);
		params_PlotsWaveform.add(W_RatioRad);
		params_PlotsWaveform.add(W_WidthMin);
		params_PlotsWaveform.add(W_bBottom);
		params_PlotsWaveform.add(W_bLabel);

		params_PlotsEsthetics.setName("ESTHETICS");
		params_PlotsWaveform.add(boxPlotIn.bUseBorder);
		params_PlotsEsthetics.add(W_bTransparent);
		params_PlotsEsthetics.add(W_Alpha);
		params_PlotsEsthetics.add(W_AlphaCircle);
		params_PlotsEsthetics.add(W_LineWidthScope);
		params_PlotsEsthetics.add(W_LineWidthLines);
		params_PlotsEsthetics.add(cPlotLineBars);
		params_PlotsEsthetics.add(cPlotBoxBg);
		params_PlotsEsthetics.add(cPlotBoxBorder);
		params_PlotsEsthetics.add(cPlotFill);
		params_PlotsEsthetics.add(cPlotStroke);
		params_PlotsEsthetics.add(cText);
		params_PlotsWaveform.add(params_PlotsEsthetics);

#ifdef USE_BLOOM
		setupBloom();
		params_PlotsWaveform.add(params_Bloom);
#endif

#ifdef USE_ROUNDED_WAVEFORM
		params_PlotsWaveform.add(roundedPlot.W_bCircled);
		params_PlotsWaveform.add(roundedPlot.params_Circled);
#endif

		ofAddListener(params_PlotsWaveform.parameterChangedE(), this, &WaveformPlot::Changed_params_PlotsWaveform);

		//--

		//TODO: Presets
		surfingPresets.setPath(pathGlobal);
		surfingPresets.AddGroup(params_PlotsWaveform);

		W_vReset.makeReferenceTo(surfingPresets.vReset);

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

#ifdef USE_ROUNDED_WAVEFORM
		//roundedPlot.setPlotPtr(&plotIn[0]);
#endif
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
				if (!ui->bMinimize) {
					ui->Add(ui->bDebug, OFX_IM_TOGGLE_ROUNDED_MINI);
					if (ui->bDebug) {
						ui->AddLabel("Elements: " + ofToString(countsamples + 1));
					}
				}
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

				//--

				// 2. Presets

				surfingPresets.drawImGui(false);

#ifdef USE_BLOOM
				if (!ui->bMinimize)
				{
					ui->AddSpacingSeparated();

					if (ui->BeginTree("BLOOM"))
					{
						ui->Add(bEnable, OFX_IM_TOGGLE);
						if (bEnable) {
							ui->Add(scale, OFX_IM_HSLIDER_MINI);
							ui->Add(thresh, OFX_IM_HSLIDER_MINI);
							ui->Add(brightness, OFX_IM_HSLIDER_MINI);
						}
						ui->EndTree();
					}
				}
#endif

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

					//--

					ui->AddSpacingSeparated();

					if (ui->BeginTree("ELEMENTS"))
					{
						ui->Add(W_bScope1, OFX_IM_TOGGLE_SMALL, 2, true);
						ui->Add(W_bScope2, OFX_IM_TOGGLE_SMALL, 2);

#ifdef USE_ROUNDED_WAVEFORM
						ui->Add(roundedPlot.W_bCircled, OFX_IM_TOGGLE_SMALL);
						ui->AddSpacingSeparated();

						ui->Add(roundedPlot.W_bCircled, OFX_IM_TOGGLE_SMALL);
						if (roundedPlot.W_bCircled) ui->AddGroup(roundedPlot.params_Circled);
						/*
						if (roundedPlot.W_bCircled) {
							if(ui - BeginTree("Circled")) {
								ui->Add(roundedPlot.gain);
								ui->Add(roundedPlot.W_CircledRadius);
								ui->Add(roundedPlot.W_CircledSize);
								ui->Add(roundedPlot.W_Spread2);
								ui->Add(roundedPlot.W_RatioWidth2);
								ui->Add(roundedPlot.W_RatioRad2);
								ui->Add(roundedPlot.W_Rounded2);
								ui->Add(roundedPlot.W_WidthMin2);
								ui->EndTree();
							}
						}
						*/
#endif
						ui->AddSpacing();

						//if(!W_bCircled){
						ui->Add(W_bLine, OFX_IM_TOGGLE_SMALL, 2, true);
						ui->Add(W_bBars, OFX_IM_TOGGLE_SMALL, 2);
						ui->Add(W_bCircles, OFX_IM_TOGGLE_SMALL);
						ui->AddSpacing();

						ui->Add(W_bMesh1, OFX_IM_TOGGLE_SMALL);

						if (W_bCircles || W_bBars || W_bLine || W_bMesh1)
							ui->AddSpacingSeparated();

						// distribution
						if (W_bCircles || W_bBars || W_bLine || W_bMesh1)
							ui->Add(W_Spread);

						if (W_bBars) ui->Add(W_RatioWidth);
						if (W_bCircles) ui->Add(W_RatioRad);

						if (W_bCircles || W_bBars || W_bLine || W_bMesh1)
							ui->Add(W_WidthMin);

						if (W_bBars) {
							ui->AddSpacing();
							ui->Add(W_Rounded);
						}

						ui->EndTree();
					}

					ui->AddSpacingSeparated();

					//--

					if (!ui->bMinimize)
					{
						if (ui->BeginTree("EXTRA"))
						{
							if (W_bCircles || W_bBars || W_bLine || W_bMesh1) {
								ui->Add(W_bAbs, OFX_IM_TOGGLE_ROUNDED_MINI);
								ui->Add(W_bMirror, OFX_IM_TOGGLE_ROUNDED_MINI);
								ui->Add(W_bBottom, OFX_IM_TOGGLE_ROUNDED_MINI);
								ui->AddSpacing();
							}
							ui->Add(W_bClamp, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->Add(W_bClampItems, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->AddSpacing();

							ui->Add(W_bHLine, OFX_IM_TOGGLE_ROUNDED_MINI);

							ui->EndTree();
						}

						//--

						ui->AddSpacingSeparated();

						if (ui->BeginTree("ESTHETIC"))
						{
							if (ui->BeginTree("BOX"))
							{
								ui->Add(W_bTransparent, OFX_IM_TOGGLE_ROUNDED_MINI);
								if (!W_bTransparent) ui->Add(cPlotBoxBg, OFX_IM_COLOR_INPUT);
								ui->Add(boxPlotIn.bUseBorder, OFX_IM_TOGGLE_ROUNDED_MINI);
								if (boxPlotIn.bUseBorder)ui->Add(cPlotBoxBorder, OFX_IM_COLOR_INPUT);
								ui->EndTree();
							}

							ui->AddSpacingSeparated();

							if (W_bMesh1)
							{
								ui->Add(W_bMeshFill, OFX_IM_TOGGLE_ROUNDED_SMALL);
								if (W_bMeshFill)ui->Add(cPlotFill, OFX_IM_COLOR_INPUT);
								ui->Add(W_bMeshStroke, OFX_IM_TOGGLE_ROUNDED_SMALL);
								if (W_bMeshStroke)ui->Add(cPlotStroke, OFX_IM_COLOR_INPUT);
							}
							if (W_bScope1 || W_bScope2 || W_bLine || W_bBars || W_bCircles)
								ui->Add(cPlotLineBars, OFX_IM_COLOR_INPUT);

							ui->Add(W_Alpha, OFX_IM_HSLIDER_MINI);
							if (W_bCircles) ui->Add(W_AlphaCircle, OFX_IM_HSLIDER_MINI);

							if (W_bScope1 || W_bScope2 || (W_bMesh1 && W_bMeshStroke))
								ui->Add(W_LineWidthScope, OFX_IM_STEPPER);
							if (W_bLine) ui->Add(W_LineWidthLines, OFX_IM_STEPPER);

							ui->AddSpacingSeparated();

							ui->Add(W_bLabel, OFX_IM_TOGGLE_ROUNDED_MINI);
							if (W_bLabel) ui->Add(cText, OFX_IM_COLOR_INPUT);

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
	void drawLabel()
	{
		int wb = boxPlotIn.getWidth();

		string s = sTextIn;

		float hh = boxPlotIn.getRectangle().getHeight();

		int w = ofxSurfingHelpers::getWidthBBtextBoxedMini(myFont, s);
		int h = ofxSurfingHelpers::getHeightBBtextBoxedMini(myFont, s);

		// center right
		//int xx = wb - w - 10;
		//int yy = h / 2;

		// bottom right
		int pad = 12;
		int xx = wb - w - 0 - pad;
		int yy = hh / 2 - pad;

		//int xx = p.x - w;
		//int yy = p.y - h;
		int x = 0;
		int y = 0;

		ofColor colorBackground = ofColor(0, 200);
		bool useShadow = true;
		ofColor colorShadow = 128;

		ofxSurfingHelpers::drawTextBoxedMini(myFont, s, xx, yy, cText, colorBackground, useShadow, colorShadow);
	}

	//--------------------------------------------------------------
	void drawBox()
	{
		// box border and interaction
		//boxPlotIn.draw();

		// Bg plot
		if (!W_bTransparent)
		{
			ofSetColor(cPlotBoxBg);
			ofFill();
			ofDrawRectangle(boxPlotIn.getRectangle());
		}
	}

	//--------------------------------------------------------------
	void drawPlots()
	{
		if (!bGui_Plots) return;
		if (!bGui_PlotIn && !bGui_PlotOut) return;

		// box
		int xb = boxPlotIn.getX();
		int wb = boxPlotIn.getWidth();
		int hb = boxPlotIn.getHeight();
		int yb = boxPlotIn.getY() + hb / 2;

		// gain
		float _gain = ofMap(gain, gain.getMin(), gain.getMax(), 0, AMP_GAIN_MAX_POWER, true);

		//--

#ifdef USE_BLOOM
		fbo.begin();
		if (!W_bTransparent)
		{
			ofColor c = cPlotBoxBg.get();
			ofClear(c.r, c.g, c.b, 255);
		}
		else ofClear(0);
#endif
		//--

		{
			ofPushStyle();
			{
				//--

				// In

				if (bGui_PlotIn)
				{
#ifndef USE_BLOOM
					drawBox();
#endif
					//--

					// Color Plot with alpha
					int _a = ofMap(W_Alpha, 0, 1, 0, cPlotLineBars.get().a);
					ofColor _c = ofColor(cPlotLineBars, _a);
					ofSetColor(_c);

					//--

					ofPushMatrix();
					{
#ifndef USE_BLOOM
						ofTranslate(xb, yb);
#else
						ofTranslate(0, hb / 2);
#endif
						//--

						// Scope

						if (W_bScope1)
						{
							ofNoFill();
							ofSetLineWidth(W_LineWidthScope);

							float a = hb * _gain; // amp gain

							for (int i = 0; i < SIZE_BUFFER - 1; ++i)//?
							{
								float x1 = ofMap(i, 0, SIZE_BUFFER - 1, 0, wb);
								float x2 = ofMap(i + 1, 0, SIZE_BUFFER - 1, 0, wb);

								float y1 = plotIn[i] * a;
								float y2 = plotIn[i + 1] * a;

								// clamp
								if (W_bClamp) {
									float px = 2;
									x1 = ofClamp(x1, px, wb - px);
									x2 = ofClamp(x2, px, wb - px);
									y1 = ofClamp(y1, -hb / 2, hb / 2);
									y2 = ofClamp(y2, -hb / 2, hb / 2);
								}

								ofDrawLine(x1, y1, x1, y2);
							}
						}

						//--

						// Lines / Bars / Circles types 
						{
							//ofNoFill();
							if (W_bLine) ofSetLineWidth(W_LineWidthLines);

							float _amplify = hb * _gain;//amp gain

							// Amount of desired elements 
							// (lines or rectangles)
							//int amount = (int)ofMap(W_Spread, 0, 1, SIZE_BUFFER, 3, true);
							int amount = (int)ofMap(W_Spread, 0, 1, boxPlotIn.getWidth(), 3, true);

							//TODO: remake
							int iStep = SIZE_BUFFER / amount;//it step
							float xStep = wb / (float)amount;//width of each

							float hb_Half = hb / 2;

							int ii = -1;

							//--

							// E Scope 2
							// a plot with positive side only
							if (W_bScope2)
							{
								ofPushMatrix();

								ofTranslate(0, -hb);
								ofTranslate(0, hb_Half);
								//if (W_bBottom) ofTranslate(0, hb_Half);

								//--

								// Color Plot with alpha 
								int _a = ofMap(W_Alpha, 0, 1, 0, cPlotLineBars.get().a);
								ofColor _c = ofColor(cPlotLineBars, _a);
								ofSetColor(_c);

								ofFill();

								ofBeginShape();
								for (int i = 0; i < SIZE_BUFFER; i++)
								{
									float _x = ofMap(i, 0, SIZE_BUFFER, 0, wb);
									if (i == 0) ofVertex(0, hb);

									float _v = ofMap(plotIn[i] * _gain, 0, 2, 0, hb, true);
									//float _v = ofMap(plotIn[i] * _gain, -2, 2, 0, hb, true);

									float _y = 2 * _v;
									if (W_bClamp) _y = ofClamp(_y, 0, hb);

									ofVertex(_x, hb - _y);

									if (i == SIZE_BUFFER - 1) ofVertex(_x, hb);
								}
								ofEndShape(false);

								ofPopMatrix();
							}

							//--

							// Mesh 1 

							// Draw
							{
								if (W_bMesh1)
								{
									if (W_bBottom)
									{
										ofPushMatrix();
										ofTranslate(0, hb_Half);
									}

									if (W_bMeshFill)
									{
										int _a = ofMap(W_Alpha, 0, 1, 0, cPlotFill.get().a);
										ofColor _c = ofColor(cPlotFill, _a);

										pathWaveform.clear();
										pathWaveform.setColor(_c);
										pathWaveform.setFilled(true);

										for (int i = 0; i < meshWaveformA.getNumVertices(); i++)
										{
											pathWaveform.lineTo(meshWaveformA.getVertex(i));
										}

										pathWaveform.draw();
									}

									if (W_bMeshStroke)
									{
										// Color Plot with alpha 
										int _a = ofMap(W_Alpha, 0, 1, 0, cPlotStroke.get().a);
										ofColor _c = ofColor(cPlotStroke, _a);
										ofSetColor(_c);
										ofSetLineWidth(W_LineWidthScope);

										meshWaveformA.draw();
									}

									if (W_bBottom) ofPopMatrix();
								}
							}

							//----

							// Elements

							countsamples = 0;

							// Init mesh
							if (W_bMesh1)
							{
								meshWaveformA.clear();
								meshWaveformA.setMode(OF_PRIMITIVE_LINE_STRIP);
								meshWaveformA.setUsage(GL_DYNAMIC_DRAW);
							}

							//--

							for (int i = 0; i < SIZE_BUFFER; i++)
							{
								/*
								if (i > amount) continue;//skip
								ii = i;
								*/

								//TODO: remake
								//TODO: should know which is the last one to close the mesh loop..
								// will discard some samples
								if (i % iStep == 0)
								{
									ii++;
								}
								else continue; // skip it from here to end (SIZE_BUFFER)!

								//--

								// Color Plot with alpha
								int _a = ofMap(W_Alpha, 0, 1, 0, cPlotLineBars.get().a);
								ofColor _c = ofColor(cPlotLineBars, _a);
								ofSetColor(_c);

								//--
								// 
								//TODO:
								// padding to clamp 
								int px = 0;//pad

								//--

								ofPushMatrix();
								{
									// The raw point to draw
									float x = ii * xStep;
									float y = plotIn[ii] * _amplify;
									float yy; // for use  when mirror

									// Apply min size
									float hMin = 10;
									y = MAX(y, W_WidthMin * hMin);

									//----

									// fix clamp 
									if (W_bClamp)
									{
										float __g;

										// clamp styles
										if (!W_bBottom && W_bMirror) __g = 2.f;
										else if (W_bBottom && !W_bMirror) __g = 2.f;
										else if (!W_bBottom && !W_bMirror) __g = 1.f;
										else if (W_bBottom && W_bMirror) __g = 2.f; // must clamp bottom side!

										float yMax = hb_Half * __g;
										if (y < 0) y = (int)ofClamp(y, -yMax, 0);
										else y = (int)ofClamp(y, 0, yMax);
									}

									if (W_bAbs) y = abs(y);

									if (W_bBottom)
									{
										ofTranslate(0, hb_Half);
									}

									// Translate for mirror mode.
									// center to x axis
									if (W_bMirror)
									{
										ofPushMatrix();
										ofTranslate(0, y / 2.f);
									}

									//--

									// D Mesh 1

									if (W_bMesh1)
									{
										glm::vec3 v(x, -y, 0);

										if (W_bMirror)
										{
											v.y = v.y + y / 2.f;
										}

										//TODO: 
										// should know which / countsamples
										// is the last one to close the mesh loop..
										// when ends, could build t he inverted shape 
										// reading the mesh backwards direction.
										//if (i == 0) meshWaveformA.addVertex(glm::vec3(0, 0, 0));

										if (!W_bClampItems || (x > px && x < wb - px))
										{
											meshWaveformA.addVertex(v);
											countsamples++;
										}
									}

									//--

									// Clamp x

									// A. Lines

									if (W_bLine)
									{
										//x = (int)ofClamp(x, px, wb - px);
										// do or skip if it's outside 
										if (!W_bClampItems || (x > px && x < wb - px))
										{
											ofNoFill();

											ofDrawLine(x, 0, x, -y);
										}
									}

									//--

									// B. Bars

									if (W_bBars)
									{
										ofFill();

										int gap = 0;
										//int gap = 1;

										float wr = ofMap(W_RatioWidth, 0, 1.f, 1, xStep - gap, true);
										float xr = x - (wr / 2.f);

										// do or skip if it's outside 
										if (!W_bClampItems || (x > px && x < wb - px))
										{
											float round = ofMap(W_Rounded, 0, 1, 0, (wr / 2));
											if (W_Rounded != 0) ofDrawRectRounded(xr, 0, wr, -y, round);
											else ofDrawRectangle(xr, 0, wr, -y);
										}
									}

									//--

									// Translate for mirror mode.
									// center to x axis
									if (W_bMirror)
									{
										ofPopMatrix();
									}

									//--

									// C. Circle 

									if (W_bCircles)
									{
										float r = ofMap(W_RatioRad, 0, 1, 4, y * 0.5f, true);
										float rMin = 5;
										r = MAX(r, W_WidthMin * rMin);

										// do or skip if it's outside
										if (!W_bClampItems || (x > r && x < (wb - r)))
										{
											ofFill();

											// Color Plot with alpha circle
											int _a = ofMap(W_AlphaCircle, 0, 1, 0, cPlotLineBars.get().a);
											ofColor _c = ofColor(cPlotLineBars, _a);
											ofSetColor(_c);

											ofDrawCircle(x, 0, r);
										}
									}
								}
								ofPopMatrix();
							}

							//--

							// D Mesh 1

							if (W_bMesh1)
							{
								//--

								// Close inverted loop

								// mesh bottom
								if (W_bMirror)
								{
									int countlast = countsamples;

									// add last from wave to axis
									{
										float _h = meshWaveformA.getVertex(countsamples - 1).y;
										//float _h = 0;
										glm::vec3 v(wb, _h, 0);
										meshWaveformA.addVertex(v);
										countsamples++;
									}

									// add first from axis to wave 
									{
										glm::vec3 v(meshWaveformA.getVertex(countlast));
										meshWaveformA.addVertex(v);
										countsamples++;
									}
									// add symmetric
									{
										glm::vec3 v(meshWaveformA.getVertex(countlast));
										v = glm::vec3(v.x, -v.y, 0);
										meshWaveformA.addVertex(v);
										countsamples++;
									}

									//--

									// B mirrorize bottom side!

									// iterate the all waveform inverted
									for (int i = countlast - 1; i >= 0; i--)
									{
										glm::vec3 v(meshWaveformA.getVertex(i));
										v = glm::vec3(v.x, -v.y, 0);
										meshWaveformA.addVertex(v);
									}

									// add last from wave to axis
									{
										float _h = meshWaveformA.getVertex(0).y;
										glm::vec3 v(0, -_h, 0);
										meshWaveformA.addVertex(v);
									}
									// add symmetric
									{
										float _h = meshWaveformA.getVertex(0).y;
										glm::vec3 v(0, _h, 0);
										meshWaveformA.addVertex(v);
									}
									// add close loop from wave to axis
									{
										glm::vec3 v(meshWaveformA.getVertex(0));
										meshWaveformA.addVertex(v);
									}
								}

								else // close
								{
									// add last from wave to axis
									{
										glm::vec3 v(wb, 0, 0);
										meshWaveformA.addVertex(v);
									}
									// add last from wave to axis
									{
										glm::vec3 v(0, 0, 0);
										meshWaveformA.addVertex(v);
									}
									{
										glm::vec3 v(meshWaveformA.getVertex(0));
										v = glm::vec3(v.x, v.y, 0);
										meshWaveformA.addVertex(v);
									}
								}
							}

						}

						//--

						// Horizontal line

						if (W_bHLine)
						{
							// Color Plot with alpha
							int _a = ofMap(W_Alpha, 0, 1, 0, cPlotLineBars.get().a);
							ofColor _c = ofColor(cPlotLineBars, _a);
							ofSetColor(_c);

							ofSetLineWidth(W_LineWidthScope);
							ofDrawLine(0, 0, wb, 0);
						}

						//--

						//TODO:
						// Circled spectrum
#ifdef USE_ROUNDED_WAVEFORM
						roundedPlot.r = boxPlotIn.getRectangle();
						for (int i = 0; i < SIZE_BUFFER; i++)
						{
							roundedPlot.plotIn[i] = plotIn[i];
						}
						//roundedPlot.plotIn = plotIn;
						roundedPlot.draw();
#endif
						//--

						// Label
#ifndef USE_BLOOM
						if (W_bLabel)
						{
							drawLabel();
			}
#endif
		}
					ofPopMatrix();
	}

				//----

				// Out

				//TODO: WIP:
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
				if (bGui_PlotOut)
				{
					boxPlotOut.draw();

					int xb = boxPlotOut.getX();
					int wb = boxPlotOut.getWidth();
					int hb = boxPlotOut.getHeight();
					int yb = boxPlotOut.getY() + hb / 2;

					ofPushMatrix();
					ofTranslate(xb, yb);

					ofSetLineWidth(W_LineWidthScope);
					ofNoFill();

					ofDrawLine(0, 0, 1, plotOut[1] * hb * _gain); // first line

					for (int i = 1; i < (wb - 1); ++i)
					{
						ofDrawLine(
							i, plotOut[i] * hb * _gain,
							i + 1, plotOut[i + 1] * hb * _gain);
					}

					//--

					// horizontal line
					if (W_bHLine)
					{
						ofSetLineWidth(1);
						ofNoFill();

						ofDrawLine(0, 0, wb, 0);
					}

					//--

					// label
					if (W_bLabel)
					{
						string s = "OUT";
						int w = ofxSurfingHelpers::getWidthBBtextBoxedMini(myFont, s);
						int h = ofxSurfingHelpers::getHeightBBtextBoxedMini(myFont, s);
						ofxSurfingHelpers::drawTextBoxedMini(myFont, s, wb - w, h / 2);
					}

					ofPopMatrix();
}
#endif
	}
			ofPopStyle();

}

#ifdef USE_BLOOM
		ofPushMatrix();
		ofTranslate(xb, yb);
		drawBox();
		ofPopMatrix();
#endif

#ifdef USE_BLOOM
		fbo.end();

		if (bEnable) {
			bloom.setScale(scale);
			bloom.setThreshold(thresh);
			bloom.setBrightness(brightness);
			bloom.process();

			bloom.draw(xb, yb - hb / 2, wb, hb);
			//bloom.draw(xb, yb + hb, wb, -hb);
		}

		else fbo.draw(xb, yb - hb / 2);
#endif

#ifdef USE_BLOOM
		if (W_bLabel)
		{
			ofPushMatrix();
			ofTranslate(xb, yb);
			drawLabel();
			ofPopMatrix();
		}
#endif

		// box border and interaction
		boxPlotIn.draw();
	}

	//--------------------------------------------------------------
	void doReset()
	{
		gain = 0;
		W_Spread = 0.5;
		W_RatioWidth = 0.5;

		W_bScope1 = false;
		W_bScope2 = false;
		W_bMesh1 = false;
		W_bMeshFill = true;
		W_bMeshStroke = false;
		W_bHLine = false;
		W_bLine = true;
		W_bBars = false;
		W_bCircles = false;
		W_bAbs = true;
		W_bMirror = true;
		W_Rounded = 0.5;
		W_bBottom = false;
		W_bLabel = true;

		//plotType = 3;

		W_LineWidthScope = 2;
		W_LineWidthLines = 3;

		cPlotFill = ofColor(0, 225);
		cPlotStroke = ofColor(0, 225);
		cPlotLineBars = ofColor(0, 225);
		cPlotBoxBg = ofColor(64, 128);
		cText = ofColor(255, 225);

#ifdef USE_BLOOM
		scale.set(0.1f);
		thresh.set(0.1f);
		brightness.set(2.5f);
#endif
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
		// Plot meshWaveformA
		else if (name == plotType.getName())
		{
			switch (plotType)
			{
			case 0: // scope
				W_bLine = false;
				W_bBars = false;
				W_bCircles = false;
				break;

			case 1: // lines
				W_bLine = true;
				W_bBars = false;
				W_bCircles = false;
				break;

			case 2: // bar
				W_bLine = false;
				W_bBars = true;
				W_bCircles = false;
				break;

			case 3: // circle
				W_bLine = false;
				W_bBars = false;
				W_bCircles = true;
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
				W_bCircles = false;
			}
		}

		else if (name == W_bBars.getName())
		{
			if (W_bBars)
			{
				W_bLine = false;
				W_bCircles = false;
			}
		}

		else if (name == W_bCircles.getName())
		{
			if (W_bCircles)
			{
				W_bLine = false;
				W_bBars = false;
			}
		}
		*/

		else if (name == cPlotBoxBorder.getName())
		{
			boxPlotIn.setBorderColor(cPlotBoxBorder);
		}
	}
};
