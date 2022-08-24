#pragma once

/*

	TODO:

	+	WIP: INPUT ONLY. But probably can be enabled and easy to fix.

	+	WIP: WINDOWS ONLY
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

// OPTIONAL

#define USE_WAVEFORM_PLOTS

#define SOUND_DEVICES_DISABLE_OUTPUT 
//TODO: Must be duplicated to WaveformPlot.h. Must edit on both places!


//TODO: WIP:
#define USE_ofBaseApp_Pointer
//enabled: add-on class uses a passed by reference ofBaseApp pointer. 
//disabled: gets ofBaseApp 'locally'. not sure if this can helps on in/out callbacks..


//#define USE_OFXGUI_INTERNAL // ofxGui internal debug or split from ImGui..

//-----------------------


#ifdef USE_OFXGUI_INTERNAL 
#include "ofxGui.h"
#endif

#include "ofxSurfingBoxHelpText.h"
#include "ofxSurfingBoxInteractive.h"
#include "ofxSurfingImGui.h"

#ifdef USE_WAVEFORM_PLOTS
#include "WaveformPlot.h"
#endif


#ifdef USE_ofBaseApp_Pointer
//--------------------------------------------------------------
class ofxSoundDevicesManager
#else
//--------------------------------------------------------------
class ofxSoundDevicesManager : public ofBaseApp
#endif

{

public:

	int indexIn = 0;
	int indexOut = 0;

#ifdef USE_WAVEFORM_PLOTS
	WaveformPlot waveformPlot;
#endif

	//--------------------------------------------------------------
	ofxSoundDevicesManager()
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
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		numOutputs = 2;
#endif
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
		ofxSurfingHelpers::CheckFolder(pathGlobal);
		ofxSurfingHelpers::saveGroup(params_Settings, pathGlobal + "/" + pathSettings);

		ofRemoveListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
		//TODO:
		// add macOS + Linux
		ApiNames.clear();
		ApiNames.push_back("MS_WASAPI"); // 0
		ApiNames.push_back("MS_ASIO"); // 1
		ApiNames.push_back("MS_DS"); // 2

		//-

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

		// TODO:
		// Hardcoded
		apiIndex_Windows = 2; // For the gui? 0-1-2 // WASAPI, ASIO, DS
		//// TODO:
		//_apiIndex_oF = 9; // API #9 on oF is Windows MS DS

		connectToSoundAPI(_apiIndex_oF); // MS_DS
		//connectToSoundAPI(_app, _apiIndex_oF); // MS_DS

#ifdef USE_WAVEFORM_PLOTS
		waveformPlot.setup();
#endif

		//--

		bDISABLE_CALBACKS = false;

		//----

		// Comment to avoid loading settings
		// helpful for when the app is stucked (when starting maybe)
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
		bGui_Main.set("DEVICES", true);

		//--

		// Gui Toggles
		params_Gui.setName("GuiToggles");
		params_Gui.add(bGui);
		params_Gui.add(bGui_Main);
		params_Gui.add(bGui_In);
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		params_Gui.add(bGui_Out);
#endif
#ifdef USE_WAVEFORM_PLOTS
		params_Gui.add(waveformPlot.bGui_Plots);
		params_Gui.add(waveformPlot.bGui_PlotsPanel);
#endif
		params_Gui.add(bGui_Internal);
		params_Gui.add(boxHelpInfo.bGui);

		bGui_Internal.setSerializable(false);

		// Control
		params_Control.setName("CONTROL");
		params_Control.add(bEnableAudio);
		params_Control.add(apiIndex_Windows); 
		params_Control.add(params_Gui);

		params_Control.add(ui.bMinimize);//to refresh help info

		//-

		// In
		deviceIn_Enable.set("Enable", false);
		deviceIn_Port.set("Port", 0, 0, 10);
		// that param is the loaded from settings. not the name. should be the same
		deviceIn_Volume.set("Volume", 0.5f, 0.f, 1.f);
		deviceIn_Api.set("Api", 0, 0, 10);
		deviceIn_ApiName.set("Api ", "");
		deviceIn_PortName.set("Port ", "");

		// Out
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		deviceOut_Enable.set("Enable", false);
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
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		deviceOut_Api.setSerializable(false);
		deviceOut_ApiName.setSerializable(false);
		deviceOut_PortName.setSerializable(false);
#endif

		//-

		// Input
		params_In.setName("INPUT");
		params_In.add(deviceIn_Enable);
		//params_In.add(deviceIn_Volume);
		params_In.add(deviceIn_Port);
		params_In.add(deviceIn_PortName);
		//params_In.add(deviceIn_Api);
		//params_In.add(deviceIn_ApiName);//labels

		// Output
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		params_Settings.add(params_Out);
#endif

		ofAddListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);
		ofAddListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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

	// Sound Devices

	ofSoundStream inStream;
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
	ofSoundStream outStream;
#endif

public:

	int numInputs;
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
	std::vector<ofSoundDevice> outDevices;
	std::vector<string> outDevicesNames;
#endif

	// Settings

	ofSoundStreamSettings inSettings;
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
	ofSoundStreamSettings outSettings;
#endif

	//-

public:

	ofParameter<bool> bGui;
	ofParameter<bool> bGui_Main;

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

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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

	ofParameter<bool> bGui_In{ "Input", true };

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
	ofParameter<bool> bGui_Out{ "Output", false };
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

	string nameLabel = "SoundDevices";
	string pathGlobal = "SoundDevices";
	string pathSettings = "SoundDevices_Settings.json";

	bool bApiConnected = false;

	//--

public:

	// API

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

private:

	//--------------------------------------------------------------
	void update(ofEventArgs& args)
	{
#ifdef USE_WAVEFORM_PLOTS
		waveformPlot.update();
#endif

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

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		outDevices.clear();
#endif

		switch (_apiEnum)
		{
		case MS_WASAPI:
			devices_ApiName = "MS_WASAPI";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
#endif
			break;

		case MS_ASIO:
			devices_ApiName = "MS_ASIO";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
#endif
			break;

		case MS_DS:
			devices_ApiName = "MS_DS";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_DS);
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		deviceOut_Port.setMax(outDevices.size() - 1);
#endif

		// In

		deviceIn_Api = _apiEnum;
		deviceIn_ApiName = devices_ApiName;
		if (inDevices.size() > deviceIn_Port)
			deviceIn_PortName = inDevices[deviceIn_Port].name;

		// Out

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		deviceOut_Api = _apiEnum;
		deviceOut_ApiName = devices_ApiName;
		if (outDevices.size() > deviceOut_Port)
			deviceOut_PortName = outDevices[deviceOut_Port].name;
#endif

		//TODO:
		// Force enable

		deviceIn_Enable.setWithoutEventNotifications(bApiConnected);
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		deviceOut_Enable.setWithoutEventNotifications(bApiConnected);
#endif

		//--

		bUpdateHelp = true;
	}

private:

	//--------------------------------------------------------------
	void drawImGui()
	{
		//if (!bGui) return;

		ui.Begin();
		{
			//ui.drawWindowsSpecialsOrganizer();
			//ui.drawWindowsSpecialsOrganizer();

			//--

			// Main

			if (ui.BeginWindowSpecial(bGui_Main))
			{
				ui.Add(ui.bMinimize, OFX_IM_TOGGLE_ROUNDED);
				ui.AddSpacing();

				ui.Add(bEnableAudio, OFX_IM_TOGGLE);
				ui.AddSpacing();

				if (ui.bMinimize) 
				{
					ui.Add(bGui_In, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
					ui.Add(bGui_Out, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
#endif

#ifdef USE_WAVEFORM_PLOTS
					ui.Add(waveformPlot.bGui_Plots, OFX_IM_TOGGLE_ROUNDED);
#endif
				}
				else 
				{
					ui.AddCombo(apiIndex_Windows, ApiNames);
					ui.AddSpacingSeparated();
					ui.Add(bGui_In, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
					ui.Add(bGui_Out, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
#endif

#ifdef USE_WAVEFORM_PLOTS
					ui.AddSpacingSeparated();
					ui.Add(waveformPlot.bGui_PlotsPanel, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
					ui.Indent();
					ui.Add(waveformPlot.bGui_Plots, OFX_IM_TOGGLE_ROUNDED);
					ui.Unindent();
#endif
					ui.AddSpacingSeparated();
					ui.Add(boxHelpInfo.bGui, OFX_IM_TOGGLE_ROUNDED);

#ifdef USE_OFXGUI_INTERNAL 
					ui.Add(bGui_Internal, OFX_IM_TOGGLE_ROUNDED_MINI);
#endif
				}

				ui.EndWindowSpecial();
			}

			//--

			// In

			if (ui.BeginWindowSpecial(bGui_In))
			{
				ui.Add(deviceIn_Enable, OFX_IM_TOGGLE);
				//ui.Add(deviceIn_Volume, OFX_IM_SLIDER);
				ui.AddCombo(deviceIn_Port, inDevicesNames);

				ui.EndWindowSpecial();
			}

			//--

			// Out

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
			if (ui.BeginWindowSpecial(bGui_Out))
			{
				ui.Add(deviceOut_Enable, OFX_IM_TOGGLE);
				//ui.Add(deviceOut_Volume, OFX_IM_SLIDER);
				ui.AddCombo(deviceOut_Port, outDevicesNames);

				ui.EndWindowSpecial();
			}
#endif
			//--
		}
		ui.End();
	}

public:

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
		waveformPlot.draw();
#endif
	}

private:

	//--------------------------------------------------------------
	void close()
	{
		deviceIn_Enable.setWithoutEventNotifications(false);
		inStream.stop();
		inStream.close();

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		deviceOut_Port = output;
#endif
	}

	//--------------------------------------------------------------
	void setInputDevice(int input)
	{
		deviceIn_Port = input;
	}

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
		if (_apiIndex_oF == 7 || !ui.bMinimize) // MS_WASAPI
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
		if (_apiIndex_oF == 8 || !ui.bMinimize) // MS_ASIO
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
		if (_apiIndex_oF == 9 || !ui.bMinimize) // MS_DS
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

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
		waveformPlot.setPath(p);
#endif
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
	}

	//--------------------------------------------------------------
	void setupGui()
	{
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__);

		ui.setName(nameLabel);
		ui.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
		ui.setup();

		ui.addWindowSpecial(bGui_Main);
		ui.addWindowSpecial(bGui_In);
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
		ui.addWindowSpecial(bGui_Out);
#endif
		// Plot
		//ui.addWindowSpecial(waveformPlot.bGui_PlotsPanel);

		ui.startup();

		//// vertical
		//ui.setWindowsSpecialsOrientation(true);
	}

	////--------------------------------------------------------------
	//float getVolumeInput()
	//{
	//	if (deviceIn_Enable) return deviceIn_Volume;
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

	////TODO:
	//float smoothedVolume_Input = 0; // rms signal to use on VU
	//float smoothedVolume_Out = 0; // rms signal to use on VU

	bool bUpdateHelp = false;

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
#ifdef USE_WAVEFORM_PLOTS
				////plotIn[indexIn] = input[i * nChannels] * getVolumeInput();
				waveformPlot.plotIn[indexIn] = input[i * nChannels];

				if (indexIn < (SIZE_BUFFER - 1))
				{
					++indexIn;
				}
				else
				{
					indexIn = 0;
				}
#endif

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

#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
		ofLogNotice("ofxSoundDevicesManager") << (__FUNCTION__) << name << " " << e;

		if (0) {}

		// MS Windows sound APIs
		else if (name == ui.bMinimize.getName())
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
#ifndef SOUND_DEVICES_DISABLE_OUTPUT
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
#endif

};

// NOTES
//https://github.com/firmread/ofxFftExamples/blob/master/example-eq/src/ofApp.cpp#L78
//https://forum.openframeworks.cc/t/question-around-void-audioin-ofsoundbuffer-buffer/22284/2
//https://forum.openframeworks.cc/t/openframeworks-and-speech-synthesis-tts-with-flite/12117/7
//https://forum.openframeworks.cc/t/playing-audio-input-in-real-time-problems-syncing/29180/2?u=moebiussurfing
//http://openframeworks.cc/documentation/sound/ofSoundBuffer/#!show_resampleTo
//https://forum.openframeworks.cc/t/listen-to-ofevent-audioin-in-custom-class/27710/3?u=moebiussurfing