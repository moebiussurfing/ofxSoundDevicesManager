#pragma once

/*

	TODO:

	+	WIP: Enable toggles not working

	+	WIP: INPUT ONLY.
		Out not working as player can't pick which output device to use!
		But probably output can be enabled and easy to fix.
		enable output and test with an example

	+	add thread to build help info without drop frames

	+	WIP: WINDOWS ONLY
	+	add macOS/Linux APIs ? https://github.com/roymacdonald/ofxSoundDeviceManager

	+	BUG: currently using only DS API, now changing API (to ASIO or WASAPI) on runtime crashes!
	+	currently using only inputs. outputs must be tested.
	+	fix API selector. add switch API/device. without exceptions/crashes.
		add disconnect to allow use only input or output. now, enablers are only like mutes.

	+	draggable rectangle to draw the waveforms

	+	store devices by names? just editing xml file bc sorting can change on the system?

	+	integrate / move to with ofSoundObjects ?
		maybe a too much dependencies to only use the input in most situations.
	+	change all to ofSoundBuffer, not buffer, channels..etc
	+	add sample-rate and other settings to gui selectors.
			store to XML too.
			Restart must be required maybe
			https://github.com/roymacdonald/ofxSoundDeviceManager

	Sound Utils
	https://github.com/perevalovds/ofxSoundUtils/blob/master/src/ofxSoundUtils.h

*/


#include "ofMain.h"


//-----------------------

// OPTIONAL

// 1.
//#define USE_DEVICE_OUTPUT
//TODO: WIP: not working yet... Do not take effect the device selector..
//TODO: should add directive to exclude input..
// Disables all Output management.
// Input is always included!

// 2.
#define USE_WAVEFORM_PLOTS 
//TODO: Must be duplicated to WaveformPlot.h
// Must edit on both places!
//TODO: split

// 3.
#define USE_SOUND_FILE_PLAYER
// Includes sound player class 
// allows load and playing sound files.

//--

//TODO: WIP:
#define USE_ofBaseApp_Pointer
// It seems that must be enabled!
// enabled: add-on class uses a passed by reference ofBaseApp pointer. 
// disabled: gets ofBaseApp 'locally'. not sure if this can helps on in/out callbacks..

//#define USE_OFXGUI_INTERNAL // ofxGui internal debug or split from ImGui..

//TODO:
//#define DETECTOR_INTERNAL

//-----------------------


#ifdef USE_OFXGUI_INTERNAL 
#include "ofxGui.h"
#endif

#include "ofxSurfingBoxHelpText.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingImGui.h"
#include "surfingTimers.h"
#include "surfingMaths.h"

#ifdef USE_WAVEFORM_PLOTS
#include "WaveformPlot.h"
#endif

#ifdef USE_SOUND_FILE_PLAYER
#include "ofxSurfingSoundPlayer.h"
#endif

#ifdef DETECTOR_INTERNAL
#include "surfingDetector.hpp"
#endif

#include <cmath>

//#define deviceIn_vuGainBoost 2.f

//--

#ifdef USE_ofBaseApp_Pointer
//--------------------------------------------------------------
class ofxSoundDevicesManager
#else
//--------------------------------------------------------------
class ofxSoundDevicesManager : public ofBaseApp
#endif
{

public:

	//TODO: test avoid thread problems..
	ofMutex waveformMutex;

#ifdef DETECTOR_INTERNAL
private:
	SurfingDetector surfingDetector;
#endif

	//--

private:

	// OneSmooth
	// Interpolator Engine
	//const size_t sz = 4;
#define szi 3
	//idea: from less to more smoothed
	double x1[szi] = { 1.00, 0.94, 0.90 };//gain
	double x2[szi] = { 0.6, 0.7, 0.75, };//smooth
	double x3[szi] = { 0.50, 0.55, 0.65 };//offset
	double x4[szi] = { 0.50, 0.6, 1.00 };//smooth2

	// Linear interpolation function
	double lerp(double a, double b, double t) {
		return (1 - t) * a + t * b;
	}

private:
	// FFT
	//TODO: should use a vector instead to vary size!
	//ofParameter<int> nBandsToGet{ "Bands", 1 , 1, 128 };
	//static constexpr size_t nBandsToGet = 32;
	static constexpr size_t nBandsToGet = 128;
	std::array<float, nBandsToGet> fftSmoothed{ {0} };
	std::vector<float> data;

	//--

	DemoRotatingText demo;

	//--

#ifdef USE_SOUND_FILE_PLAYER
private:
	ofxSurfingSoundPlayer player;
#endif

public:
	int indexIn = 0;
	int indexOut = 0;

#ifdef USE_WAVEFORM_PLOTS
	WaveformPlot waveformPlot;
#endif

	//--------------------------------------------------------------
	ofxSoundDevicesManager::ofxSoundDevicesManager()
	{
		// Default audio settings

		// Internal default
		//sampleRate = 44100;
		sampleRate = 48000;

		// Internal default
		bufferSize = 512;
		//bufferSize = 256; 

		// Internal default
		numBuffers = 4;
		//numBuffers = 2;

		//--

		// Channels

		numInputs = 2;

#ifdef USE_DEVICE_OUTPUT
		numOutputs = 2;
#endif
		_apiIndex_oF = -1;

		//--

		ofAddListener(ofEvents().update, this, &ofxSoundDevicesManager::update);
	};

	//--------------------------------------------------------------
	ofxSoundDevicesManager::~ofxSoundDevicesManager()
	{
		exit();

		ofRemoveListener(ofEvents().update, this, &ofxSoundDevicesManager::update);
	};

	//--------------------------------------------------------------
	void keyPressed(int key)
	{
		if (!ui.isKeys()) return;
		//// Disable keyboard 
		//// when typing into any ui widget.
		//if (ui.isOverInputText()) return;
		////if (this->getUiPtr()->isOverInputText()) return;

#ifdef DETECTOR_INTERNAL
		surfingDetector.keyPressed(key);
#endif

#ifdef USE_SOUND_FILE_PLAYER
		player.keyPressed(key);
#endif
	};

	//--------------------------------------------------------------
	void exit()
	{
		ofxSurfingHelpers::CheckFolder(pathGlobal);
		ofxSurfingHelpers::saveGroup(params_Settings, pathGlobal + "/" + pathSettings);

		ofRemoveListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);

#ifdef USE_DEVICE_OUTPUT
		ofRemoveListener(params_Out.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Out);
#endif
		ofRemoveListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);

		close();
	}

public:

	//--------------------------------------------------------------
	void setup(int _samplerate, int _buffersize, int _numbuffers)
	{
		sampleRate = _samplerate;
		bufferSize = _buffersize;
		numBuffers = _numbuffers;

		//TODO: is this save?
#ifdef USE_ofBaseApp_Pointer 
		auto _app = ofGetAppPtr();
		setup(_app);
#else
		setup();
#endif
	}

#ifdef USE_ofBaseApp_Pointer
	//--------------------------------------------------------------
	void setup(ofBaseApp* _app, int _samplerate, int _buffersize)
#else
	//--------------------------------------------------------------
	void setup(int _samplerate, int _buffersize)
#endif
	{
		sampleRate = _samplerate;
		bufferSize = _buffersize;

#ifdef USE_ofBaseApp_Pointer 
		setup(_app);
#else
		setup();
#endif
	}

	//--

#ifdef USE_ofBaseApp_Pointer
	//--------------------------------------------------------------
	void setup(ofBaseApp* _app)
#else
	//--------------------------------------------------------------
	void setup()
#endif
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__);

#ifdef USE_ofBaseApp_Pointer 
		_app_ = _app;
#endif
		//--

		setupParams();

		//--

		// Settings paths
		setPath(pathGlobal);

		// Text box
		boxHelpInfo.bGui.setName("DEVICES INFO");
		boxHelpInfo.setTitle("DEVICES INFO");
		boxHelpInfo.setFontSize(8);
		boxHelpInfo.setFontTitleSize(11);
		boxHelpInfo.setup();

		//----

		//TODO:
		// For MS Windows only.
		ApiNames.clear();
		ApiNames.push_back("MS_WASAPI"); // 0
		ApiNames.push_back("MS_ASIO"); // 1
		ApiNames.push_back("MS_DS"); // 2
		//TODO:
		// add macOS + Linux

		//--

		// Internal Gui

#ifdef USE_OFXGUI_INTERNAL 
		gui.setup("DEVICES");
		gui.add(params_Settings);
		gui.getGroup(params_Settings.getName()).minimizeAll();
		gui.getGroup(params_Settings.getName()).minimize();

		// Advanced
		// user gui
		//position = glm::vec2(25, 25);
		//gui.setPosition(position.x, position.y);
#endif
		//--

		setupGui();

		//--

		// API
		// Default settings

		//TODO:
		// Hardcoded
		apiIndex_Windows = 2; // For the gui? 0-1-2 // WASAPI, ASIO, DS
		////TODO:
		//_apiIndex_oF = 9; // API #9 on oF is Windows MS DS

		connectToSoundAPI(_apiIndex_oF); // MS_DS
		//connectToSoundAPI(_app, _apiIndex_oF); // MS_DS

#ifdef USE_WAVEFORM_PLOTS
		waveformPlot.setup();
#endif
		historyVU.assign(MAX_HISTORY_VU, 0.0);

		//--

		//TODO:
		setupSmoothAvg(historySmoothAvgMax);

		//--

		//TODO:
		//fails
#ifdef DETECTOR_INTERNAL
		surfingDetector.setAudioDevicesPtr(this);
		//surfingDetector.setAudioDevicesPtr(&audioDevices);
		//surfingDetector.setAudioDevicesPtr((ofxSoundDevicesManager * )this);
		//surfingDetector.setAudioDevicesPtr((ofxSoundDevicesManager *) (this));
#endif

		//--

		bDISABLE_CALBACKS = false;

		//----

		// Comment to avoid loading settings
		// helpful for when the app is stuck (when starting maybe)
		// for ASIO / WASAPI or some other problem.

		startup();
	}

	//----

