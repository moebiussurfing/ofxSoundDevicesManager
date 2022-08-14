#pragma once

/*

	TODO:

	+	WIP: WINDOWSX ONLY
	+	add macOS/Linux APIs ? https://github.com/roymacdonald/ofxSoundDeviceManager

	+	BUG: currently using only DS API, now changing API (to ASIO or WASAPI) on runtime crashes!
	+	currently using only inputs. outputs must be tested.

	+	draggable rectangle to draw the waveforms
	+	fix api selector. add switch API/device. without exceptions/crashes.
		add disconnect to allow use only input or output. now, enablers are only like mutes.

	+	enable output and test with an example
	+	store devices by names? just editing xml file bc sorting can change on the system?

	+	integrate / move to with ofSoundObjects
	+	change all to ofSoundBuffer, not buffer, channels..etc
	+	add sample-rate and other settings to gui selectors. store to XML too. Restart must be required maybe
	https://github.com/roymacdonald/ofxSoundDeviceManager

	+	alternative waveforms plotting:
	+	alternative plots or vu's
		code from here: https://github.com/edap/examplesOfxMaxim
		rms calculation as explained here http://openframeworks.cc/ofBook/chapters/sound.html
	+	better vu meter using other rms snippets:
	https://github.com/Feliszt/sound-analyzer-OF
	https://github.com/roymacdonald/ofxSoundObjects

*/


#include "ofMain.h"

//-----------------------

//TODO: WIP		
#define USE_ofBaseApp_Pointer
//enabled: addon class uses a passed by reference ofBaseApp pointer. 
//disabled: gets ofBaseApp 'locally'. not sure if this can helps on in/out callbacks..

//-----------------------

#include "ofxGui.h"
#include "ofxSurfingBoxHelpText.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingImGui.h"

#define AMP_GAIN_MAX_POWER 30 /// for plots drawing

#ifdef USE_ofBaseApp_Pointer
//--------------------------------------------------------------
class ofxSoundDevicesManager
#else
//--------------------------------------------------------------
class ofxSoundDevicesManager : public ofBaseApp
#endif

{

public:

	//--------------------------------------------------------------
	ofxSoundDevicesManager()
	{
		// font text
		size_TTF = 11;
		name_TTF = "JetBrainsMonoNL-ExtraBold.ttf";
		string pathRoot = "assets/fonts/";
		path_TTF = pathRoot + name_TTF;
		bool bLoaded = myFont.load(path_TTF, size_TTF, true, true);
		path_TTF = pathRoot + "telegrama_render.otf";
		if (!bLoaded) bLoaded = myFont.load(path_TTF, size_TTF, true, true);
		else if (!bLoaded) bLoaded = myFont.load(OF_TTF_MONO, size_TTF, true, true);

		//--

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
		numOutputs = 2;

		_apiIndex_oF = -1;

		//--

		ofAddListener(ofEvents().update, this, &ofxSoundDevicesManager::update);
	}

	//--------------------------------------------------------------
	~ofxSoundDevicesManager()
	{
		exit();

		ofRemoveListener(ofEvents().update, this, &ofxSoundDevicesManager::update);
	}

	//--------------------------------------------------------------
	void exit()
	{
		saveGroup(params, pathGlobal + pathSettings);

		ofRemoveListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);
		ofRemoveListener(params_Out.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Out);
		ofRemoveListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);
		ofRemoveListener(params_PlotsWaveform.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_PlotsWaveform);

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

		// Text box
		textBoxWidget.bGui.setName("Devices Info");
		textBoxWidget.setTitle("DEVICES INFO");
		textBoxWidget.setFontSize(8);
		textBoxWidget.setFontTitleSize(11);
		textBoxWidget.setup();

		// Plot boxes

		ofColor cBg = ofColor(0, 255);
		//ofColor cPlotBg = ofColor(ofColor::yellow, 200);

		boxPlotIn.setUseBorder(true);
		boxPlotIn.setBorderColor(cBg);
		boxPlotIn.setPosition(400, 10);
		boxPlotIn.bGui.setName("Plot In");
		boxPlotIn.setup();

		boxPlotOut.setUseBorder(true);
		boxPlotOut.setBorderColor(cBg);
		boxPlotOut.setPosition(400, 410);
		boxPlotOut.bGui.setName("Plot Out");
		boxPlotOut.setup();

		//----

		//TODO:
		// For MS Windows only.
		//TODO:
		// add macOS + Linux
		ApiNames.clear();
		ApiNames.push_back("MS_WASAPI"); // 0
		ApiNames.push_back("MS_ASIO"); // 1
		ApiNames.push_back("MS_DS"); // 2

		//-

		// Internal Gui
		gui.setup("DEVICES");
		gui.add(params);
		gui.getGroup(params.getName()).minimizeAll();
		gui.getGroup(params.getName()).minimize();

		// Advanced
		// user gui
		//position = glm::vec2(25, 25);
		//gui.setPosition(position.x, position.y);

		//--

		setupGui();

		//--

		// API
		// Default settings

		// TODO:
		// Hardcoded
		apiIndex_Windows = 2; // For the gui? 0-1-2 // WASAPI, ASIO, DS
		//// TODO:
		//_apiIndex_oF = 9; // API #9 on oF is Windows MS DS

		connectToSoundAPI(_apiIndex_oF); // MS_DS
		//connectToSoundAPI(_app, _apiIndex_oF); // MS_DS

		initFbo();

		//--

		bDISABLE_CALBACKS = false;

		//----

		// Comment to avoid loading settings
		// helpful for when the app is stucked (when starting maybe)
		// for ASIO / WASAPI or some other problem.

		startup();
	}