private:

	//--------------------------------------------------------------
	void setupParams()
	{
		bEnableAudio.set("ENABLE", true);

		bGui.set("SOUND DEVICES", true);
		bGui_Main.set("SOUND DEVICES", true);

		bDebugExtra.set("Extra", false);

		//--

		// Gui Toggles
		params_Gui.setName("GuiToggles");
		params_Gui.add(bGui);
		params_Gui.add(bGui_Main);
		params_Gui.add(bGui_In);

#ifdef USE_DEVICE_OUTPUT
		params_Gui.add(bGui_Out);
#endif

#ifdef USE_WAVEFORM_PLOTS
		params_Gui.add(waveformPlot.bGui_Plots);
		params_Gui.add(waveformPlot.bGui_Main);
#endif
		params_Gui.add(bGui_Internal);
		params_Gui.add(boxHelpInfo.bGui);
		params_Gui.add(bDebugExtra);

		bGui_Internal.setSerializable(false);

		// Control
		params_Control.setName("CONTROL");
		params_Control.add(bEnableAudio);
		params_Control.add(apiIndex_Windows);
		params_Control.add(params_Gui);

		// Extra
		params_Control.add(bGui_VuPlot);
		params_Control.add(bHorizontal);
		params_Control.add(VUPadding);
		params_Control.add(VUDivisions);
		params_Control.add(bGui_Vu);
		params_Control.add(bGui_FFT);
		params_Control.add(bGui_BigVSlider);

		// Detector
		params_Control.add(threshold);
		params_Control.add(tGateDur);

		params_Control.add(ui.bMinimize);//to refresh help info

		//-

		// In
		deviceIn_Enable.set("ENABLE IN", false);
		deviceIn_Port.set("Port", 0, 0, 10);
		// that param is the loaded from settings. not the name. should be the same

		//deviceIn_Gain.set("Gain", 0.f, -1.f, 1.f);
		deviceIn_Gain.set("Gain", 0.5f, 0.f, 1.f);

		deviceIn_Api.set("Api", 0, 0, 10);
		deviceIn_ApiName.set("Api ", "");
		deviceIn_PortName.set("Port ", "");

		// Out
#ifdef USE_DEVICE_OUTPUT
		deviceOut_Enable.set("ENABLE OUT", false);
		deviceOut_Port.set("Port", 0, 0, 10);
		//that param is the loaded from settings. not the name. should be the same
		deviceOut_Volume.set("Volume", 0.5f, 0.f, 1.f);
		deviceOut_Api.set("Api", 0, 0, 10);
		deviceOut_ApiName.set("Api ", "");
		deviceOut_PortName.set("Port ", "");
#endif

		// Exclude
		//not been used. stored on settings for future reading to be protected under new connected devices, or changes of the listing.

		// In
		deviceIn_Api.setSerializable(false);
		deviceIn_ApiName.setSerializable(false);
		deviceIn_PortName.setSerializable(false);

		// Out
#ifdef USE_DEVICE_OUTPUT
		deviceOut_Api.setSerializable(false);
		deviceOut_ApiName.setSerializable(false);
		deviceOut_PortName.setSerializable(false);
#endif

		//-

		// Input
		params_In.setName("INPUT");
		params_In.add(deviceIn_Enable);
		params_In.add(deviceIn_Port);
		params_In.add(deviceIn_PortName);
		params_In.add(deviceIn_Gain);
		params_In.add(deviceIn_vuSmooth1);
		params_In.add(deviceIn_vuOffsetPow);
		// smooth2
		params_In.add(deviceIn_vuGainBoost);
		params_In.add(deviceIn_PowerSmoothAvg);
		params_In.add(deviceIn_bEnable_SmoothAvg);
		params_In.add(deviceIn_vuAwengine);
		params_In.add(deviceIn_timeAwePatience);
		//params_In.add(deviceIn_timeAwengine);
		params_In.add(bDebug_Awengine);

		params_In.add(deviceIn_OneSmooth);
		params_In.add(plotScale);

		//params_In.add(deviceIn_Api);
		//params_In.add(deviceIn_ApiName);//labels

		// Output
#ifdef USE_DEVICE_OUTPUT
		params_Out.setName("OUTPUT");
		params_Out.add(deviceOut_Enable);
		//params_Out.add(deviceOut_Volume);
		params_Out.add(deviceOut_Port);
		params_Out.add(deviceOut_PortName);
		//params_Out.add(deviceOut_ApiName);//labels
		//params_Out.add(deviceOut_Api);
#endif

		//----

		// For the settings file
		params_Settings.setName("ofxSoundDevicesManager");
		params_Settings.add(params_Control);
		params_Settings.add(params_In);

#ifdef USE_WAVEFORM_PLOTS
		params_Settings.add(waveformPlot.bGui);
		params_Settings.add(waveformPlot.index);
#endif

#ifdef USE_DEVICE_OUTPUT
		params_Settings.add(params_Out);
#endif
		//--

		ofAddListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);

		ofAddListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);

#ifdef USE_DEVICE_OUTPUT
		ofAddListener(params_Out.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Out);
#endif

	}

	//--

#ifdef USE_ofBaseApp_Pointer 
private:
	ofBaseApp* _app_;
#endif

	//--

private:
public:

	// Sound Devices

	ofSoundStream inStream;

#ifdef USE_DEVICE_OUTPUT
	ofSoundStream outStream;
#endif

public:

	auto getSoundStream() {
		return inStream;
	}

	//--

	// Useful exposed vars to be used externally 
	// as sensing rms and a threshold overpassed,
	// to trig a bang

	float getVumeterValue() const {
		return deviceIn_vuValue.get();
	}
	float getThreshold() const {
		return threshold.get();
	}
	bool getIsBangState() const {
		return bBang.get();
	}
	bool getIsAwengineEnabled() const {
		return deviceIn_vuAwengine.get();
	}
	float getGateProgress() const {
		return remainGatePrc;
	}

	// Get delta/flag bang.
	// not the state but a bool true just int the moment going to true.
	// kind of simple callback.
	bool getIsBangDelta()
	{
		static bool bBangPRE = !bBang.get();
		if (bBang.get() != bBangPRE) // changed
		{
			bBangPRE = bBang.get();
			if (bBang.get()) {
				return true;
			}
		}
		return false;
	}

	//--

public:

	int numInputs;

#ifdef USE_DEVICE_OUTPUT
	int numOutputs;
#endif

	int sampleRate;
	int bufferSize;

	int numBuffers;
	//nBuffers - Is the number of buffers that your system will create and swap out. The more buffers, the faster your computer will write information into the buffer, but the more memory it will take up. You should probably use two for each channel that you’re using. Here’s an example call:
	//ofSoundStreamSetup(2, 0, 44100, 256, 4);
	//https://openframeworks.cc/documentation/sound/ofSoundStream/

	//--

private:

	// API

	int _apiIndex_oF;
	// all oF possible apis!
	// NOT the indexIn of the available apis on this system.!
	// WASAPI, ASIO, DS
	// e.g: on this windows system could be: WASAPI, ASIO, DS -> will be 0 to 2
	ofParameter<int> apiIndex_Windows{ "API", 0, 0, 2 };
	// shared by both input and output
	// this indexIn will be related to all available apis ONLY on this system!
	// also in your same system, devices will change when disabling on windows sound preferences/devices

	//TODO:
	// add macOS + Linux

	std::vector<string> ApiNames;
	////to speed up help info build
	//std::vector<string> ApiNames_ASIO;
	//std::vector<string> ApiNames_WASAPI;
	//std::vector<string> ApiNames_DS;

	// Devices

	std::vector<ofSoundDevice> inDevices;
	std::vector<string> inDevicesNames;

#ifdef USE_DEVICE_OUTPUT
	std::vector<ofSoundDevice> outDevices;
	std::vector<string> outDevicesNames;
#endif

	// Settings

	ofSoundStreamSettings inSettings;

#ifdef USE_DEVICE_OUTPUT
	ofSoundStreamSettings outSettings;
#endif

	//-

public:

	ofParameter<bool> bGui;
	ofParameter<bool> bGui_Main;
	ofParameter<bool> bDebugExtra;

	//-

private:

	//glm::vec2 position;

	// Parameters

	ofParameter<bool> bEnableAudio;

	// In

	ofParameterGroup params_In;
	ofParameter<bool> deviceIn_Enable;
	ofParameter<float> deviceIn_Gain;
	ofParameter<int> deviceIn_Port;
	ofParameter<string> deviceIn_PortName;
	ofParameter<int> deviceIn_Api;
	ofParameter<string> deviceIn_ApiName;

	ofParameter<float> deviceIn_vuGainBoost{ "Boost", 3, 0, 6 };

	ofParameter<float> deviceIn_OneSmooth{ "Smooth", 0, 0, 1 }; // main control
	ofParameter<float> deviceIn_OneGain{ "OneGain", 0, 0, 1 }; // main control

	// rms signal to use on VU
	ofParameter<float> deviceIn_vuOffsetPow{ "Offset", 0, -1, 1 }; // Offset pow
	ofParameter<float> deviceIn_vuSmooth1{ "Smooth1", 0, 0, 1 }; // to smooth the vu signal
	ofParameter<float> deviceIn_vuValue{ "RMS", 0, 0, 1 }; // to use into the vu
	ofParameter<float> plotScale{ "Scale", 0.5, 0, 1 };

	ofParameter<bool> deviceIn_vuAwengine{ "AWENGINE", false };
	ofParameter<bool> bDebug_Awengine{ "Debug", false };
	ofParameter<float> deviceIn_timeAwePatience{ "PATIENCE", 0, 0, 1 };
	int deviceIn_timeAwengine;
	//ofParameter<int> deviceIn_timeAwengine{ "PATIENCE", 4, 1, 10 };

	float deviceIn_VuMax = 0;
	uint32_t timeLastAwengine = 0;

	float deviceIn_GainLog;//TODO: maybe useful to be public to apply gain on the parent class.

	// Detector
	// threshold bang
	uint64_t timeLastBang = 0;
	float thresholdLastBang;
	ofParameter<int> tGateDur{ "GATE", 1000, 250, 1750 };
	bool bGateClosed = false;
	float remainGatePrc = 0;

public:

	ofParameter<bool> bBang{ "BANG", false };
	ofParameter<float> threshold{ "Threshold", 1, 0, 1 };

private:

	const int MAX_HISTORY_VU = 1024;
	vector<float> historyVU;
	//float historyVU[1024];

	ofParameter<bool> bGui_Vu{ "VU", false };
	ofParameter<bool> bHorizontal{ "Horizontal", false };
	ofParameter<int> VUPadding{ "Pad", 1, 0, 50 };
	ofParameter<int> VUDivisions{ "Divs", 20, 1, 100 };

	ofParameter<bool> bGui_VuPlot{ "VU HISTORY", false };

	ofParameter<bool> bGui_FFT{ "FFT", false };
	ofParameter<float> fftStart{ "Start", 0, 0, 1 };
	ofParameter<float> fftEnd{ "End", 1, 0, 1 };

	ofParameter<bool> bGui_BigVSlider{ "BIG THRESHOLD", false };

	ofColor* colorGrab = nullptr;
public:

	void setColorSliderGrab(ofColor* color) {
		colorGrab = color;
	}

	//--

	// Out

private:

#ifdef USE_DEVICE_OUTPUT
	ofParameterGroup params_Out;
	ofParameter<bool> deviceOut_Enable;
	ofParameter<float> deviceOut_Volume;
	ofParameter<int> deviceOut_Port;
	ofParameter<string> deviceOut_PortName;
	ofParameter<int> deviceOut_Api;
	ofParameter<string> deviceOut_ApiName;