private:

	//TODO: WIP
	bool bUseFbo = false;
	bool bUpdateFbo = false;

	void initFbo(int width = 320, int height = 240)
	{
		if (!bUseFbo) return;

		//if (bUseFbo && !fbo.isAllocated())
		{
			fbo.allocate(width, height);
			fbo.begin();
			ofClear(0, 0, 0, 0);
			fbo.end();

			bUpdateFbo = true;
		}
	}

	//--------------------------------------------------------------
	void setupParams()
	{
		bEnableAudio.set("ENABLE", true);

		bGui.set("SOUND DEVICES", true);

		bGui_Main.set("SOUND DEVICES", true);
		bGui_Plots.set("Plots", true);

		params_Control.setName("CONTROL");
		params_Control.add(bEnableAudio);
		params_Control.add(apiIndex_Windows);

		params_Gui.setName("Gui");
		params_Gui.add(bGui_Main);
		params_Gui.add(bGui_In);
		params_Gui.add(bGui_Out);
		params_Gui.add(bGui_PlotsPanel);
		params_Gui.add(bGui_Internal);
		params_Gui.add(textBoxWidget.bGui);
		params_Gui.add(guiManager.bMinimize);

		params_Control.add(params_Gui);

		//-

		deviceIn_Enable.set("Enable", false);
		deviceIn_Port.set("Port", 0, 0, 10);
		//that param is the loaded from settings. not the name. should be the same
		deviceIn_Volume.set("Volume", 0.5f, 0.f, 1.f);
		deviceIn_Api.set("Api", 0, 0, 10);
		deviceIn_ApiName.set("Api ", "");
		deviceIn_PortName.set("Port ", "");

		deviceOut_Enable.set("Enable", false);
		deviceOut_Port.set("Port", 0, 0, 10);
		//that param is the loaded from settings. not the name. should be the same
		deviceOut_Volume.set("Volume", 0.5f, 0.f, 1.f);
		deviceOut_Api.set("Api", 0, 0, 10);
		deviceOut_ApiName.set("Api ", "");
		deviceOut_PortName.set("Port ", "");

		// Exclude
		//not been used. stored on settings for future reading to be protected under new connected devices, or changes of the listing.
		// In
		deviceIn_Api.setSerializable(false);
		deviceIn_ApiName.setSerializable(false);
		deviceIn_PortName.setSerializable(false);
		// Out
		deviceOut_Api.setSerializable(false);
		deviceOut_ApiName.setSerializable(false);
		deviceOut_PortName.setSerializable(false);

		//-

		// Input
		params_In.setName("INPUT");
		params_In.add(deviceIn_Enable);
		params_In.add(deviceIn_Volume);
		params_In.add(deviceIn_Port);
		params_In.add(deviceIn_PortName);
		//params_In.add(deviceIn_Api);
		//params_In.add(deviceIn_ApiName);//labels

		// Output
		params_Out.setName("OUTPUT");
		params_Out.add(deviceOut_Enable);
		params_Out.add(deviceOut_Volume);
		params_Out.add(deviceOut_Port);
		params_Out.add(deviceOut_PortName);
		//params_Out.add(deviceOut_ApiName);//labels
		//params_Out.add(deviceOut_Api);

		//--

		// Waveform

		W_bLine.setSerializable(false);
		W_bBars.setSerializable(false);
		W_bCircle.setSerializable(false);

		params_PlotsWaveform.setName("PLOTS WAVEFORM");
		params_PlotsWaveform.add(bGui_PlotIn);
		params_PlotsWaveform.add(bGui_PlotOut);
		params_PlotsWaveform.add(W_Gain);
		params_PlotsWaveform.add(W_Spread);
		params_PlotsWaveform.add(W_bAbs);
		params_PlotsWaveform.add(W_bLine);
		params_PlotsWaveform.add(W_bBars);
		params_PlotsWaveform.add(W_bCircle);
		params_PlotsWaveform.add(W_bHLine);
		params_PlotsWaveform.add(W_bLabel);
		params_PlotsWaveform.add(W_LineWidthScope);
		params_PlotsWaveform.add(W_LineWidthLines);
		params_PlotsWaveform.add(W_Rounded);
		params_PlotsWaveform.add(W_bMirror);
		params_PlotsWaveform.add(W_Width);
		params_PlotsWaveform.add(W_bReset);
		params_PlotsWaveform.add(plotType);//that handles the seon file reload

		//----

		// for the settings file
		params.setName("ofxSoundDevicesManager");
		params.add(params_Control);
		params.add(params_In);
		params.add(params_Out);
		params.add(params_PlotsWaveform);

		ofAddListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);
		ofAddListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);
		ofAddListener(params_Out.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Out);
		ofAddListener(params_PlotsWaveform.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_PlotsWaveform);
	}

	//--

#ifdef USE_ofBaseApp_Pointer 
private:
	ofBaseApp* _app_;
#endif

	//--

private:

	// Sound Devices
	ofSoundStream inStream;
	ofSoundStream outStream;

public:

	int numInputs;
	int numOutputs;
	int sampleRate;
	int bufferSize;

	int numBuffers;
	//nBuffers - Is the number of buffers that your system will create and swap out. The more buffers, the faster your computer will write information into the buffer, but the more memory it will take up. You should probably use two for each channel that you’re using. Here’s an example call:
	//ofSoundStreamSetup(2, 0, 44100, 256, 4);
	//https://openframeworks.cc/documentation/sound/ofSoundStream/

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
	std::vector<ofSoundDevice> outDevices;
	std::vector<string> inDevicesNames;
	std::vector<string> outDevicesNames;

	// Settings
	ofSoundStreamSettings inSettings;
	ofSoundStreamSettings outSettings;

	//-

public:

	ofParameter<bool> bGui;
	ofParameter<bool> bGui_Main;
	ofParameter<bool> bGui_Plots;

	ofFbo fbo;

	//-

private:

	//glm::vec2 position;

	// Parameters

	ofParameter<bool> bEnableAudio;

	// In
	ofParameterGroup params_In;
	ofParameter<bool> deviceIn_Enable;
	ofParameter<float> deviceIn_Volume;
	ofParameter<int> deviceIn_Port;
	ofParameter<string> deviceIn_PortName;
	ofParameter<int> deviceIn_Api;
	ofParameter<string> deviceIn_ApiName;

	// Out
	ofParameterGroup params_Out;
	ofParameter<bool> deviceOut_Enable;
	ofParameter<float> deviceOut_Volume;
	ofParameter<int> deviceOut_Port;
	ofParameter<string> deviceOut_PortName;
	ofParameter<int> deviceOut_Api;
	ofParameter<string> deviceOut_ApiName;

	//--

	ofParameterGroup params;
	ofParameterGroup params_Control;
	ofParameterGroup params_Gui;

	ofParameter<bool> bGui_In{ "Input", true };
	ofParameter<bool> bGui_Out{ "Output", false };

	//--

	ofParameterGroup params_PlotsWaveform;

	ofParameter<bool> bGui_PlotsPanel{ "SOUND PLOTS", false };
	ofParameter<bool> bGui_PlotIn{ "Plot In", true };
	ofParameter<bool> bGui_PlotOut{ "Plot Out", false };

	ofParameter<bool> W_bAbs{ "Abs", true };
	ofParameter<float> W_Spread{ "Spread", 0, 0, 1 };
	ofParameter<float> W_Gain{ "Gain", 0, -1, 1 };
	ofParameter<bool> W_bBars{ "Bars", false };
	ofParameter<bool> W_bCircle{ "Circle", false };
	ofParameter<float> W_Width{ "Width", 0.5, 0, 1 };
	ofParameter<bool> W_bLine{ "Line", true };
	ofParameter<bool> W_bHLine{ "H Line", true };
	ofParameter<bool> W_bMirror{ "Mirror", false };
	ofParameter<float> W_Rounded{ "Rounded", 0, 0, 1 };
	ofParameter<int> W_LineWidthScope{ "L Scope", 3, 1, 10 };
	ofParameter<int> W_LineWidthLines{ "L Lines", 3, 1, 10 };
	ofParameter<bool> W_bLabel{ "Label", true };

	ofParameter<bool> W_bReset{ "Reset", false };

	ofParameter<bool> bGui_Internal{ "Internal", false };

	bool bDISABLE_CALBACKS = false;//to avoid callback crashes or to enable only after setup()

	ofxPanel gui;

	//--

private:

	ofxSurfing_ImGui_Manager guiManager;

	ofxSurfingBoxHelpText textBoxWidget;

	ofxSurfingBoxInteractive boxPlotIn;
	ofxSurfingBoxInteractive boxPlotOut;

	string helpInfo = "";
	string devices_ApiName;

	string pathGlobal = "ofxSoundDevicesManager/";
	string pathSettings = "ofxSoundDevicesManager.xml";

	bool bApiConnected = false;

	//--