#endif

	//--

	ofParameterGroup params_Settings;
	ofParameterGroup params_Control;
	ofParameterGroup params_Gui;

	ofParameter<bool> bGui_In{ "INPUT", true };

#ifdef USE_DEVICE_OUTPUT
	ofParameter<bool> bGui_Out{ "OUTPUT", false };
#endif

	ofParameter<bool> bGui_Internal{ "Internal", false };

	bool bDISABLE_CALBACKS = false;//to avoid callback crashes or to enable only after setup()

#ifdef USE_OFXGUI_INTERNAL 
	ofxPanel gui;
#endif

	//--

private:

	ofxSurfingGui ui;

	ofxSurfingBoxHelpText boxHelpInfo;

	string helpInfo = "";
	string devices_ApiName;

	//string nameLabel = "SoundDevices";
	string nameLabel = "ofxSoundDevicesManager";

	string pathGlobal = "ofxSoundDevicesManager";
	string pathSettings = "ofxSoundDevicesManager_Settings.json";

	bool bApiConnected = false;

	//--

public:

	// API

	//--------------------------------------------------------------
	bool getIsVisibleGui() const
	{
		return bGui.get();
	}

	//--------------------------------------------------------------
	void setVisible(bool b)
	{
		bGui = b;
		if (bGui_Main != bGui) bGui_Main = bGui;
	}

	//--------------------------------------------------------------
	void setVisibleToggle()
	{
		bGui = !bGui;
		if (bGui_Main != bGui) bGui_Main = bGui;
	}

	//--

private:

	int historySmoothAvgMax = 15;//that's the slower value. hardcoded!
	float valueSmoothedAvg = 0.0;
	ofParameter<bool> deviceIn_bEnable_SmoothAvg{ "AVG", false };
	ofParameter<float> deviceIn_PowerSmoothAvg{ "Smooth2", 0.5f, 0, 1 };
	vector<float> historySmoothAvg;
	int ioffset = 0;
	int ioffsetEnd = historySmoothAvgMax;
	void setupSmoothAvg(int sz = 60)
	{
		historySmoothAvgMax = sz;

		historySmoothAvg.clear();
		historySmoothAvg.assign(sz, 0.0f);
	};
	void setSmoothAvgPow(float prc)
	{
		int sz = MAX(1, prc * historySmoothAvgMax);
		ioffsetEnd = sz;
	};
	void updateSmoothAvg()
	{
		int sz = ioffsetEnd;

		valueSmoothedAvg = 0;
		for (int i = 0; i < sz; i++)
		{
			valueSmoothedAvg += historySmoothAvg[i];
		}
		valueSmoothedAvg /= sz;

		ioffset++;
		ioffset = ioffset % sz;
	};
	void addValueSmoothAvg(float value)
	{
		historySmoothAvg[ioffset] = value;
	};
	const float getValueSmoothAvg()
	{
		return valueSmoothedAvg;
	};

	//--

	//--------------------------------------------------------------
	void update(ofEventArgs& args)
	{
#ifdef USE_WAVEFORM_PLOTS
		waveformPlot.update();
#endif

		//--

#ifdef USE_SOUND_FILE_PLAYER
		player.update();
#endif

		//--

		if (bUpdateHelp)
		{
			bUpdateHelp = false;

			buildHelpInfo();
		}

		//--

		// Bang
		uint64_t tElapsed = 0;
		if (!bGateClosed)//if gate open
		{
			//if (getVumeterValue() > getThreshold())
			if (deviceIn_vuValue.get() > threshold.get())
			{
				bBang = true;
				bGateClosed = true;
				timeLastBang = ofGetElapsedTimeMillis();
				thresholdLastBang = threshold.get();
			}
		}
		else
		{
			uint64_t t = ofGetElapsedTimeMillis();
			tElapsed = (t - timeLastBang);

			remainGatePrc = ofMap(tElapsed, 0, tGateDur, 1.0f, 0.f, true);
			//gate being closed

			//release gate
			if (tElapsed > tGateDur)
			{
				bGateClosed = false;//open gate
				bBang = false;//release boolean bang
			}
		}

		if (!bGateClosed) {
			remainGatePrc = 0;
		}

		//--

		// Peak memo
		// refresh max every deviceIn_timeAwengine seconds
		//TODO: could improve by using timeLastBang somehow
		// bc threshold switch could force that a new bang happens!

		if (deviceIn_vuValue > deviceIn_VuMax) deviceIn_VuMax = deviceIn_vuValue;

		int tf = MAX(1, int(deviceIn_timeAwengine * 60));
		if (ofGetFrameNum() % tf == 0)
		{
			// AWENGINE!

			//TODO:
			// should use the last threshold or time for last bang ?
			// to improve results
			//timeLastBang
			//thresholdLastBang

			if (deviceIn_vuAwengine)
			{
				doRunAwengine();
			}

			// reset
			deviceIn_VuMax = 0;
		}
	}

private:

	//--------------------------------------------------------------
	void connectToSoundAPI(int _apiIndex)
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__);

		//TODO:
		//if (!bEnableAudio) return;

		close();

		//TODO:
		// API #9 on oF is Windows MS DS
		if (_apiIndex_oF != _apiIndex) _apiIndex_oF = _apiIndex;

		//TODO:
		// both locked to the same
		deviceIn_Api = _apiIndex_oF;

#ifdef USE_DEVICE_OUTPUT
		deviceOut_Api = _apiIndex_oF;
#endif

		//TODO:
		if (_apiIndex_oF != 7 && _apiIndex_oF != 8 && _apiIndex_oF != 9) // WASAPI, ASIO, DS
		{
			ofLogError("ofxSoundDevicesManager") << (__FUNCTION__) << "Skip API index bc out of MS Windows range: " << _apiIndex_oF << endl;

			return;
		}

		//--

		// Clean
		ofSoundStreamSettings _settings;
		inSettings = _settings;
#ifdef USE_DEVICE_OUTPUT
		outSettings = _settings;
#endif

		//----

		// Supported APIs by oF

		enum Api
		{
			UNSPECIFIED = 0,
			DEFAULT,		//TODO: must implement all other APIs: macOS. I don't have Linux...

			ALSA,			/*!< The Advanced Linux Sound Architecture API. */
			PULSE,			/*!< The Linux PulseAudio API. */
			OSS,			/*!< The Linux Open Sound System API. */
			JACK,			/*!< The Jack Low-Latency Audio Server API. */

			OSX_CORE,		/*!< Macintosh OS-X Core Audio API. */

			// MS Windows
			MS_WASAPI,//7	/*!< The Microsoft WASAPI API. */
			MS_ASIO,//8		/*!< The Steinberg Audio Stream I/O API. */
			MS_DS,//9		/*!< The Microsoft Direct Sound API. */

			NUM_APIS

		} _apiEnum = UNSPECIFIED;

		switch (_apiIndex_oF)
		{
		case 7:
			_apiEnum = MS_WASAPI;//7
			break;

		case 8:
			_apiEnum = MS_ASIO;//8
			break;

		case 9:
			_apiEnum = MS_DS;//9
			break;
		}

		//------------------

		// APIs and devices

		inDevices.clear();

#ifdef USE_DEVICE_OUTPUT
		outDevices.clear();
#endif

		switch (_apiEnum)
		{
		case MS_WASAPI:
			devices_ApiName = "MS_WASAPI";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);

#ifdef USE_DEVICE_OUTPUT
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
#endif
			break;

		case MS_ASIO:
			devices_ApiName = "MS_ASIO";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);

#ifdef USE_DEVICE_OUTPUT
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
#endif
			break;

		case MS_DS:
			devices_ApiName = "MS_DS";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_DS);

#ifdef USE_DEVICE_OUTPUT
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_DS);
#endif
			break;
		}

		//--

		// Get devices string names

		// Input
		inDevicesNames.clear();
		inDevicesNames.resize(inDevices.size());
		for (int d = 0; d < inDevices.size(); d++)
		{
			inDevicesNames[d] = inDevices[d].name;
		}

		// Output
#ifdef USE_DEVICE_OUTPUT
		outDevicesNames.clear();
		outDevicesNames.resize(outDevices.size());
		for (int d = 0; d < outDevices.size(); d++)
		{
			outDevicesNames[d] = outDevices[d].name;
		}
#endif

		//--

		// API settings & devices

		// Input

		inSettings.sampleRate = sampleRate;
		inSettings.bufferSize = bufferSize;
		inSettings.numBuffers = numBuffers;
		inSettings.numOutputChannels = 0;
		inSettings.numInputChannels = numInputs;

		switch (_apiEnum)
		{
		case MS_WASAPI:
			inSettings.setApi(ofSoundDevice::Api::MS_WASAPI);
			break;

		case MS_ASIO:
			inSettings.setApi(ofSoundDevice::Api::MS_ASIO);
			break;

		case MS_DS:
			inSettings.setApi(ofSoundDevice::Api::MS_DS);
			break;
		}

#ifdef USE_ofBaseApp_Pointer 
		inSettings.setInListener(_app_);
#else
		inSettings.setInListener(ofGetAppPtr());//?
#endif

		if (inDevices.size() > deviceIn_Port) {
			inSettings.setInDevice(inDevices[deviceIn_Port]);

			inStream.setup(inSettings);
			bApiConnected = true;
		}
		else
		{
			ofLogError("ofxSoundDevicesManager") << (__FUNCTION__) << "Error: Out of range deviceIn_Port: " << deviceIn_Port << endl;
			bApiConnected = false;
		}

		//--

		// Output

#ifdef USE_DEVICE_OUTPUT
		outSettings.bufferSize = bufferSize;
		outSettings.numBuffers = numBuffers;
		outSettings.sampleRate = sampleRate;
		outSettings.numInputChannels = 0;
		outSettings.numOutputChannels = numOutputs;

		switch (_apiEnum)
		{
		case MS_WASAPI:
			outSettings.setApi(ofSoundDevice::Api::MS_WASAPI);
			break;

		case MS_ASIO:
			outSettings.setApi(ofSoundDevice::Api::MS_ASIO);
			break;

		case MS_DS:
			outSettings.setApi(ofSoundDevice::Api::MS_DS);
			break;
		}

#ifdef USE_ofBaseApp_Pointer 
		outSettings.setOutListener(_app_);
#else
		outSettings.setOutListener(ofGetAppPtr());//?
#endif

		if (outDevices.size() > deviceOut_Port)
		{
			outSettings.setOutDevice(outDevices[deviceOut_Port]);

			outStream.setup(outSettings);
			bApiConnected = true;
		}
		else {
			ofLogError("ofxSoundDevicesManager") << (__FUNCTION__) <<
				"Error: Out of range deviceOut_Port: " << deviceOut_Port << endl;
			bApiConnected = false;
		}
#endif

		//--

		// Max ports

		deviceIn_Port.setMax(inDevices.size() - 1);

#ifdef USE_DEVICE_OUTPUT
		deviceOut_Port.setMax(outDevices.size() - 1);
#endif

		// In

		deviceIn_Api = _apiEnum;
		deviceIn_ApiName = devices_ApiName;
		if (inDevices.size() > deviceIn_Port)
			deviceIn_PortName = inDevices[deviceIn_Port].name;

		// Out

#ifdef USE_DEVICE_OUTPUT
		deviceOut_Api = _apiEnum;
		deviceOut_ApiName = devices_ApiName;
		if (outDevices.size() > deviceOut_Port)
			deviceOut_PortName = outDevices[deviceOut_Port].name;
#endif

		//TODO:
		// Force enable

		deviceIn_Enable.setWithoutEventNotifications(bApiConnected);

#ifdef USE_DEVICE_OUTPUT
		deviceOut_Enable.setWithoutEventNotifications(bApiConnected);
#endif

		//--

		bUpdateHelp = true;
	}

	//--

private:

	//--------------------------------------------------------------
	void drawImGui()
	{
		ofScopedLock waveformLock(waveformMutex);

		//if (!bGui) return;

		ui.Begin();
		{
			// Game Mode
			//if (!ui.bGui_GameMode) ui.bGui_GameMode = true;

			drawImGuiGameMode();

			// Big Floating Slider
			if (bGui_BigVSlider)
			{
				//ofColor *colorGrab = nullptr;
				//ofColor colorGrab = ofColor::pink;

				vector<SliderMarks> marks;

				// vu
				SliderMarks m1;
				m1.value = deviceIn_vuValue;
				m1.pad = ImGui::GetStyle().ItemSpacing.x;
				m1.thick = 6;
				m1.color = ofColor(ofColor::black, 96);
				marks.push_back(m1);

				// peak memo
				SliderMarks m2;
				m2.value = deviceIn_VuMax;
				m2.pad = 0;
				m2.thick = 2;
				if (colorGrab != nullptr) m2.color = ofColor(ofColor(*colorGrab), 128);
				else m2.color = ofColor(ofColor::yellow, 200);
				marks.push_back(m2);

				ofxImGuiSurfing::AddSliderBigVerticalFloating(threshold, ImVec2(-1, -1), true, colorGrab, &marks);

				//ofxImGuiSurfing::AddSliderBigVerticalFloating(threshold, ImVec2(-1, -1), true, nullptr, &marks);

				//TODO: do not works. maybe bc windowed?
				//ofxImGuiSurfing::AddMouseWheel(p);
				ofxImGuiSurfing::AddMouseWheel(threshold, false);
				ofxImGuiSurfing::AddMouseClickRightReset(threshold);
			}

			//--

			// Skip!
			if (!ui.bSolo_GameMode)
			{
				// Main

				drawImGuiMain();

				//--

				// Input

				drawImGuiIn();

				//--

				// Out

				drawImGuiOut();

				//--

				// Extras
				drawImGuiExtras();
			}

			//--

			// Wave plot

#ifdef USE_WAVEFORM_PLOTS
			waveformPlot.drawImGui(false);
#endif
			//--

			//TODO:
			// Alternative to box with ImGui native
			//ofxImGuiSurfing::AddTextBoxWindow(boxHelpInfo.bGui.getName(), helpInfo);

			////TODO:
			//static ofRectangle r{ 100,100,100,100 };
			//static ImVec2 pos{ 100,100 };
			//string t1 = boxHelpInfo.bGui.getName();
			//auto t2 = helpInfo.c_str();
			//ofxImGuiSurfing::AddTooltipPinned(t1.c_str(), pos, &r, t2);

			//--

#ifdef USE_SOUND_FILE_PLAYER
			player.drawImGui();
#endif
			//--
		}
		ui.End();

		//--

#ifdef DETECTOR_INTERNAL
		surfingDetector.draw();
#endif
	}

	//--------------------------------------------------------------
	void drawImGuiGameMode()
	{
		if (ui.BeginWindow(ui.bGui_GameMode))
		{
			bool bMax = ui.isMaximized();
			string s;

			ui.AddMinimizerXsToggle(ui.bMinimize);
			//ui.AddTooltip(ui.bMinimize ? "Maximize" : "Minimize");

			ui.AddSpacing();

			ui.PushFont(SurfingFontTypes::OFX_IM_FONT_BIG);
			ui.Add(deviceIn_vuAwengine, OFX_IM_TOGGLE_BIG_XXL_BORDER_BLINK);
			ui.PopFont();
			if (deviceIn_vuAwengine) s = "Threshold is locked, \nand controlled by the Engine.";
			else s = "Threshold is manual, un-locked \nand controlled by the user.";
			ui.AddTooltip(s, bMax);

			ui.AddSpacing();
			ui.AddSpacing();

			// Patience
			{
				SurfingGuiFlags flags = SurfingGuiFlags_None;
				flags += SurfingGuiFlags_NoInput;
				//flags += SurfingGuiFlags_NoTitle;
				//flags += SurfingGuiFlags_TooltipValue;

				const float amt = 2;//size

				ui.Add(deviceIn_vuGainBoost, OFX_IM_KNOB_TICKKNOB, amt, 1 / amt, false, flags);
				ui.AddTooltip(deviceIn_vuGainBoost, true, true);
				ui.SameLine();
				ui.Add(deviceIn_OneSmooth, OFX_IM_KNOB_TICKKNOB, amt, 1 / amt, false, flags);
				ui.AddTooltip(deviceIn_OneSmooth, true, true);
			}

			ui.AddSpacingSeparated();

			if (ui.isMaximized()) ui.AddLabelBig("VU");
			else ui.AddSpacing();
			ui.Add(deviceIn_vuValue, OFX_IM_PROGRESS_BAR_NO_TEXT);
			s = "Main smoothed signal. \nWill push the threshold.";
			ui.AddTooltip(s, bMax);

			if (ui.isMaximized())
			{
				ui.AddSpacing();
				ui.Add(plotScale, OFX_IM_HSLIDER_MINI_NO_NUMBER);
				s = "Re scale Plot.";
				ui.AddTooltip(s, bMax);
			}

			// Plot
			{
				// mark threshold
				vector<SliderMarks> marks;
				SliderMarks m1;
				m1.value = threshold;
				m1.pad = -1;
				m1.thick = 2;
				//m1.color = ofColor(ofColor::yellow, 96);
				if (colorGrab != nullptr && deviceIn_vuAwengine) m1.color = ofColor(*colorGrab);
				else m1.color = ImGui::GetStyleColorVec4(ImGuiCol_SliderGrabActive);
				marks.push_back(m1);

				ofxImGuiSurfing::AddPlot(deviceIn_vuValue, &marks, true, plotScale.get(), bDebug_Awengine.get());
				//ImGui::Spacing();
			}

			ui.AddSpacingSeparated();

			// Threshold

			//// make slider half size and center it
			//ui.AddLabelBig(threshold.getName(), true, true);
			//const float amt = 2;
			//float wk = ui.getWidgetsWidth(amt) / amt;
			////wk += 20;//a bit bigger
			//ofxImGuiSurfing::AddSpacingPad(wk);
			//ui.Add(threshold, OFX_IM_VSLIDER_NO_NAME, 2); 

			if (ui.isMaximized()) ui.AddLabelBig(threshold.getName(), true);

			// change grab slider
			if (colorGrab != nullptr && deviceIn_vuAwengine) {
				ImVec4 cg = ImVec4(*colorGrab);
				auto c1 = ImGui::GetColorU32(cg);
				auto c2 = ImGui::GetColorU32(ImVec4(cg.x, cg.y, cg.z, 0.5f * cg.w));
				ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, c1);
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, c2);
			}

			ui.Add(threshold, OFX_IM_VSLIDER_NO_NAME);

			// change grab slider
			if (colorGrab != nullptr && deviceIn_vuAwengine)
			{
				ImGui::PopStyleColor(2);
			}

			s = "When VU signal passes above, \na Bang will be trigged.";
			ui.AddTooltip(s, bMax);

			ui.AddSpacingSeparated();

			// Gate
			{
				ui.PushFont(SurfingFontTypes::OFX_IM_FONT_BIG);
				ui.Add(bBang, OFX_IM_TOGGLE_BIG_BORDER);
				ui.PopFont();
				s = "Bang deltas will be used \nexternally to trig events.";
				ui.AddTooltip(s, bMax);

				bool b = ui.isMaximized();
				//if (b)
				{
					ui.AddSpacing();
					ui.Add(tGateDur, b ? OFX_IM_HSLIDER_MINI : OFX_IM_HSLIDER_MINI_NO_LABELS);
					if (bMax) {
						s = "After a Bang. \nGate will be closed \nfor some time, \n";
						s += "ignoring incoming Bangs \nuntil the Gate is released.";
						ui.AddTooltip(s);
					}
					else {
						s = "Gate time";
						ui.AddTooltip(s);
					}
				}
				ofxImGuiSurfing::PushMinimalHeights();
				ofxImGuiSurfing::AddProgressBar(remainGatePrc);
				ofxImGuiSurfing::PopMinimalHeights();
				ui.AddSpacing();
			}

			ui.AddSpacingSeparated();

			// Patience
			if (deviceIn_vuAwengine)
			{
				ui.AddSpacing();
				bool b = ui.isMaximized();
				ui.Add(deviceIn_timeAwePatience, b ? OFX_IM_HSLIDER_MINI_NO_NUMBER : OFX_IM_HSLIDER_MINI_NO_LABELS);
				if (bMax) {
					s = "Lower patience will control \nthe threshold faster.\n";
					s += "Slower patience will wait \nmore time before update \nagain the threshold.";
					ui.AddTooltip(s);
				}
				else {
					s = "Patience time";
					ui.AddTooltip(s);
				}

				float v = ofMap(ofGetElapsedTimeMillis() - timeLastAwengine,
					0, deviceIn_timeAwengine * 1000, 0, 1, true);
				ofxImGuiSurfing::PushMinimalHeights();
				ofxImGuiSurfing::AddProgressBar(v);
				ofxImGuiSurfing::PopMinimalHeights();
				ui.AddSpacing();
				if (ui.isMaximized()) {
					ui.Add(bDebug_Awengine, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
					s = "Enables more internal debug: \nnotifies when threshold switches \nby the engine,\n";
					s += "show average plot, ...";
					ui.AddTooltip(s, bMax);
				}

				ui.AddSpacingSeparated();
			}

			{
				//const float amt = 2;
				//float wk = ui.getWidgetsWidth(amt) / amt;
				//ofxImGuiSurfing::AddSpacingPad(wk);
				//ImVec2 sz{ ui.getWidgetsWidth(amt) , ui.getWidgetsHeightUnit() };
				//if (ui.AddButton("RESET ALL", sz))
				//{
				//	doResetAll();
				//}

				if (ui.AddButton("RESET ALL"))
				{
					doResetAll();
				}
			}

			if (ui.isMaximized()) {

				ui.AddSpacingSeparated();

#ifdef USE_SOUND_FILE_PLAYER
				ui.Add(player.bGui, OFX_IM_TOGGLE_ROUNDED);
				ui.AddSpacingSeparated();
#endif

				ui.Add(bGui_BigVSlider, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				s = "Big and floating widget \nfor the Threshold.";
				ui.AddTooltip(s);

				ui.Add(ui.bSolo_GameMode, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
			}

			//--

			ui.EndWindow();
		}
	}

	//--------------------------------------------------------------
	void drawImGuiMain()
	{
		if (bGui_Main)
		{
			//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_SMALL;

			if (ui.BeginWindowSpecial(bGui_Main))
			{
				ui.AddMinimizeToggle();
				if (ui.isMaximized()) {
					ui.AddKeysToggle();
					ui.AddLogToggle();
				}
				ui.AddSpacingSeparated();

				//--

				if (ui.isMinimized()) // minimized
				{
					ui.AddSpacing();

					ui.Add(bGui_In, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					if (!bGui_In)
					{
						ui.AddSpacing();
						ui.Add(deviceIn_vuValue, OFX_IM_PROGRESS_BAR_NO_TEXT);
					}

#ifdef USE_DEVICE_OUTPUT
					ui.AddSpacingBigSeparated();

					ui.Add(bGui_Out, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
#endif

#ifdef USE_WAVEFORM_PLOTS
					ui.AddSpacingSeparated();

					ui.Add(waveformPlot.bGui, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					if (waveformPlot.bGui) {
						ui.Indent();
						ui.Add(waveformPlot.bGui_Main, OFX_IM_TOGGLE_ROUNDED_SMALL);
						ui.Add(waveformPlot.bGui_Edit, OFX_IM_TOGGLE_ROUNDED_SMALL);
						ui.Unindent();
					}

					ui.AddSpacing();
					ui.Add(waveformPlot.bGui_Plots, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					if (waveformPlot.bGui_Plots) {

						//ui.Indent();
						//ui.Add(waveformPlot.bGui_Edit, OFX_IM_TOGGLE_ROUNDED);

						//ui.Add(waveformPlot.gain, OFX_IM_HSLIDER_MINI);
						//ui.Add(waveformPlot.gain, OFX_IM_KNOB_DOTKNOB, 2);

						// Center a single widget
						{
							float w = ui.getWidgetsWidth(2) / 2;
							// Pass the expected widget width divided by two
							AddSpacingPad(w);
							ui.Add(waveformPlot.gain, OFX_IM_KNOB_TICKKNOB, 2);
						}
						//ui.Unindent();
					}
#endif
				}
				else // maximized
				{
					ui.AddLabelBig("API");
					ui.AddSpacing();
					ui.AddCombo(apiIndex_Windows, ApiNames);
					ui.AddSpacing();
					ui.Add(bEnableAudio, OFX_IM_TOGGLE);
					ui.AddSpacingSeparated();

					//--

					// In

					ui.AddSpacing();
					ui.Add(bGui_In, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					ui.AddSpacing();
					if (!bGui_In)
					{
						ui.Add(deviceIn_Enable, OFX_IM_TOGGLE);
						ui.AddSpacing();
						//ui.AddLabelBig("IN");
						ui.AddCombo(deviceIn_Port, inDevicesNames);
					}
					if (!bGui_In)
					{
						ui.AddSpacingSeparated();
						ui.Add(deviceIn_vuValue, OFX_IM_PROGRESS_BAR_NO_TEXT);
					}

					//--

					// Out

#ifdef USE_DEVICE_OUTPUT

					//--

					ui.AddSpacingBigSeparated();
					ui.Add(bGui_Out, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
#endif
					//--

#ifdef USE_WAVEFORM_PLOTS
					ui.AddSpacingSeparated();

					ui.Add(waveformPlot.bGui, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					if (waveformPlot.bGui) {
						ui.Indent();
						ui.Add(waveformPlot.bGui_Main, OFX_IM_TOGGLE_ROUNDED_SMALL);
						ui.Add(waveformPlot.bGui_Edit, OFX_IM_TOGGLE_ROUNDED_SMALL);
						ui.Unindent();
					}

					ui.AddSpacing();
					ui.Add(waveformPlot.bGui_Plots, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					if (waveformPlot.bGui_Plots) {
						//ui.Add(waveformPlot.gain, OFX_IM_KNOB_DOTKNOB, 2);
						//ui.Add(waveformPlot.gain, OFX_IM_HSLIDER_MINI);

						// Center a single widget
						float w = ui.getWidgetsWidth(2) / 2;
						// Pass the expected widget width divided by two

						SurfingGuiFlags flags = SurfingGuiFlags_NoInput;
						flags += SurfingGuiFlags_TooltipValue;

						AddSpacingPad(w);
						ui.Add(waveformPlot.gain, OFX_IM_KNOB_DOTKNOB, 2, flags);
						//ui.Unindent();
					}
#endif
					ui.AddSpacingSeparated();
					ui.Add(boxHelpInfo.bGui, OFX_IM_TOGGLE_ROUNDED_SMALL);
					if (boxHelpInfo.bGui) {
						ui.Indent();
						ui.Add(bDebugExtra, OFX_IM_TOGGLE_ROUNDED_MINI);
						ui.Unindent();
					}
#ifdef USE_OFXGUI_INTERNAL 
					ui.Add(bGui_Internal, OFX_IM_TOGGLE_ROUNDED_MINI);
#endif
				}

				//--

#ifdef USE_SOUND_FILE_PLAYER
				ui.AddSpacingSeparated();
				ui.Add(player.bGui, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
#endif
				ui.AddSpacingSeparated();
				ui.Add(ui.bGui_GameMode, OFX_IM_TOGGLE_ROUNDED_MEDIUM);

				//--

				ui.EndWindowSpecial();
			}
		}
	}

	//--------------------------------------------------------------
	void drawImGuiIn()
	{
		//if (bGui_In) {
		//	ImGui::SetNextWindowSizeConstraints(ImVec2{ 0,0 }, ImVec2{ 175,1080 });
		//}

		if (ui.BeginWindowSpecial(bGui_In))
		{
			if (!ui.bMinimize) {
				ui.AddLabelBig("DEVICE");
				ui.AddSpacing();
			}

			ui.Add(deviceIn_Enable, OFX_IM_TOGGLE);
			ui.AddSpacing();

			ui.AddCombo(deviceIn_Port, inDevicesNames);

			ui.AddSpacingSeparated();

			ui.Add(deviceIn_vuValue, OFX_IM_PROGRESS_BAR_NO_TEXT); // Used out!
			ui.AddTooltip("Real-time VU");

			if (!ui.bMinimize)
			{
				ui.AddSpacingSeparated();

				if (ui.BeginTree("VU SMOOTH"))
				{
					//ui.AddSpacing();
					//ui.AddLabelBig("VU");
					//ui.AddSpacing();

					//const float amt = 3;
					//// NEW API: pass amount of widgets per row and the prc of size
					//// 3 widgets per row with a width of 1/3 of the total available window width
					//ui.Add(deviceIn_Gain, OFX_IM_KNOB_TICKKNOB, amt, 1 / amt);
					//ui.SameLine();
					//ui.Add(deviceIn_vuSmooth1, OFX_IM_KNOB_TICKKNOB, amt, 1 / amt);
					//ui.SameLine();
					//ui.Add(deviceIn_vuOffsetPow, OFX_IM_KNOB_STEPPEDKNOB, amt, 1 / amt);
					////ui.AddTooltip("Offset Gain");
					{
						const float amt = 2;
						// NEW API: pass amount of widgets per row and the prc of size
						// 3 widgets per row with a width of 1/3 of the total available window width
						ui.Add(deviceIn_Gain, OFX_IM_KNOB_TICKKNOB, amt, 1 / amt);
						ui.SameLine();
						ui.Add(deviceIn_vuSmooth1, OFX_IM_KNOB_TICKKNOB, amt, 1 / amt);

						ui.Add(deviceIn_vuGainBoost, OFX_IM_KNOB_DOTKNOB, amt, 1 / amt);
						ui.SameLine();
						ui.Add(deviceIn_vuOffsetPow, OFX_IM_KNOB_DOTKNOB, amt, 1 / amt);
					}

					ui.AddSpacingSeparated();

					//TODO:
					{
						ui.Add(deviceIn_bEnable_SmoothAvg, OFX_IM_TOGGLE);
						{
							if (deviceIn_bEnable_SmoothAvg) {
								float wk = ui.getWidgetsWidth(2) / 2 - ui.getWidgetsSpacingX();
								ofxImGuiSurfing::AddSpacingPad(wk);

								SurfingGuiFlags flags = SurfingGuiFlags_None;
								//flags += SurfingGuiFlags_NoInput;
								//flags += SurfingGuiFlags_TooltipValue;
								ui.Add(deviceIn_PowerSmoothAvg, OFX_IM_KNOB_TICKKNOB, 2, flags);
							}

							//string s;
							//s = "ioffset: " + ofToString(ioffset);
							//ui.AddLabel(s);
							//s = "ioffsetEnd: " + ofToString(ioffsetEnd);
							//ui.AddLabel(s);
						}
					}

					ui.AddSpacingSeparated();

					if (ui.AddButton("RESET"))
					{
						doResetVuSmooth();
					}
					string s = "Note that \nthis Real-time VU value \nis not necessarily correlated \nto RMS/dB exact values.\n";
					s += "Instead of that, is a WIP \nmore usable intuitive value.\nA personal engine \nthat needs more tweaking.";
					ui.AddTooltip(s);

					//ui.AddSpacing();
					ui.AddSpacingSeparated();

					ImGui::Spacing();
					ui.AddExtraToggle(false);
					ImGui::Spacing();
					if (ui.isExtraEnabled())
					{
						ofxImGuiSurfing::AddPad2D(deviceIn_vuSmooth1, deviceIn_Gain, ImVec2{ -1,-1 }, false, true);
						ImGui::Spacing();

						ofxImGuiSurfing::AddPlot(deviceIn_vuValue, ImVec2{ -1,-1 }, nullptr);
					}

					ui.EndTree();
				}
			}

			if (ui.isMaximized())
			{
				ui.AddSpacingSeparated();

				if (ui.BeginTree("DETECTOR"))
				{
					string s;
					ui.Add(threshold, OFX_IM_HSLIDER_MINI);
					s = "Threshold level limit to consider \nthe VU level as \na Bang. Occurred when passed!";
					ui.AddTooltip(s);
					ui.Add(bGui_BigVSlider, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					s = "Big and floating widget \nfor the Threshold.";
					ui.AddTooltip(s);
					ui.Add(tGateDur, OFX_IM_STEPPER);
					s = "Gated time in ms \nafter a Bang trig.";
					ui.AddTooltip(s);
					ui.Add(bBang, OFX_IM_TOGGLE_BIG_XXL_BORDER_BLINK);
					s = "Boolean Bang enabled \nduring gated time.";
					s = "Consider linking a callback \nto be notified \nwhen delta/trig happens.";
					ui.AddTooltip(s);
					///*if (bBang) */ofxImGuiSurfing::AddProgressBar(1 - remainGatePrc);
					ofxImGuiSurfing::PushMinimalHeights();
					ofxImGuiSurfing::AddProgressBar(remainGatePrc);
					ofxImGuiSurfing::PopMinimalHeights();
					ui.EndTree();
				}

				ui.AddSpacingSeparated();

				if (ui.BeginTree("EXTRA WIDGETS"))
				{
					ui.AddSpacing();

					ui.Add(bGui_Vu, OFX_IM_TOGGLE_ROUNDED_SMALL);
					if (bGui_Vu) {
						ui.Indent();
						ui.Add(VUPadding, OFX_IM_STEPPER);
						ui.Add(VUDivisions, OFX_IM_STEPPER);
						ofxImGuiSurfing::AddToggleNamed(bHorizontal, "Horizontal", "Vertical");
						//ui.Add(bHorizontal, OFX_IM_TOGGLE_ROUNDED_MINI);
						ui.Unindent();
					}
					ui.AddSpacingSeparated();

					ui.Add(bGui_VuPlot, OFX_IM_TOGGLE_ROUNDED_SMALL);
					ui.AddSpacingSeparated();

					ui.Add(bGui_FFT, OFX_IM_TOGGLE_ROUNDED_SMALL);
					if (bGui_FFT) {
						ui.Indent();
						ui.Add(fftStart, OFX_IM_HSLIDER_MINI_NO_LABELS);
						ui.Add(fftEnd, OFX_IM_HSLIDER_MINI_NO_LABELS);
						ui.Unindent();
					}

					ui.EndTree();
				}
			}

			//--

			ui.EndWindowSpecial();
		}
	}

	//--------------------------------------------------------------
	void drawImGuiExtras()
	{
		//if (ui.isMaximized()) 
		{
			if (bGui_Vu) ofxImGuiSurfing::AddVU(bGui_Vu.getName(), deviceIn_vuValue, bHorizontal, true, ImVec2(-1, -1), false, VUPadding, VUDivisions);

			if (bGui_VuPlot)
			{
				// mark threshold
				vector<SliderMarks> marks;
				SliderMarks m1;
				m1.value = threshold;
				m1.pad = -1;
				m1.thick = 2;
				m1.color = ofColor(ofColor::yellow, 96);
				marks.push_back(m1);

				ofxImGuiSurfing::AddWaveform(bGui_VuPlot.getName(), &historyVU[0], MAX_HISTORY_VU, true, ImVec2(-1, -1), false, &marks);
			}

			//TODO: FFT bands
			bool bWindowed = true;
			ImVec2 sz = ImVec2(-1, -1);
			bool bNoHeader = false;
			float start = fftStart;
			float end = fftEnd;

			if (bGui_FFT) ofxImGuiSurfing::AddFFT(bGui_FFT.getName(), &data, 1.f, bWindowed, ImVec2(-1, -1), bNoHeader, start, end);
		}
	}

	//--------------------------------------------------------------
	void drawImGuiOut()
	{
#ifdef USE_DEVICE_OUTPUT
		if (bGui_Out) IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_SMALL;
		if (ui.BeginWindowSpecial(bGui_Out))
		{
			ui.Add(deviceOut_Enable, OFX_IM_TOGGLE);
			//ui.Add(deviceOut_Volume, OFX_IM_SLIDER);
			ui.AddCombo(deviceOut_Port, outDevicesNames);

			ui.EndWindowSpecial();
		}
#endif
	}

public:

	//TODO: WIP: trick to expose ui..
	//--------------------------------------------------------------
	ofxSurfingGui* getUiPtr() {
		return &ui;
	};

	//--------------------------------------------------------------
	void drawGui()
	{
		if (bGui)
		{
			drawImGui();

#ifdef USE_OFXGUI_INTERNAL 
			if (bGui_Internal) gui.draw();
#endif

			boxHelpInfo.draw();
		}

#ifdef USE_WAVEFORM_PLOTS
		////if(bGui && bGui_Main) waveformPlot.draw();
		//if (bGui)
		//{
		//	/*
		//	//TODO: not working..
		//	// bc special window has internal set pos
		//	ImGui::SetNextWindowPos(ImVec2(0, 0));
		//	if (waveformPlot.bGui)
		//		if (waveformPlot.bGui_Main || waveformPlot.bGui_Edit)
		//		{
		//			if (bGui_Main) ui.setNextWindowAfterWindowNamed(bGui_Main);
		//		}
		//	*/
		//	//waveformPlot.drawImGui();
		//}

		// Plot
		waveformPlot.drawPlots();
#endif
	}

private:

	//--------------------------------------------------------------
	void close()
	{
		deviceIn_Enable.setWithoutEventNotifications(false);
		inStream.stop();
		inStream.close();

#ifdef USE_DEVICE_OUTPUT
		deviceOut_Enable.setWithoutEventNotifications(false);
		outStream.stop();
		outStream.close();
#endif

		// redundant
		ofSoundStreamStop();
		ofSoundStreamClose();

		bApiConnected = false;
	}

	// Devices
	//--------------------------------------------------------------
	void setDevices(int input, int output)
	{
		deviceIn_Port = input;
#ifdef USE_DEVICE_OUTPUT
		deviceOut_Port = output;
#endif
	}

	//--------------------------------------------------------------
	void setInputDevice(int input)
	{
		deviceIn_Port = input;
	}

#ifdef USE_DEVICE_OUTPUT
	//--------------------------------------------------------------
	void setOutputDevice(int output)
	{
		deviceOut_Port = output;
	}
#endif

	//--

private:

	//--------------------------------------------------------------
	void buildHelpInfo(bool bForce = false)
	{
		if (!boxHelpInfo.bGui && !bForce) return;//avoid refresh devices if hidden

		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__);

		//TODO:
		// getDeviceList is slow. should store each api devices names out of here to speed up.
		// that slow build happens when minimize

		helpInfo = "";

		helpInfo += "\n> AUDIO DEVICE\n";
		helpInfo += "\n  Samplerate : " + ofToString(sampleRate);
		helpInfo += "\n  Buffersize : " + ofToString(bufferSize);
		helpInfo += "\n  Buffers    : " + ofToString(numBuffers);
		helpInfo += "\n  Inputs     : " + ofToString(numInputs);
#ifdef USE_DEVICE_OUTPUT
		helpInfo += "\n  Outputs    : " + ofToString(numOutputs);
#endif
		helpInfo += "\n\n";

		//--

		if (!ui.bMinimize)
		{
			helpInfo += "> INPUT DEVICES FROM ALL APIS\n\n";
		}

		//--

		// List all APIS
		// this is slow..
		// 
		// WASAPI
		if (_apiIndex_oF == 7 || (!ui.bMinimize && bDebugExtra)) // MS_WASAPI
		{
			if (!ui.bMinimize) helpInfo += "  > WASAPI \n\n";
			auto devicesWs = inStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
			//auto devicesWs = ApiNames_WASAPI;
			int d = -1;
			for (auto dev : devicesWs)
			{
				d++;
				if (ui.bMinimize && d != deviceIn_Port) continue;
				bool b = _apiIndex_oF == 7 && deviceIn_Enable && bApiConnected;
				if (d == deviceIn_Port && b) helpInfo += "* "; else helpInfo += "  ";
				helpInfo += ofToString(dev) + "\n";
			}
			if (!ui.bMinimize) helpInfo += "\n";
		}

		// ASIO
		if (_apiIndex_oF == 8 || (!ui.bMinimize && bDebugExtra)) // MS_ASIO
		{
			if (!ui.bMinimize) helpInfo += "  > ASIO \n\n";
			auto devicesAsio = inStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
			//auto devicesAsio = ApiNames_ASIO;
			int d = -1;
			for (auto dev : devicesAsio)
			{
				d++;
				if (ui.bMinimize && d != deviceIn_Port) continue;
				bool b = _apiIndex_oF == 8 && deviceIn_Enable && bApiConnected;
				if (d == deviceIn_Port && b) helpInfo += "* "; else helpInfo += "  ";
				helpInfo += ofToString(dev) + "\n";
			}
			if (!ui.bMinimize) helpInfo += "\n";
		}

		// DS
		if (_apiIndex_oF == 9 || (!ui.bMinimize && bDebugExtra)) // MS_DS
		{
			if (!ui.bMinimize) helpInfo += "  > MS DIRECTSHOW \n\n";
			auto devicesDs = inStream.getDeviceList(ofSoundDevice::Api::MS_DS);
			//auto devicesDs = ApiNames_DS;
			int d = -1;
			for (auto dev : devicesDs)
			{
				d++;
				if (ui.bMinimize && d != deviceIn_Port) continue;
				bool b = _apiIndex_oF == 9 && deviceIn_Enable && bApiConnected;
				if (d == deviceIn_Port && b) helpInfo += "* "; else helpInfo += "  ";
				helpInfo += ofToString(dev) + "\n";
			}
			if (!ui.bMinimize) helpInfo += "\n";
		}

		//--

		if (bApiConnected)
		{
			//if (!ui.bMinimize)
			{
				//helpInfo += "------------------------------------------";
				helpInfo += "\n> CONNECTED \n\n";
			}

			helpInfo += "  API \n";
			helpInfo += "  " + devices_ApiName + "\n\n";

			helpInfo += "  Input  " + ofToString(deviceIn_Port) + "\n";
			helpInfo += "  " + deviceIn_PortName.get() + "\n\n";

#ifdef USE_DEVICE_OUTPUT
			helpInfo += "  Output " + ofToString(deviceOut_Port) + "\n";
			helpInfo += "  " + deviceOut_PortName.get() + "\n\n";
#endif

			//if (!ui.bMinimize)
			//{
			//	//helpInfo += "------------------------------------------\n\n\n";
			//}
		}

		//--

		boxHelpInfo.setText(helpInfo);
	}

public:

	//--------------------------------------------------------------
	void setName(string p) {
		nameLabel = p;
	}

	//--------------------------------------------------------------
	void setPath(string p) {
		pathGlobal = p;

		boxHelpInfo.setPath(pathGlobal + "/" + "HelpBox");

#ifdef USE_WAVEFORM_PLOTS
		waveformPlot.setPath(pathGlobal + "/" + "Waveform");
#endif
	}

	//--------------------------------------------------------------
	string getPathGlobal() const {
		return pathGlobal;
	}

	//--

private:

	//--------------------------------------------------------------
	void startup()
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__);

#ifdef USE_WAVEFORM_PLOTS
		waveformPlot.startup();
#endif
		// Force
		ui.bKeys = false;
		ui.bHelpInternal = false;

		// Load Settings
		ofxSurfingHelpers::loadGroup(params_Settings, pathGlobal + "/" + pathSettings);

		buildHelpInfo(true);

		////TODO:
		// workaround
		//setVisible(true);
		bGui_Main = true;

		//--

#ifdef USE_WAVEFORM_PLOTS
		waveformPlot.index = waveformPlot.index;
#endif
	}

	//--------------------------------------------------------------
	void setupGui()
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__);

		// Game Mode
		ui.bGui_GameMode.setName("AWENGINE");
		ui.bGui_GameMode = true;

		ui.setName(nameLabel);
		ui.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
		ui.setup();

		ui.addWindowSpecial(bGui_Main);
		ui.addWindowSpecial(bGui_In);
#ifdef USE_DEVICE_OUTPUT
		ui.addWindowSpecial(bGui_Out);
#endif
#ifdef USE_WAVEFORM_PLOTS
		ui.addWindowSpecial(waveformPlot.bGui_Main);
		ui.addWindowSpecial(waveformPlot.bGui_Edit);
#endif
#ifdef USE_WAVEFORM_3D_OBJECT
		ui.addWindowSpecial(waveformPlot.o.bGui);
#endif

#ifdef USE_SOUND_FILE_PLAYER
		player.setUiPtr(&ui);
		player.setup();
		ui.addWindowSpecial(player.bGui);
#endif

		// Plot
		//ui.addWindowSpecial(waveformPlot.bGui_Main);

		ui.startup();

		// stack windows vertically/horizontally
		ui.setWindowsSpecialsOrientation(false);

#ifdef USE_WAVEFORM_PLOTS
		waveformPlot.setUiPtr(&ui);
#endif
		//workflow default
		ui.bExtra = false;
	}

	////--------------------------------------------------------------
	//float getVolumeInput()
	//{
	//	if (deviceIn_Enable) return deviceIn_Gain;
	//	else return 0.0f;
	//}

	////--------------------------------------------------------------
	//float getVolumeOutput()
	//{
	//	if (deviceOut_Enable) return deviceOut_Volume;
	//	else return 0.0f;
	//}

	////--------------------------------------------------------------
	//void setPosition(glm::vec2 _position)
	//{
	//	position = _position;
	//}

	////--------------------------------------------------------------
	//void loadGroup(ofParameterGroup& g, string path)
	//{
	//	ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << "loadGroup: " << g.getName() << " to " << path;
	//	ofLogVerbose(__FUNCTION__) << "parameters: " << g.toString();
	//	ofXml settings;
	//	settings.load(path);
	//	ofDeserialize(settings, g);
	//}

	////--------------------------------------------------------------
	//void saveGroup(ofParameterGroup& g, string path)
	//{
	//	ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << "saveGroup: " << g.getName() << " to " << path;
	//	ofLogVerbose(__FUNCTION__) << "parameters: " << g.toString();
	//	ofXml settings;
	//	ofSerialize(settings, g);
	//	settings.save(path);
	//}

	//-

private:

	// Plot waveforms

	bool bUpdateHelp = false;

public:

	//--------------------------------------------------------------
	void audioIn(ofSoundBuffer& input)
	{
		ofScopedLock waveformLock(waveformMutex);

		//TODO:
		if (!bEnableAudio) return;
		if (!deviceIn_Enable) return;

		std::size_t nChannels = input.getNumChannels();

		if (deviceIn_Enable.get())
		{
			// Convert Gain control to log scaled
			float g = ofMap(deviceIn_Gain, 0, 1, 0.3, 0.85, true);
			//float g = ofMap(deviceIn_Gain, 0, 1, 0.1, 0.9, true);
			deviceIn_GainLog = ofxSurfingHelpers::squaredFunction(g);
			////deviceIn_GainLog = ofxSurfingHelpers::reversedExponentialFunction(deviceIn_Gain * 10.f);
			//deviceIn_GainLog = ofMap(deviceIn_Gain, 0, 1, 0.3, 0.85, true);

			//--

			//TODO: FFT bands
			{
				//TODO: notice that do not get signal from input,
				// only from the player.
				// (5) grab the fft, and put in into a "smoothed" array,
				// by taking maximums, as peaks and then smoothing downward
				float* val = ofSoundGetSpectrum(nBandsToGet); // request 128 values for fft
				for (int i = 0; i < nBandsToGet; i++) {

					// let the smoothed value sink to zero:
					fftSmoothed[i] *= 0.96f;

					// take the max, either the smoothed or the incoming:
					if (fftSmoothed[i] < val[i]) fftSmoothed[i] = ofClamp(val[i], 0, 1);
					//if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
				}
				//TODO: pass array to a vector to respect ImPlot API..
				data.clear();
				for (size_t i = 0; i < nBandsToGet; i++)
				{
					data.push_back(fftSmoothed[i]);
				}
			}

			//--

			float _rms = 0.0f;
			//TODO: not used
			int _count = 0;

			////TODO:
			//waveformPlot.makeWaveformMesh(input);
			//waveformPlot.updateWaveformMesh(input);

			//--

			for (size_t i = 0; i < input.getNumFrames(); i++)
			{
#ifdef USE_WAVEFORM_PLOTS

				/*
				waveformPlot.plotIn[indexIn] = input[i * nChannels];
				*/

				////plotIn[indexIn] = input[i * nChannels] * getVolumeInput();

				//--

				{
					///*

					// Smooth
					//TODO: 
					// Could be better inside the plot class..

					float tempInput = input[i * nChannels];
					float tempOutput = waveformPlot.plotIn[indexIn];

					if (waveformPlot.bSmooth)
					{
						// sub gain
						float _gain = ofMap(waveformPlot.smoothGain.get(),
							waveformPlot.smoothGain.getMin(), waveformPlot.smoothGain.getMax(),
							0, AMP_GAIN_MAX_POWER, true);
						tempInput *= _gain;

						/*
							float s1 = ofMap(waveformPlot.smoothVal1.get(), 0, 1, SMOOTH_MIN, SMOOTH_MAX);
							waveformPlot.ofxKuValueSmooth(tempOutput, tempInput, s1);
						*/

						if (waveformPlot.typeSmooth.get() == 1)
						{
							float s1 = ofMap(waveformPlot.smoothVal1.get(), 0, 1, SMOOTH_MIN, SMOOTH_MAX);
							float s2 = ofMap(waveformPlot.smoothVal2.get(), 0, 1, SMOOTH_MIN, SMOOTH_MAX);
							waveformPlot.ofxKuValueSmoothDirected(tempOutput, tempInput, s1, s2);
						}
						else if (waveformPlot.typeSmooth.get() == 0)
						{
							float s1 = ofMap(waveformPlot.smoothVal1.get(), 0, 1, SMOOTH_MIN, SMOOTH_MAX);
							waveformPlot.ofxKuValueSmooth(tempOutput, tempInput, s1);
						}

						waveformPlot.plotIn[indexIn] = tempOutput;
					}
					else
					{
						waveformPlot.plotIn[indexIn] = tempInput;
					}

					//*/
				}

				//--

				if (indexIn < (SIZE_BUFFER - 1))
				{
					++indexIn;
				}
				else
				{
					indexIn = 0;
				}
#endif
				//--

				/*

				// Get the audio signal source here
				{
					// Gain
					// You should get the audio samples here,
					// On in a parent callback...
					auto g = deviceIn_GainLog;
					float left = g * input[0];
					float right =g * input[1];
				}

				*/

				//--

				// Calculate rms

				//--

				//TODO: 
				// Could use ofxSurfingSmooth // ofxStream code snippets

				// code from here: 
				// https://github.com/edap/examplesOfxMaxim
				// rms calculation 
				// as explained here http://openframeworks.cc/ofBook/chapters/sound.html

				float left = input[0];
				float right = input[1];

				//cout << "input: " << left << "," << right << endl;

				// treat as both channels mixed
				_rms += left * left;
				_rms += right * right;

				//--

				// count added samples
				_count += 2; // 2 channels. stereo
				//_count += 1; // 1 channel. mono
			}

			//--

			// Final rms
			_rms = _rms / (float)_count;
			_rms = sqrt(_rms);

			// Clamp
			_rms = ofClamp(_rms, 0, 1);

			// Alternative
			//deviceIn_vuValue *= 0.93;
			//deviceIn_vuValue += 0.07 * _rms;

			//--

			// Extra custom smoothing step

			// prepare the smooth control to fit a more useful range

			// A. Raw
			float smooth = deviceIn_vuSmooth1.get();

			//// B. Expanded
			//smooth = ofxSurfingHelpers::squaredFunction(smooth);
			// 
			// C. Better mapped
			//smooth = ofMap(smooth, 0.f, 1.0f, 0.f, 1.f, true);
			smooth = ofMap(smooth, 0.f, 1.0f, 0.f, .95f, true);

			float _vu = deviceIn_vuValue;

			//_vu *= smooth;
			//_vu += (1 - smooth) * _rms;

			ofxSurfingHelpers::ofxKuValueSmooth(_vu, _rms, smooth);

			//--

			//TODO:
			if (deviceIn_bEnable_SmoothAvg) {
				updateSmoothAvg();
				addValueSmoothAvg(_vu);
				_vu = getValueSmoothAvg();
			}

			//--

			// Apply gain

			// Use the control gain 
			// but converted to log scale.
			// then add an extra gain to the raw vu value.

			float gainExtra;
			if (deviceIn_vuOffsetPow.get() == 0) {
				gainExtra = 1.f;
			}
			else if (deviceIn_vuOffsetPow < 0) {
				float p = -ofxSurfingHelpers::squaredFunction(deviceIn_vuOffsetPow.get());
				gainExtra = ofMap(p, -1.f, 0, 1.f / (float)deviceIn_vuGainBoost, 1.f, true);
			}
			else {
				float p = ofxSurfingHelpers::squaredFunction(deviceIn_vuOffsetPow.get());
				gainExtra = ofMap(p, 0, 1.f, 1.f, (float)deviceIn_vuGainBoost, true);
			}

			_vu = deviceIn_GainLog * _vu * gainExtra;

			//--

			// Clamp
			deviceIn_vuValue = ofClamp(_vu, 0, 1.f);

			//--

			// Plot

			historyVU.push_back(deviceIn_vuValue.get());

			// refresh
			if (historyVU.size() >= MAX_HISTORY_VU) {
				historyVU.erase(historyVU.begin(), historyVU.begin() + 1);
			}

			//--

			//DebugCoutParam(deviceIn_vuValue);
		}

		//----

		// Device in not enabled. 
		// Erase wave plot to zero!
		else
		{
			for (size_t i = 0; i < input.getNumFrames(); ++i)
			{
#ifdef USE_WAVEFORM_PLOTS
				waveformPlot.plotIn[indexIn] = 0;

				if (indexIn < (SIZE_BUFFER - 1))
				{
					++indexIn;
				}
				else
				{
					indexIn = 0;
				}
#endif
			}
		}
	}

#ifdef USE_DEVICE_OUTPUT
	//--------------------------------------------------------------
	void audioOut(ofSoundBuffer& output)
	{
		ofScopedLock waveformLock(waveformMutex);

		//TODO:
		if (!bEnableAudio) return;
		if (!deviceOut_Enable) return;

		std::size_t outChannels = output.getNumChannels();

		if (deviceOut_Enable) // fill plot
		{
			float rms = 0.0;
			int numCounted = 0;

			rms /= (float)numCounted;
			rms = sqrt(rms);

			//smoothedVolume_Out = rms * deviceOut_Volume;
		}
		else // erase plot
		{
			for (size_t i = 0; i < output.getNumFrames(); ++i)
			{
#ifdef USE_WAVEFORM_PLOTS
				plotOut[indexOut] = 0;
				if (indexOut < (ofGetWidth() - 1))
				{
					++indexOut;
				}
				else
				{
					indexOut = 0;
				}
#endif
			}
		}
	}
#endif

private:

	//--------------------------------------------------------------
	void Changed_params_Control(ofAbstractParameter& e)
	{
		if (bDISABLE_CALBACKS) return;

		string name = e.getName();

		//ofLogNotice(__FUNCTION__) << name << " " << e;
		//ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << name << " " << e;
		ofLogNotice("ofxSoundDevicesManager") << name << " " << e;

		if (0) {}

		// MS Windows sound APIs
		else if (name == ui.bMinimize.getName())
		{
			bUpdateHelp = true;

			return;
		}
		else if (name == bDebugExtra.getName())
		{
			bUpdateHelp = true;

			return;
		}

		else if (name == bEnableAudio.getName())
		{
			if (bEnableAudio)
			{
				connectToSoundAPI(_apiIndex_oF);
			}
			else
			{
				close();
			}

			bUpdateHelp = true;

			return;
		}

		//TODO:
		// add macOS + Linux
		else if (name == apiIndex_Windows.getName())
		{
			switch (apiIndex_Windows)
			{
			case 0:
				_apiIndex_oF = 7; // MS_WASAPI
				break;

			case 1:
				_apiIndex_oF = 8; // MS_ASIO
				break;

			case 2:
				_apiIndex_oF = 9; // MS_DS
				break;

			default:
				ofLogError("ofxSoundDevicesManager") << (__FUNCTION__) << "Error: Out of range  _apiIndex_oF: " << _apiIndex_oF << endl;
				bUpdateHelp = true;
				return;//skip
				break;
			}

			ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << "API: " << _apiIndex_oF << ":" << devices_ApiName;
			connectToSoundAPI(_apiIndex_oF);
			bUpdateHelp = true;

			return;
		}
	}

	//--

	//--------------------------------------------------------------
	void Changed_params_In(ofAbstractParameter& e)
	{
		if (bDISABLE_CALBACKS) return;

		string name = e.getName();
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << name << " " << e;

		if (0) {}

		else if (name == deviceIn_timeAwePatience.getName())
		{
			deviceIn_timeAwengine = ofMap(deviceIn_timeAwePatience.get(), 0, 1, 1, 10, true);
		}

		//TODO:
		else if (name == deviceIn_OneSmooth.getName())
		{
			/*

			deviceIn_Gain
			deviceIn_vuSmooth1
			deviceIn_vuOffsetPow

			deviceIn_vuGainBoost
			deviceIn_PowerSmoothAvg

			*/

			// Master knob from 0 to 1
			double control = deviceIn_OneSmooth.get();

			// Interpolate between good values for each of the float variables
			double y1 = lerp(x1[0], x1[szi - 1], control);
			double y2 = lerp(x2[0], x2[szi - 1], control);
			double y3 = lerp(x3[0], x3[szi - 1], control);
			double y4 = lerp(x4[0], x4[szi - 1], control);

			deviceIn_Gain = y1;
			deviceIn_vuSmooth1 = y2;
			deviceIn_vuOffsetPow = y3;
			deviceIn_PowerSmoothAvg = y4;
		}

		else if (name == deviceIn_PowerSmoothAvg.getName())
		{
			setSmoothAvgPow(deviceIn_PowerSmoothAvg.get());
		}

		else if (name == deviceIn_Enable.getName())
		{
			if (deviceIn_Enable)
			{
				connectToSoundAPI(_apiIndex_oF);
				//deviceIn_Port = deviceIn_Port;//refresh
			}
			else
			{
				inStream.close();
				deviceIn_PortName = "";
			}

			bUpdateHelp = true;

			return;
		}

		else if (name == deviceIn_Port.getName())
		{
			inStream.close();
			if (deviceIn_Port >= inDevices.size()) return; //skip

			inSettings.setInDevice(inDevices[deviceIn_Port]);
			inStream.setup(inSettings);

			if (inDevices.size() > deviceIn_Port)
				deviceIn_PortName = inDevices[deviceIn_Port].name;

			bUpdateHelp = true;

			return;
		}
	}

	bool bFlagDoneAwengine = false;

public:

	//--------------------------------------------------------------
	const bool isDoneAwengine() {
		if (bFlagDoneAwengine) {
			bFlagDoneAwengine = false;
			return true;
		}
		else return false;
	}
	//--------------------------------------------------------------
	const bool isDebugAwengine() {
		return bDebug_Awengine.get();
	}

private:

	//--------------------------------------------------------------
	void doRunAwengine()
	{
		// DO THE MAGYC!
		float gapUpper = 1.f;//target will be the same than last max

		//TODO:
		// could have different behaviors presets like
		// slow, medium, aggressive

		//TODO:
		// should use the last threshold or time for last bang ?
		// to improve results
		//timeLastBang
		//thresholdLastBang

		// threshold will be reduced from last catched bang..,
		// check if it "could be too low", then make it x% more bigger.
		//if (deviceIn_VuMax < thresholdLastBang) gapUpper = 1.1f;
		//if (deviceIn_VuMax < thresholdLastBang) gapUpper = 1.07f;

		//// make lower to make easy that trigs bang
		//float gap1 = 0.025f;
		//gapUpper = gapUpper - gap1;

		//TODO:
		// waiting time could be synced with BPM tempo!
		// this will improve sync..

		float thresholdTarget = deviceIn_VuMax * gapUpper;

		// avoid switch if diff is small
		float gapDiff = 0.025f;
		float diff = abs(thresholdTarget - threshold);
		if (diff > gapDiff)
		{
			threshold = thresholdTarget;
			bFlagDoneAwengine = true;
		}
		else
		{
			// bypass threshold switch!
			ofLogVerbose("ofxSoundDevicesManager:doRunAwengine") << "Ignored diff: " << diff;
		}

		timeLastAwengine = ofGetElapsedTimeMillis();
	}

	//--------------------------------------------------------------
	void doResetAll()
	{
		tGateDur = 1000;
		plotScale = 0.f;
		threshold = 1;

		doResetVuSmooth();
	}

	//--------------------------------------------------------------
	void doResetVuSmooth()
	{
		deviceIn_OneSmooth = 0.5;
		deviceIn_timeAwePatience = 0.5;
		//deviceIn_Gain = .84;
		//deviceIn_vuSmooth1 = .84;
		//deviceIn_vuGainBoost = 3;
		//deviceIn_vuOffsetPow = 0.7f;
		//deviceIn_PowerSmoothAvg = 0.5;
		deviceIn_vuGainBoost = (deviceIn_vuGainBoost.getMax() - deviceIn_vuGainBoost.getMin()) / 2;
		//deviceIn_vuGainBoost = 3;

		deviceIn_bEnable_SmoothAvg = true;
	}

	//--

#ifdef USE_DEVICE_OUTPUT
	//--------------------------------------------------------------
	void Changed_params_Out(ofAbstractParameter& e)
	{
		if (bDISABLE_CALBACKS) return;

		string name = e.getName();
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << name << " " << e;

		if (0) {}

		else if (name == deviceOut_Enable.getName())
		{
			if (deviceOut_Enable)
			{
				connectToSoundAPI(_apiIndex_oF);
				deviceOut_Port = deviceOut_Port;//refresh
			}
			else
			{
				outStream.close();
				deviceOut_PortName = "";
			}

			bUpdateHelp = true;

			return;
		}

		//TODO: WIP:
		else if (name == deviceOut_Port.getName())
		{
			outStream.close();
			if (deviceOut_Port >= outDevices.size()) return; //skip

			if (outDevices.size() > deviceOut_Port)
				outSettings.setOutDevice(outDevices[deviceOut_Port]);
			outStream.setup(outSettings);

			if (outDevices.size() > deviceOut_Port)
				deviceOut_PortName = outDevices[deviceOut_Port].name;

			bUpdateHelp = true;

			return;
		}
	}
#endif

	/*
	* //TODO: smooth helpers...
	float getRMSAmplitude(std::vector<float> &buffer, std::size_t channels) const {
		double acc = 0;
		for (size_t i = 0; i < buffer.size(); i++) {
			acc += buffer[i] * buffer[i];
		}
		return sqrt(acc / (double)buffer.size());
	}

	float getRMSAmplitudeChannel(std::vector<float> &buffer, std::size_t channels, std::size_t channel) const {
		if (channel > channels - 1) {
			return 0;
		}

		double acc = 0;
		for (size_t i = 0; i < getNumFrames(); i++) {
			float sample = getSample(i, channel);
			acc += sample * sample;
		}
		return sqrt(acc / (double)getNumFrames());
	}
	*/

};

// NOTES
//https://github.com/firmread/ofxFftExamples/blob/master/example-eq/src/ofApp.cpp#L78
//https://forum.openframeworks.cc/t/question-around-void-audioin-ofsoundbuffer-buffer/22284/2
//https://forum.openframeworks.cc/t/openframeworks-and-speech-synthesis-tts-with-flite/12117/7
//https://forum.openframeworks.cc/t/playing-audio-input-in-real-time-problems-syncing/29180/2?u=moebiussurfing
//http://openframeworks.cc/documentation/sound/ofSoundBuffer/#!show_resampleTo
//https://forum.openframeworks.cc/t/listen-to-ofevent-audioin-in-custom-class/27710/3?u=moebiussurfing