public:

	// API

	//--------------------------------------------------------------
	void setVisible(bool b)
	{
		bGui = b;
		if (!bGui_Main) bGui_Main = true;
	}

	//--------------------------------------------------------------
	void setVisibleToggle()
	{
		bGui = !bGui;
		if (!bGui_Main) bGui_Main = true;
	}

private:

	//--------------------------------------------------------------
	void update(ofEventArgs& args)
	{
		if (boxPlotIn.isChangedShape())
		{
			ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << "Box Input plot changed!";
			initFbo(boxPlotIn.getWidth(), boxPlotIn.getHeight());
		}

		//--

		if (bUpdateHelp)
		{
			bUpdateHelp = false;

			buildHelpInfo();
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
		deviceOut_Api = _apiIndex_oF;

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
		outSettings = _settings;

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
		outDevices.clear();

		switch (_apiEnum)
		{
		case MS_WASAPI:
			devices_ApiName = "MS_WASAPI";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
			break;

		case MS_ASIO:
			devices_ApiName = "MS_ASIO";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
			break;

		case MS_DS:
			devices_ApiName = "MS_DS";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_DS);
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_DS);
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
		outDevicesNames.clear();
		outDevicesNames.resize(outDevices.size());
		for (int d = 0; d < outDevices.size(); d++)
		{
			outDevicesNames[d] = outDevices[d].name;
		}

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

		//--

		// Max ports
		deviceIn_Port.setMax(inDevices.size() - 1);
		deviceOut_Port.setMax(outDevices.size() - 1);

		// In
		deviceIn_Api = _apiEnum;
		deviceIn_ApiName = devices_ApiName;
		if (inDevices.size() > deviceIn_Port)
			deviceIn_PortName = inDevices[deviceIn_Port].name;

		// Out
		deviceOut_Api = _apiEnum;
		deviceOut_ApiName = devices_ApiName;
		if (outDevices.size() > deviceOut_Port)
			deviceOut_PortName = outDevices[deviceOut_Port].name;

		//TODO:
		// Force enable
		deviceIn_Enable.setWithoutEventNotifications(bApiConnected);
		deviceOut_Enable.setWithoutEventNotifications(bApiConnected);

		//--

		bUpdateHelp = true;
	}

private:

	//--------------------------------------------------------------
	void drawImGui()
	{
		if (!bGui) return;

		guiManager.begin();
		{
			//guiManager.drawWindowsSpecialsOrganizer();
			guiManager.drawWindowsSpecialsOrganizer();

			//--

			// Main

			if (guiManager.beginWindowSpecial(bGui_Main))
			{

				guiManager.Add(guiManager.bGui_WindowsAlignHelpers, OFX_IM_TOGGLE_ROUNDED);
				guiManager.Add(guiManager.getWindowsAlignHelpersGuiToggle(), OFX_IM_TOGGLE_ROUNDED);

				guiManager.Add(guiManager.bMinimize, OFX_IM_TOGGLE_ROUNDED);
				guiManager.AddSpacing();

				guiManager.Add(bEnableAudio, OFX_IM_TOGGLE);
				guiManager.AddSpacing();

				if (!guiManager.bMinimize) {
					guiManager.AddCombo(apiIndex_Windows, ApiNames);
				}
				guiManager.AddSpacingSeparated();

				guiManager.Add(bGui_In, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				guiManager.Add(bGui_Out, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				guiManager.AddSpacingSeparated();

				guiManager.Add(bGui_PlotsPanel, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				guiManager.Indent();
				guiManager.Add(bGui_Plots, OFX_IM_TOGGLE_ROUNDED);
				guiManager.Unindent();

				if (!guiManager.bMinimize)
				{
					guiManager.AddSpacingSeparated();
					guiManager.Add(textBoxWidget.bGui, OFX_IM_TOGGLE_ROUNDED);
					guiManager.AddSpacingSeparated();
				}

				if (!guiManager.bMinimize)
				{
					guiManager.Add(bGui_Internal, OFX_IM_TOGGLE_ROUNDED_MINI);
				}

				guiManager.endWindowSpecial();
			}

			//--

			// In

			if (guiManager.beginWindowSpecial(bGui_In))
			{
				guiManager.Add(deviceIn_Enable, OFX_IM_TOGGLE);
				guiManager.Add(deviceIn_Volume, OFX_IM_SLIDER);
				guiManager.AddCombo(deviceIn_Port, inDevicesNames);

				guiManager.endWindowSpecial();
			}

			//--

			// Out

			if (guiManager.beginWindowSpecial(bGui_Out))
			{
				guiManager.Add(deviceOut_Enable, OFX_IM_TOGGLE);
				guiManager.Add(deviceOut_Volume, OFX_IM_SLIDER);
				guiManager.AddCombo(deviceOut_Port, outDevicesNames);

				guiManager.endWindowSpecial();
			}

			//--

			// Plots

			if (guiManager.beginWindowSpecial(bGui_PlotsPanel))
			{
				guiManager.Add(bGui_PlotIn, OFX_IM_TOGGLE_ROUNDED);
				if (bGui_PlotIn)
					if (!guiManager.bMinimize) {
						guiManager.Indent();
						guiManager.Add(boxPlotIn.bEdit, OFX_IM_TOGGLE_ROUNDED_MINI);
						guiManager.Add(boxPlotIn.bUseBorder, OFX_IM_TOGGLE_ROUNDED_MINI);
						guiManager.Unindent();
					}
				guiManager.AddSpacingSeparated();
				guiManager.Add(bGui_PlotOut, OFX_IM_TOGGLE_ROUNDED);
				if (bGui_PlotOut)
					if (!guiManager.bMinimize) {
						guiManager.Indent();
						guiManager.Add(boxPlotOut.bEdit, OFX_IM_TOGGLE_ROUNDED_MINI);
						guiManager.Add(boxPlotOut.bUseBorder, OFX_IM_TOGGLE_ROUNDED_MINI);
						guiManager.Unindent();
					}
				guiManager.AddSpacingSeparated();

				guiManager.Add(W_Gain, OFX_IM_SLIDER);
				guiManager.AddSpacingSeparated();

				guiManager.AddLabelBig("Plot Style");

				// type

				if (guiManager.AddButton("<", OFX_IM_BUTTON_SMALL, 2, true)) {
					if (plotType == plotType.getMin()) plotType = plotType.getMax();
					else plotType = plotType - 1;
				}
				if (guiManager.AddButton(">", OFX_IM_BUTTON_SMALL, 2)) {
					if (plotType == plotType.getMax()) plotType = plotType.getMin();
					else plotType = plotType + 1;
				}

				guiManager.AddCombo(plotType, plotTypeNames);
				//guiManager.AddSpacing();

				guiManager.AddSpacingSeparated();

				if (!guiManager.bMinimize)
					if (guiManager.beginTree("EDIT"))
					{
						if (!guiManager.bMinimize) {

							guiManager.Add(W_bLine, OFX_IM_TOGGLE_ROUNDED_MINI);
							guiManager.Add(W_bBars, OFX_IM_TOGGLE_ROUNDED_MINI);
							guiManager.Add(W_bCircle, OFX_IM_TOGGLE_ROUNDED_MINI);

							guiManager.AddSpacingSeparated();
						}

						if (!guiManager.bMinimize) {

							if (plotType == 1)//lines
							{
								guiManager.Add(W_Spread);
							}

							if (plotType == 2 || plotType == 3)//bars, circles
							{
								guiManager.Add(W_Spread);
								guiManager.Add(W_Width);
								guiManager.Add(W_Rounded);
							}

							guiManager.AddSpacing();

							if (plotType == 0)//scope
							{
								guiManager.Add(W_LineWidthScope, OFX_IM_STEPPER);
							}

							if (plotType == 1)//lines
							{
								guiManager.Add(W_LineWidthLines, OFX_IM_STEPPER);
							}

							guiManager.AddSpacing();

							guiManager.Add(W_bAbs, OFX_IM_TOGGLE_ROUNDED_MINI);

							if (plotType != 0)//scope
								guiManager.Add(W_bMirror, OFX_IM_TOGGLE_ROUNDED_MINI);

							guiManager.Add(W_bHLine, OFX_IM_TOGGLE_ROUNDED_MINI);
							guiManager.Add(W_bLabel, OFX_IM_TOGGLE_ROUNDED_MINI);

							guiManager.AddSpacingSeparated();

							guiManager.Add(W_bReset, OFX_IM_BUTTON_SMALL);
						}

						guiManager.endTree();
					}

				guiManager.endWindowSpecial();
			}
		}
		guiManager.end();
	}

public:

	//--------------------------------------------------------------
	void drawGui()
	{
		if (bUseFbo) fbo.draw(300, 300, 320, 240);

		if (bGui_Plots) drawPlots();

		if (bGui)
		{
			drawImGui();

			if (bGui_Internal) gui.draw();

			textBoxWidget.draw();
		}
	}

private:

	//--------------------------------------------------------------
	void close()
	{
		deviceIn_Enable.setWithoutEventNotifications(false);
		deviceOut_Enable.setWithoutEventNotifications(false);

		inStream.stop();
		inStream.close();

		outStream.stop();
		outStream.close();

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
		deviceOut_Port = output;
	}
	//--------------------------------------------------------------
	void setInputDevice(int input)
	{
		deviceIn_Port = input;
	}
	//--------------------------------------------------------------
	void setOutputDevice(int output)
	{
		deviceOut_Port = output;
	}

	//--

private:

	//--------------------------------------------------------------
	void buildHelpInfo()
	{
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
		helpInfo += "\n  Outputs    : " + ofToString(numOutputs) + "\n\n";

		//--

		if (!guiManager.bMinimize)
		{
			helpInfo += "> INPUT DEVICES FROM ALL APIS\n\n";
		}

		//--

		// List all APIS
		// this is slow..
		// 
		// WASAPI
		if (_apiIndex_oF == 7 || !guiManager.bMinimize) // MS_WASAPI
		{
			if (!guiManager.bMinimize) helpInfo += "  > WASAPI \n\n";
			auto devicesWs = inStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
			//auto devicesWs = ApiNames_WASAPI;
			int d = -1;
			for (auto dev : devicesWs)
			{
				d++;
				if (guiManager.bMinimize && d != deviceIn_Port) continue;
				bool b = _apiIndex_oF == 7 && deviceIn_Enable && bApiConnected;
				if (d == deviceIn_Port && b) helpInfo += "* "; else helpInfo += "  ";
				helpInfo += ofToString(dev) + "\n";
			}
			if (!guiManager.bMinimize) helpInfo += "\n";
		}

		// ASIO
		if (_apiIndex_oF == 8 || !guiManager.bMinimize) // MS_ASIO
		{
			if (!guiManager.bMinimize) helpInfo += "  > ASIO \n\n";
			auto devicesAsio = inStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
			//auto devicesAsio = ApiNames_ASIO;
			int d = -1;
			for (auto dev : devicesAsio)
			{
				d++;
				if (guiManager.bMinimize && d != deviceIn_Port) continue;
				bool b = _apiIndex_oF == 8 && deviceIn_Enable && bApiConnected;
				if (d == deviceIn_Port && b) helpInfo += "* "; else helpInfo += "  ";
				helpInfo += ofToString(dev) + "\n";
			}
			if (!guiManager.bMinimize) helpInfo += "\n";
		}

		// DS
		if (_apiIndex_oF == 9 || !guiManager.bMinimize) // MS_DS
		{
			if (!guiManager.bMinimize) helpInfo += "  > MS DIRECTSHOW \n\n";
			auto devicesDs = inStream.getDeviceList(ofSoundDevice::Api::MS_DS);
			//auto devicesDs = ApiNames_DS;
			int d = -1;
			for (auto dev : devicesDs)
			{
				d++;
				if (guiManager.bMinimize && d != deviceIn_Port) continue;
				bool b = _apiIndex_oF == 9 && deviceIn_Enable && bApiConnected;
				if (d == deviceIn_Port && b) helpInfo += "* "; else helpInfo += "  ";
				helpInfo += ofToString(dev) + "\n";
			}
			if (!guiManager.bMinimize) helpInfo += "\n";
		}

		//--

		if (bApiConnected)
		{
			//if (!guiManager.bMinimize)
			{
				//helpInfo += "------------------------------------------";
				helpInfo += "\n> CONNECTED \n\n";
			}

			helpInfo += "  API \n";
			helpInfo += "  " + devices_ApiName + "\n\n";

			helpInfo += "  Input  " + ofToString(deviceIn_Port) + "\n";
			helpInfo += "  " + deviceIn_PortName.get() + "\n\n";

			helpInfo += "  Output " + ofToString(deviceOut_Port) + "\n";
			helpInfo += "  " + deviceOut_PortName.get() + "\n\n";

			//if (!guiManager.bMinimize)
			//{
			//	//helpInfo += "------------------------------------------\n\n\n";
			//}
		}

		//--

		textBoxWidget.setText(helpInfo);
	}

public:

	//--------------------------------------------------------------
	void setPath(string p) {
		pathGlobal = p;

		textBoxWidget.setPath(pathGlobal);

		boxPlotIn.setPathGlobal(pathGlobal);
		boxPlotOut.setPathGlobal(pathGlobal);
	}

private:

	//--------------------------------------------------------------
	void startup()
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__);

		doReset();

		// force
		guiManager.bKeys = false;
		guiManager.bHelpInternal = false;

		// Startup
		// Load Settings
		loadGroup(params, pathGlobal + pathSettings);
	}

	//--------------------------------------------------------------
	void setupGui()
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__);

		guiManager.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
		guiManager.setup();

		guiManager.addWindowSpecial(bGui_Main);
		guiManager.addWindowSpecial(bGui_In);
		guiManager.addWindowSpecial(bGui_Out);
		guiManager.addWindowSpecial(bGui_PlotsPanel);

		guiManager.startup();
	}

	//--------------------------------------------------------------
	float getVolumeInput()
	{
		if (deviceIn_Enable) return deviceIn_Volume;
		else return 0.0f;
	}

	//--------------------------------------------------------------
	float getVolumeOutput()
	{
		if (deviceOut_Enable) return deviceOut_Volume;
		else return 0.0f;
	}

	////--------------------------------------------------------------
	//void setPosition(glm::vec2 _position)
	//{
	//	position = _position;
	//}

	//--------------------------------------------------------------
	void loadGroup(ofParameterGroup& g, string path)
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << "loadGroup: " << g.getName() << " to " << path;
		ofLogVerbose(__FUNCTION__) << "parameters: " << g.toString();
		ofXml settings;
		settings.load(path);
		ofDeserialize(settings, g);
	}

	//--------------------------------------------------------------
	void saveGroup(ofParameterGroup& g, string path)
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << "saveGroup: " << g.getName() << " to " << path;
		ofLogVerbose(__FUNCTION__) << "parameters: " << g.toString();
		ofXml settings;
		ofSerialize(settings, g);
		settings.save(path);
	}

	//-

private:

	bool bUpdateHelp = false;

#define SIZE_BUFFER 4096

	float plotIn[SIZE_BUFFER]; // make this bigger, just in case
	float plotOut[SIZE_BUFFER]; // make this bigger, just in case

	int indexIn = 0;
	int indexOut = 0;

	//float smoothedVolume_Input = 0; // rms signal to use on VU
	//float smoothedVolume_Out = 0; // rms signal to use on VU

	ofParameter<int> plotType{ "Type", 0, 0, 3 };
	vector<string>plotTypeNames = { "Scope", "Lines", "Bars" , "Circles" };

private:

	ofTrueTypeFont myFont;
	std::string path_TTF;
	std::string name_TTF;
	int size_TTF;

	// Legacy
	//void drawWaveforms() { drawPlots(); }

	//--------------------------------------------------------------
	void drawPlots()
	{
		if (!bGui_PlotIn && !bGui_PlotOut) return;

		ofColor cPlot = ofColor(0, 225);
		ofColor cPlotBg = ofColor(64, 128);
		ofColor cText = ofColor(255, 225);

		float g = ofMap(W_Gain, W_Gain.getMin(), W_Gain.getMax(), 0, AMP_GAIN_MAX_POWER, true);

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

				ofSetColor(cPlotBg);
				ofFill();
				ofDrawRectangle(boxPlotIn.getRectangle());

				ofPushMatrix();
				ofTranslate(__x, __y);

				//--

				if (bUseFbo)
					if (bUpdateFbo)
					{
						//bUpdateFbo = false;
						fbo.begin();
						ofClear(0, 0, 0, 0);
					}

				//--

				ofSetColor(cPlot);

				// 0. Scope

				if (plotType == 0)
				{
					ofNoFill();
					ofSetLineWidth(W_LineWidthScope);

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
				}

				//--

				// 123 Lines / Bars / Circles types 

				//if (plotType == 1 || plotType == 2 || plotType == 3)
				else
				{
					ofNoFill();
					ofSetLineWidth(W_LineWidthLines);

					float a = __h * g;//amp gain

					//amount of desired lines or rectangles
					int amount = (int)ofMap(W_Spread, 0, 1, 200, 6, true);

					int stepi = SIZE_BUFFER / amount;
					float stepw = __w / (float)amount;//width of each

					int ii = -1;

					for (int i = 0; i < SIZE_BUFFER; i++)
					{
						// will discard some samples
						if (i % stepi == 0)
						{
							ii++;
						}
						else continue;//skip it

						// The raw point to draw
						int x = ii * stepw;
						int y = plotIn[ii] * a;

						//----

						if (W_bAbs) y = abs(y);

						// fix clamp y and translate for mirror mode
						if (W_bMirror && !W_bCircle)
						{
							if (y < 0) y = (int)ofClamp(y, 0, -__h);
							else y = (int)ofClamp(y, 0, __h);

							ofPushMatrix();
							ofTranslate(0, y / 2);
						}

						//--

						// clamp x

						int px = 2;//pad

						//TODO:
						//bool bskip = false;//discard object that could be out of the box
						//if (x<px || x>(__w - 2 * px)) bskip = true;

						//x = (int)ofClamp(x, px, __w - 2 * px);

						//--

						// clamp y

						y = (int)ofClamp(y, -__h / 2, __h / 2);

						//--

						// A. Line
						if (W_bLine)
						{
							//x = (int)ofClamp(x, px, __w - px);
							if (x > 0 && x < __w) // do or skip if it's outside 
							{
								ofNoFill();
								ofDrawLine(x, 0, x, -y);
							}
						}

						// B. Bars
						if (W_bBars)
						{
							ofFill();
							int wr = ofMap(W_Width, 0, 1, 4, stepw, true);
							int xr = x - wr / 2;
							x = (int)ofClamp(xr, px, __w - px);
							//if (xr > wr && x < (__w - wr)) // do or skip if it's outside 
							{
								if (W_Rounded != 0)ofDrawRectRounded(xr, 0, wr, -y, wr * W_Rounded);
								else ofDrawRectangle(xr, 0, wr, -y);
							}
						}

						// C. Circle 
						if (W_bCircle)
						{
							float r = ofMap(W_Width, 0, 1, MAX(2, y * 0.05), y * 1, true);
							if (x > r && x < (__w - r)) // do or skip if it's outside
							{
								ofFill();
								ofDrawCircle(x, 0, r);
							}
						}

						if (W_bMirror && !W_bCircle)
						{
							ofPopMatrix();
						}
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

				// label
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
					ofxSurfingHelpers::drawTextBoxedMini(myFont, s, xx, yy);
				}

				//--

				if (bUseFbo)
					if (bUpdateFbo)
					{
						fbo.end();
						bUpdateFbo = true;
					}

				//--

				ofPopMatrix();
			}

			//----

			// Out

			//TODO: WIP:

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
		}
		ofPopStyle();
	}

public:

	//--------------------------------------------------------------
	void audioIn(ofSoundBuffer& input) {

		std::size_t nChannels = input.getNumChannels();

		if (deviceIn_Enable)
		{
			float _rms = 0.0;
			int _count = 0;

			for (size_t i = 0; i < input.getNumFrames(); i++)
			{
				plotIn[indexIn] = input[i * nChannels] * getVolumeInput();

				if (indexIn < (SIZE_BUFFER - 1))
				{
					++indexIn;
				}
				else
				{
					indexIn = 0;
				}

				//-

				// code from here: https://github.com/edap/examplesOfxMaxim
				// rms calculation as explained here http://openframeworks.cc/ofBook/chapters/sound.html

				float left = input[0];
				float right = input[1];

				_rms += left * left;
				_rms += right * right;

				//--

				_count += 2; // 2 channels
				//_count += 1; // 1 channel
			}

			//--

			_rms /= (float)_count;
			_rms = sqrt(_rms);

			//smoothedVolume_Input = _rms * deviceIn_Volume;
		}

		// not enabled: 
		// erase plot
		else
		{
			for (size_t i = 0; i < input.getNumFrames(); ++i)
			{
				plotIn[indexIn] = 0;

				if (indexIn < (SIZE_BUFFER - 1))
				{
					++indexIn;
				}
				else
				{
					indexIn = 0;
				}
			}
		}
	}

	//--------------------------------------------------------------
	void audioOut(ofSoundBuffer& output) {
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
				plotOut[indexOut] = 0;
				if (indexOut < (ofGetWidth() - 1))
				{
					++indexOut;
				}
				else
				{
					indexOut = 0;
				}
			}
		}
	}

private:

	//--------------------------------------------------------------
	void Changed_params_Control(ofAbstractParameter& e)
	{
		if (bDISABLE_CALBACKS) return;

		string name = e.getName();
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << name << " " << e;

		if (0) {}

		// MS Windows sound APIs
		else if (name == guiManager.bMinimize.getName())
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

	//--------------------------------------------------------------
	void doReset()
	{
		W_Spread = 0;
		W_Width = 0.5;
		W_Gain = 0;
		W_bHLine = false;
		W_bLine = true;
		W_bBars = false;
		W_bCircle = false;
		W_bAbs = true;
		W_bMirror = true;
		W_Rounded = 0.5;
		W_bLabel = true;

		plotType = 3;

		W_LineWidthScope = 2;
		W_LineWidthLines = 3;
	}

	//--------------------------------------------------------------
	void Changed_params_PlotsWaveform(ofAbstractParameter& e)
	{
		if (bDISABLE_CALBACKS) return;

		string name = e.getName();

		if (0) {}

		else if (name == W_bReset.getName() && W_bReset)
		{
			W_bReset = false;
			doReset();
		}

		//--

		// Plot waveform
		else if (name == plotType.getName())
		{
			switch (plotType)
			{
			case 0://scope
				W_bLine = false;
				W_bBars = false;
				W_bCircle = false;
				break;

			case 1://lines
				W_bLine = true;
				W_bBars = false;
				W_bCircle = false;
				break;

			case 2://bar
				W_bLine = false;
				W_bBars = true;
				W_bCircle = false;
				break;

			case 3://circle
				W_bLine = false;
				W_bBars = false;
				W_bCircle = true;
				break;
			}

			return;
		}


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

// NOTES
//https://github.com/firmread/ofxFftExamples/blob/master/example-eq/src/ofApp.cpp#L78
//https://forum.openframeworks.cc/t/question-around-void-audioin-ofsoundbuffer-buffer/22284/2
//https://forum.openframeworks.cc/t/openframeworks-and-speech-synthesis-tts-with-flite/12117/7
//https://forum.openframeworks.cc/t/playing-audio-input-in-real-time-problems-syncing/29180/2?u=moebiussurfing
//http://openframeworks.cc/documentation/sound/ofSoundBuffer/#!show_resampleTo
//https://forum.openframeworks.cc/t/listen-to-ofevent-audioin-in-custom-class/27710/3?u=moebiussurfing