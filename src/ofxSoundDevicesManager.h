#pragma once

/// TODO:
///
/// +		add some other example
///	+++		enable output and test with an example
///	+++		integrate with ofSoundObjects
/// ++		add switch api/device. without exceptions/crashes.
///	+++		add background box to clarify text
/// ++		change all to ofSoundBuffer, not buffer, channels..etc
/// ++		add disconnectors to use only input or output. now, enablers are only like mutes. 
/// +		add user gui toggle for advanced mode. key to 'G'
/// +		add macOS/linux apis?
/// +		add samplerate and other settings to gui selectors. store to xml too. restart must be required maybe
/// +		store devices by names? just editing xml file bc sorting can change on the system?
/// +		alternative waveforms plotting: https://github.com/Feliszt/sound-analyzer-OF
/// +		better vumeter using other rms snippets

#include "ofMain.h"

//-----------------------

// DEFINES

//#define USE_ofBaseApp_Pointer
// //TODO: WIP		
//enabled: addon class uses a passed by reference ofBaseApp pointer. 
//disabled: gets ofBaseApp 'locally'. not sure if this can helps on in/out callbacks..

//-----------------------

#include "ofxGui.h"
#include "ofxSimpleFloatingGui.h"
#include "ofxSurfingBoxHelpText.h"
#include "ofxSurfingImGui.h"

#ifdef USE_ofBaseApp_Pointer
//--------------------------------------------------------------
class ofxSoundDevicesManager
#else
//--------------------------------------------------------------
class ofxSoundDevicesManager : public ofBaseApp
#endif
{

public:

	ofxSurfing_ImGui_Manager guiManager;

	ofxSurfingBoxHelpText textBoxWidget;

	string helpInfo = "";

	//--------------------------------------------------------------
	void logLine(string text)
	{
		helpInfo += text + "\n";
	}

	//--------------------------------------------------------------
	ofxSoundDevicesManager()
	{
		ofAddListener(ofEvents().update, this, &ofxSoundDevicesManager::update);

		bDISABLE_CALBACKS = true;

		//-

		// default audio seTtings

		// internal default
		sampleRate = 44100;
		//sampleRate = 48000;

		// internal default
		bufferSize = 512;
		//bufferSize = 256; 

		// internal default
		numBuffers = 4;
		//numBuffers = 2;

		//-

		// channels
		numInputs = 2;
		numOutputs = 2;

		//--

		bGui.set("SOUND DEVICES", true);
		bGui_Main.set("MAIN", true);

		params_Control.setName("CONTROL");
		params_Control.add(apiGuiIndex);
		params_Control.add(bGui_In);
		params_Control.add(bGui_Out);
		params_Control.add(bGui_Waveform);
		params_Control.add(bGui_Internal);
		params_Control.add(textBoxWidget.bGui);

		//-

		deviceIn_Enable.set("ENABLE", false);
		deviceIn_Volume.set("VOLUME", 0.5f, 0.f, 1.f);
		deviceIn_Api.set("API", 0, 0, 10);
		deviceIn_ApiName.set("", "");
		deviceIn_Port.set("PORT", 0, 0, 10);
		deviceIn_PortName.set("", "");

		deviceOut_Enable.set("ENABLE", false);
		deviceOut_Volume.set("VOLUME", 0.5f, 0.f, 1.f);
		deviceOut_Api.set("API", 0, 0, 10);
		deviceOut_ApiName.set("", "");
		deviceOut_Port.set("PORT", 0, 0, 10);
		deviceOut_PortName.set("", "");

		// exclude
		deviceIn_Api.setSerializable(false);
		deviceIn_ApiName.setSerializable(false);
		deviceIn_PortName.setSerializable(false);

		deviceOut_Api.setSerializable(false);
		deviceOut_ApiName.setSerializable(false);
		deviceOut_PortName.setSerializable(false);

		//-

		//TODO:
		apiOnAllOFSystemsIndex = 9; // api #9 on OF is MS-DS

		//-

		params_In.setName("INPUT");
		params_In.add(deviceIn_Enable);
		params_In.add(deviceIn_Volume);
		params_In.add(deviceIn_Api);
		params_In.add(deviceIn_Port);
		params_In.add(deviceIn_ApiName);//labels
		params_In.add(deviceIn_PortName);

		params_Out.setName("OUTPUT");
		params_Out.add(deviceOut_Enable);
		params_Out.add(deviceOut_Volume);
		params_Out.add(deviceOut_Api);
		params_Out.add(deviceOut_Port);
		params_Out.add(deviceOut_ApiName);//labels
		params_Out.add(deviceOut_PortName);

		//--

		// Waveform
		params_Waveform.setName("WAVEFORM");
		params_Waveform.add(bGui_WaveIn);
		params_Waveform.add(bGui_WaveOut);
		params_Waveform.add(W_Height);
		params_Waveform.add(W_Spread);
		params_Waveform.add(W_bAbs);
		params_Waveform.add(W_bLine);
		params_Waveform.add(W_LineWidth);
		params_Waveform.add(W_bRectangle);
		params_Waveform.add(W_WidthGap);
		params_Waveform.add(W_bReset);

		//--

		params.setName("ofxSoundDevicesManager");
		params.add(params_Control);
		params.add(params_In);
		params.add(params_Out);
		params.add(params_Waveform);

		ofAddListener(params_Waveform.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Waveform);
		ofAddListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);
		ofAddListener(params_Out.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Out);

		ofAddListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);

		// user gui
		position = glm::vec2(25, 25);

		// text box
		textBoxWidget.setTitle("\nSOUND DEVICES");
		textBoxWidget.setFontSize(8);
		textBoxWidget.setFontTitleSize(11);
		textBoxWidget.setup();
	};

	//--------------------------------------------------------------
	~ofxSoundDevicesManager() {
		exit();

		ofRemoveListener(ofEvents().update, this, &ofxSoundDevicesManager::update);
	};

	//--------------------------------------------------------------
	void exit() {
		saveGroup(params, pathSettings);

		ofRemoveListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);
		ofRemoveListener(params_Out.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Out);
		ofRemoveListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);
		ofRemoveListener(params_Waveform.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Waveform);

		close();
	};

	//--

public:

	string pathSettings = "ofxSoundDevicesManager.xml";
	string str_error = "";

#ifdef USE_ofBaseApp_Pointer 
	ofBaseApp* _app_;
#endif

	//-

private:

	// Sound devices
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

	// Api
	int apiOnAllOFSystemsIndex;//all OF possile apis!
	//NOT the index of the available apis on this system.!
	//ie: on this windows system could be: wasapi, asio, ds -> will be 0 to 2
	ofParameter<int> apiGuiIndex{ "API", 0, 0, 2 };//this index will be related to all available apis ONLY on this system!
	//also in your same system, devices will change when disabling on windows sound preferences/devices

	std::vector<string> ApiNames;

	// devices
	std::vector<ofSoundDevice> inDevices;
	std::vector<ofSoundDevice> outDevices;

	std::vector<string> inDevicesNames;
	std::vector<string> outDevicesNames;

	// settings
	ofSoundStreamSettings inSettings;
	ofSoundStreamSettings outSettings;

	//-

	ofParameter<bool> bGui;
	ofParameter<bool> bGui_Main;

	glm::vec2 position;

	//-

private:

	// parameters

	// in
	ofParameterGroup params_In;
	ofParameter<bool> deviceIn_Enable;
	ofParameter<float> deviceIn_Volume;
	ofParameter<int> deviceIn_Api;
	ofParameter<string> deviceIn_ApiName;
	ofParameter<int> deviceIn_Port;
	ofParameter<string> deviceIn_PortName;

	// out
	ofParameterGroup params_Out;
	ofParameter<bool> deviceOut_Enable;
	ofParameter<float> deviceOut_Volume;
	ofParameter<int> deviceOut_Api;
	ofParameter<string> deviceOut_ApiName;
	ofParameter<int> deviceOut_Port;
	ofParameter<string> deviceOut_PortName;

	//-

	ofParameterGroup params;
	ofParameterGroup params_Control;

	ofxPanel gui;
	ofParameter<bool> bGui_Internal{ "Internal", true };

	ofParameter<bool> bGui_In{ "Input", true };
	ofParameter<bool> bGui_Out{ "Output", false };

	//--

	ofParameterGroup params_Waveform;
	ofParameter<bool> bGui_Waveform{ "Waveforms", true };
	ofParameter<bool> bGui_WaveIn{ "Wave In", true };
	ofParameter<bool> bGui_WaveOut{ "Wave Out", false };
	ofParameter<bool> W_bAbs{ "Abs", true };
	ofParameter<float> W_Spread{ "Spread", 0, 0, 1 };
	ofParameter<int> W_Height{ "Hight", 500, 10, 5000 };
	ofParameter<bool> W_bRectangle{ "Rectangle", false };
	ofParameter<float> W_WidthGap{ "Gap", 1, 0, 1 };
	ofParameter<bool> W_bLine{ "Line", true };
	ofParameter<int> W_LineWidth{ "LineWidth", 3, 1, 100 };
	ofParameter<bool> W_bReset{ "Reset", false };

	//-

	bool bDISABLE_CALBACKS = false;//to avoid callback crashes or to enable only after setup()

public:

	// API

	void setVisible(bool b)
	{
		bGui_Internal = b;
	}
	void setVisibleToggle()
	{
		bGui_Internal = !bGui_Internal;
		if (!bGui && bGui_Internal) bGui = true;
	}

	//void setVisibleAdvanced(bool b)
	//{
	//	bGui_Advanced = b;
	//}
	//void toggleVisibleAdvanced()
	//{
	//	bGui_Advanced = !bGui_Advanced;
	//}

	//--

	//----

public:

	////--------------------------------------------------------------
	//void updateGuiUser()
	//{
	//	//simple callbacks

	//	//in

	//	//enable
	//	if (GUI_enableInput.getValue() != deviceIn_Enable)
	//	{
	//		deviceIn_Enable = GUI_enableInput.getValue();
	//	}
	//	//volumen
	//	if (GUI_volumeInput.getValue() != deviceIn_Volume)
	//	{
	//		deviceIn_Volume = GUI_volumeInput.getValue();
	//	}
	//	//port
	//	if (GUI_deviceIndexInput.getValueInt() != deviceIn_Port)
	//	{
	//		inStream.close();
	//		inSettings.setInDevice(inDevices[GUI_deviceIndexInput.getValueInt()]);
	//		inStream.setup(inSettings);
	//		deviceIn_Port = GUI_deviceIndexInput.getValueInt();
	//	}

	//	//-

	//	//out

	//	//enable
	//	if (GUI_enableOutput.getValue() != deviceOut_Enable)
	//	{
	//		deviceOut_Enable = GUI_enableOutput.getValue();
	//	}

	//	//volumen
	//	if (GUI_volumeOutput.getValue() != deviceOut_Volume)
	//	{
	//		deviceOut_Volume = GUI_volumeOutput.getValue();
	//	}

	//	//port
	//	if (GUI_deviceIndexOutput.getValueInt() != deviceOut_Port)
	//	{
	//		outStream.close();
	//		if (outDevices.size() > GUI_deviceIndexOutput.getValueInt())
	//			outSettings.setOutDevice(outDevices[GUI_deviceIndexOutput.getValueInt()]);
	//		outStream.setup(outSettings);
	//		deviceOut_Port = GUI_deviceIndexOutput.getValueInt();
	//	}

	//	//-

	//	//api
	//	if (GUI_Api.getValueInt() != apiGuiIndex)
	//	{
	//		int iEnum;
	//		int _i = GUI_Api.getValueInt();
	//		ofLogVerbose(__FUNCTION__) << "GUI_Api.getValueInt() " << _i << endl;

	//		//windows apis
	//		switch (_i)
	//		{
	//		case 0:
	//			iEnum = 7;//MS_WASAPI
	//			break;
	//		case 1:
	//			iEnum = 8;//MS_ASIO
	//			break;
	//		case 2:
	//			iEnum = 9;//MS_DS
	//			break;
	//		default:
	//			ofLogError(__FUNCTION__) << "error: iEnum: " << iEnum << endl;
	//			break;
	//		}
	//		ofLogVerbose(__FUNCTION__) << "iEnum:" << iEnum;

	//		//-

	//		connectAp(iEnum);
	//		apiGuiIndex = GUI_Api.getValueInt();
	//	}
	//}

	//--------------------------------------------------------------
	void update(ofEventArgs& args)
	{
		//updateWaveforms();//for using alternative plot engines

		//-

		////user gui
		//updateGuiUser();
	}

	//--------------------------------------------------------------
	//void connectAp(ofBaseApp* _app, int _apiIndex)
	void connectAp(int _apiIndex)
	{
		close();

		string str;

		//-

		//clean
		ofSoundStreamSettings _settings;
		inSettings = _settings;
		outSettings = _settings;

		//----

		//supported apis
		enum Api {
			UNSPECIFIED = 0,
			DEFAULT,		//TODO: must implement all other APIs: macOS. I don't have Linux...
			ALSA,			/*!< The Advanced Linux Sound Architecture API. */
			PULSE,			/*!< The Linux PulseAudio API. */
			OSS,			/*!< The Linux Open Sound System API. */
			JACK,			/*!< The Jack Low-Latency Audio Server API. */
			OSX_CORE,		/*!< Macintosh OS-X Core Audio API. */
			MS_WASAPI,//7	/*!< The Microsoft WASAPI API. */
			MS_ASIO,//8		/*!< The Steinberg Audio Stream I/O API. */
			MS_DS,//9		/*!< The Microsoft Direct Sound API. */
			NUM_APIS
		} _apiEnum = UNSPECIFIED;

		//TODO:
		// api choice

		//index is related to (above enum) all OF supported apis
		//_apiEnum = MS_WASAPI;//7
		//_apiEnum = MS_ASIO;//8
		//_apiEnum = MS_DS;//9

		switch (_apiIndex)
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
		default:
			break;
		}

		//------------------

		// apis and devices

		inDevices.clear();
		outDevices.clear();

		string str_api;
		switch (_apiEnum)
		{
		case MS_WASAPI:
			str_api = "MS_WASAPI";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
			break;

		case MS_ASIO:
			str_api = "MS_ASIO";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
			break;

		case MS_DS:
			str_api = "MS_DS";
			inDevices = inStream.getDeviceList(ofSoundDevice::Api::MS_DS);
			outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_DS);
			break;
		}

		//-

		// api settings & devices

		//input

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
		if (inDevices.size() > deviceIn_Port)
			inSettings.setInDevice(inDevices[deviceIn_Port]);

		inStream.setup(inSettings);

		//-

		//output

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
			outSettings.setOutDevice(outDevices[deviceOut_Port]);

		outStream.setup(outSettings);

		//-

		str = "------------------------------------------";
		logLine(str);

		str = "\n> CONNECTED DEVICE\n";
		logLine(str);

		str = "API    [ " + str_api + " ]";
		logLine(str);

		str = "Input  [ " + ofToString(deviceIn_Port) + " ]";
		logLine(str);

		str = "Output [ " + ofToString(deviceOut_Port) + " ]";
		logLine(str);

		str = "------------------------------------------";
		logLine(str);

		str = "\n";
		logLine(str);

		textBoxWidget.setText(helpInfo);

		//-

		//update params

		//max ports
		deviceIn_Port.setMax(inDevices.size() - 1);
		deviceOut_Port.setMax(outDevices.size() - 1);

		deviceIn_Api = _apiEnum;
		deviceIn_ApiName = str_api;
		if (inDevices.size() > deviceIn_Port)
			deviceIn_PortName = inDevices[deviceIn_Port].name;

		deviceOut_Api = _apiEnum;
		deviceOut_ApiName = str_api;
		deviceOut_PortName = outDevices[deviceOut_Port].name;
	}

	//--------------------------------------------------------------
	void drawImGui()
	{
		if (!bGui) return;

		guiManager.begin();
		{
			if (guiManager.beginWindowSpecial(bGui_Main))
			{
				guiManager.AddLabelBig(bGui_Main.getName());
				guiManager.AddGroup(params_Control);
				guiManager.endWindowSpecial();
			}
			if (guiManager.beginWindowSpecial(bGui_In))
			{
				guiManager.AddLabelBig(bGui_In.getName());
				guiManager.AddGroup(params_In);
				guiManager.endWindowSpecial();
			}

			if (guiManager.beginWindowSpecial(bGui_Out))
			{
				guiManager.AddLabelBig(bGui_Out.getName());
				guiManager.AddGroup(params_Out);
				guiManager.endWindowSpecial();
			}

			if (guiManager.beginWindowSpecial(bGui_Waveform))
			{
				guiManager.AddLabelBig(bGui_Out.getName());
				guiManager.AddGroup(params_Waveform);
				guiManager.endWindowSpecial();
			}
		}
		guiManager.end();
	}

	//--------------------------------------------------------------
	void drawGui()
	{
		if (bGui)
		{
			drawWaveforms();
	
			//--

			drawImGui();

			//--

			if (bGui_Internal) gui.draw();

			textBoxWidget.draw();
		}
	}

	//--------------------------------------------------------------
	void close()
	{
		inStream.stop();
		inStream.close();

		outStream.stop();
		outStream.close();

		ofSoundStreamStop();
		ofSoundStreamClose();
	}

	// Settings
#ifdef USE_ofBaseApp_Pointer 
	//--------------------------------------------------------------
	void setup(ofBaseApp* _app, int _samplerate, int _buffersize, int _numbuffers)
#else
	//--------------------------------------------------------------
	void setup(int _samplerate, int _buffersize, int _numbuffers)
#endif
	{
		sampleRate = _samplerate;
		bufferSize = _buffersize;
		numBuffers = _numbuffers;

#ifdef USE_ofBaseApp_Pointer 
		setup(_app);
#else
		setup();
#endif
	}

	//--------------------------------------------------------------
#ifdef USE_ofBaseApp_Pointer
	void setup(ofBaseApp* _app, int _samplerate, int _buffersize)
#else
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

	//--------------------------------------------------------------
	//devices
	void setDevices(int input, int output)
	{
		deviceIn_Port = input;
		deviceOut_Port = output;
	}
	void setInputDevice(int input)
	{
		deviceIn_Port = input;
	}
	void setOutputDevice(int output)
	{
		deviceOut_Port = output;
	}

	//-

	void setupHelpInfo()
	{
		string _str;
		string str;

		// Log
		_str = "\n> SETUP DEVICE\n";
		logLine(_str);
		_str = "SAMPLERATE  : " + ofToString(sampleRate);
		logLine(_str);
		_str = "BUFFERSIZE  : " + ofToString(bufferSize);
		logLine(_str);
		_str = "NUM BUFFERS : " + ofToString(numBuffers);
		logLine(_str);
		_str = "NUM INPUTS  : " + ofToString(numInputs);
		logLine(_str);
		_str = "NUM OUTPUTS : " + ofToString(numOutputs);
		logLine(_str);

		//------------------

		str = "\n> LIST AUDIO DEVICES FROM ALL APIS\n";
		logLine(str);

		// List all APIS

		// ASIO
		logLine("ASIO\n");
		auto devicesAsio = inStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
		for (auto dev : devicesAsio)
		{
			str = ofToString(dev);
			logLine(str);
		}
		str = "";
		logLine(str);

		// DS
		logLine("DS\n");
		auto devicesDs = inStream.getDeviceList(ofSoundDevice::Api::MS_DS);
		for (auto dev : devicesDs)
		{
			str = ofToString(dev);
			logLine(str);
		}
		str = "";
		logLine(str);

		// WASAPI
		logLine("WASAPI\n");
		auto devicesWs = inStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
		for (auto dev : devicesWs)
		{
			str = ofToString(dev);
			logLine(str);
		}
		str = "";
		logLine(str);

		//--

		textBoxWidget.setText(helpInfo);
	}

#ifdef USE_ofBaseApp_Pointer
	//--------------------------------------------------------------
	void setup(ofBaseApp* _app) {
#else
	//--------------------------------------------------------------
	void setup() {
#endif

#ifdef USE_ofBaseApp_Pointer 
		_app_ = _app;
#endif
		//-

		//TODO:
		// windows only
		ApiNames.clear();
		ApiNames.push_back("MS_WASAPI");//0
		ApiNames.push_back("MS_ASIO");//1
		ApiNames.push_back("MS_DS");//2

		//--

		setupHelpInfo();

		
		//--

		bDISABLE_CALBACKS = false;

		// Settings
		loadGroup(params, pathSettings);

		//-

		// API

		//connectAp(_app, apiOnAllOFSystemsIndex);//MS_DS
		connectAp(apiOnAllOFSystemsIndex);//MS_DS

		// TODO:
		// Harcoded
		apiGuiIndex = 2;//for gui

		//-

		// Control
		gui.setup("DEVICES");
		gui.add(params);
		gui.getGroup("ofxSoundDevicesManager").minimizeAll();
		gui.getGroup("ofxSoundDevicesManager").minimize();

		// Advanced
		gui.setPosition(700, 400);

		//--

		//// waveform
		//gui_waveform.setup("WAVEFORM");
		//gui_waveform.add(params_Waveform);

		//--

		setupGui();
	}

	//--------------------------------------------------------------
	void setupGui()
	{
		guiManager.setWindowsMode(IM_GUI_MODE_WINDOWS_SPECIAL_ORGANIZER);
		guiManager.setup();

		guiManager.addWindowSpecial(bGui_Main);
		guiManager.addWindowSpecial(bGui_In);
		guiManager.addWindowSpecial(bGui_Out);
		guiManager.addWindowSpecial(bGui_Waveform);

		guiManager.startup();

		guiManager.bHelpInternal = false;
	}

	//--------------------------------------------------------------
	float getVolumeInput()
	{
		if (deviceIn_Enable)
			return deviceIn_Volume;
		else
			return 0.0;
	}

	//--------------------------------------------------------------
	float getVolumeOutput()
	{
		if (deviceOut_Enable)
			return deviceOut_Volume;
		else
			return 0.0;
	}

	//--------------------------------------------------------------
	void setPosition(glm::vec2 _position)
	{
		position = _position;
	}

	//--------------------------------------------------------------
	void loadGroup(ofParameterGroup & g, string path)
	{
		ofLogNotice(__FUNCTION__) << "loadGroup: " << g.getName() << " to " << path;
		ofLogVerbose(__FUNCTION__) << "parameters: " << g.toString();
		ofXml settings;
		settings.load(path);
		ofDeserialize(settings, g);

	}

	//--------------------------------------------------------------
	void saveGroup(ofParameterGroup & g, string path)
	{
		ofLogNotice(__FUNCTION__) << "saveGroup: " << g.getName() << " to " << path;
		ofLogVerbose(__FUNCTION__) << "parameters: " << g.toString();
		ofXml settings;
		ofSerialize(settings, g);
		settings.save(path);
	}

	//-

#define SIZE_BUFFER 4096
	float waveformInput[SIZE_BUFFER]; //make this bigger, just in case
	int waveInputIndex;
	float waveformOutput[SIZE_BUFFER]; //make this bigger, just in case
	int waveOutputIndex;


	//alternative plot 1
	//rms, vu's
	//ofParameter<bool> enableSmooth;//enable smooth VU
	//ofParameter<float> smoothRatio;
	float smoothedVolume_Input = 0;//rms signal to use on VU
	float smoothedVolume_Out = 0;//rms signal to use on VU
	//ofParameterGroup paramsWaveforms;
	//float scaledVolume = 0;
	//alternative plots
	//ofParameter<int> radius;
	//ofParameter<int> lineScale;
	//vector <float> outHistory;
	//ofPolyline line;

	////alternative plot 2
	//SliderB     volScale;
	//float curVol, curVolPrev, divVol, maxDivVol;
	//int   numHist;
	//float smoothedVol;
	//float scaledVol;
	//int 	bufferCounter;
	//vector <float> left;
	//vector <float> right;
	//vector <float> volHistory;
	//vector <float> derVolHistory;
	//vector <float> freq_amp;
	//vector <float> magnitude;
	//vector <float> phase;
	//vector <float> power;
	//vector <float> binsAmp, binsAmpNormalized, maxFreqBins;

	////--------------------------------------------------------------
	//void setupWaveforms() {
	//	paramsWaveforms.setName("VU");
	//	paramsWaveforms.add(enableSmooth.set("ENABLE SMOOTH", false));
	//	paramsWaveforms.add(smoothRatio.set("SMOOTH", 0.97, 0.70, 0.99));

	//	//alternative plots
	//	//paramsWaveforms.add(radius.set("MAX", 200, 100, 800));
	//	//paramsWaveforms.add(lineScale.set("SCALE", 500, 200, 1200));
	//	//outHistory.assign(ofGetWidth(), 0.0);

	//	//////plot2
	//	//// Signals setup
	//	//left.assign(bufferSize, 0.0);
	//	//right.assign(bufferSize, 0.0);
	//	//freq_amp.assign((int)bufferSize / 2, 0.0);
	//	//volHistory.assign(ofGetWidth(), 0.0);
	//	////volHistory.assign(WW, 0.0);
	//	//magnitude.assign(bufferSize, 0.0);
	//	//power.assign(bufferSize, 0.0);
	//	//phase.assign(bufferSize, 0.0);
	//	////binsAmp.assign(numBinsSetting, 0.0);
	//	////binsAmpNormalized.assign(numBinsSetting, 0.0);
	//	////maxFreqBins.assign(50, 0.0);
	//}

	//alternative plots
	////--------------------------------------------------------------
	//void updateWaveforms() {
	//	//copied from audioInExample
	//	outHistory.push_back(smoothedVolume_Out);
	//	//if we are bigger the the size we want to record - lets drop the oldest value
	//	if (outHistory.size() >= ofGetWidth()) {
	//		outHistory.erase(outHistory.begin(), outHistory.begin() + 1);
	//	}
	//	int x = 0;
	//	for (auto vol : outHistory) {
	//		auto ver = glm::vec3(x, -vol * lineScale, 0);
	//		line.addVertex(ver);
	//		if (line.size() > ofGetWidth()) {
	//			line.getVertices().erase(
	//				line.getVertices().begin()
	//			);
	//		}
	//		x++;
	//	}
	//}

	//--------------------------------------------------------------
	void drawWaveforms() {
		ofPushStyle();
		ofPushMatrix();
		{
			ofFill();
			ofSetColor(0, 225);
			ofSetLineWidth(W_LineWidth);

			int _width = ofGetWidth();
			float _max = 200;
			int _margin = 50;

			// input
			ofTranslate(0, ofGetHeight() / 4.f);

			if (bGui_WaveIn)
			{
				////oscilloscope style
				//ofDrawLine(0, 0, 1, waveformInput[1] * _max); //first line
				//for (int i = 1; i < (ofGetWidth() - 1); ++i) {
				//	ofDrawLine(i, waveformInput[i] * _max, i + 1, waveformInput[i + 1] * _max);
				//}

				// bars style

				int __spread = W_Spread * bufferSize;
				__spread = ofClamp(__spread, 1, bufferSize);

				float y1 = 0;

				float numBars = (bufferSize / __spread);
				float wBars = _width / numBars;

				//float _pw = abs(wBars - W_WidthGap);
				float _pw = (wBars * W_WidthGap);
				_pw = MAX(1, _pw);

				//TODO: this could be redone
				for (int i = 0; i < bufferSize; i++)
				{
					if (i % __spread == 0)//spread spaces
					{
						float p = (i / (float)(bufferSize));//normalized pos in array
						float x = p * _width;

						float _ph;
						if (W_bAbs) _ph = abs(waveformInput[i] * W_Height);//make positive
						else _ph = waveformInput[i] * W_Height;
						//_ph = MAX(0, _ph);

						//y1 = ofClamp(y1, 0, _h);
						//_ph = ofClamp(_ph, 0, _h);

						if (W_bLine) ofDrawLine(x, y1, x, -_ph);//line
						if (W_bRectangle) ofDrawRectangle(x, y1, _pw, -_ph);//rectangle
					}
				}

				ofDrawBitmapStringHighlight("INPUT ", ofGetWidth() - _margin, 5);
				//drawWaveform(waveformInput, 0, 0, 0, 0);
			}

			//--

			if (bGui_WaveOut) {
				// output
				ofTranslate(0, 2 * ofGetHeight() / 4);
				ofDrawLine(0, 0, 1, waveformOutput[1] * _max); // first line

				for (int i = 1; i < (ofGetWidth() - 1); ++i) {
					ofDrawLine(i, waveformOutput[i] * _max, i + 1, waveformOutput[i + 1] * _max);
				}

				ofDrawBitmapStringHighlight("OUTPUT", ofGetWidth() - _margin, +5);
			}

			//-

			////alternative plots
			////ofSetColor(255, 0, 0);
			////ofDrawCircle(500, -500, smoothedVolume_Out * radius);
			////line.draw();
			//////cout << "smoothedVolume_Out:" << smoothedVolume_Out << endl;

			//ofTranslate(0, -ofGetHeight() / 2.f);
			//int WW = 500;
			//int HW = 500;

			//// draw circle for scaled volume
			//ofSetColor(255,0,0,200);
			//ofFill();
			//ofDrawCircle(WW / 2, HW / 2, ofClamp(smoothedVol * 100, 0, HW / 2));

			////lets draw the volume history as a graph
			//ofBeginShape();
			//for (unsigned int i = 0; i < derVolHistory.size(); i++) {
			//	float abs = ofMap(i, 0, derVolHistory.size(), 0, WW);
			//	if (i == 0) ofVertex(i, HW);
			//	float value = ofClamp(ofMap(derVolHistory[i], 0, volScale.getValue(), 0, HW), 0, HW);
			//	ofVertex(abs, HW - value);
			//	if (i == derVolHistory.size() - 1) ofVertex(abs, HW);
			//}
			//ofEndShape(false);

			//-
		}
		ofPopMatrix();
		ofPopStyle();

		//--
	}

	//--------------------------------------------------------------
	void audioIn(ofSoundBuffer & input) {

		std::size_t nChannels = input.getNumChannels();

		if (deviceIn_Enable)
		{
			// vu
			float rms = 0.0;
			int numCounted1 = 0;

			for (size_t i = 0; i < input.getNumFrames(); i++)
			{
				waveformInput[waveInputIndex] = input[i * nChannels]
					* getVolumeInput()
					;

				if (waveInputIndex < (ofGetWidth() - 1)) {
					++waveInputIndex;
				}
				else {
					waveInputIndex = 0;
				}

				//-

				//vu
				//code from here: https://github.com/edap/examplesOfxMaxim
				//rms calculation as explained here http://openframeworks.cc/ofBook/chapters/sound.html
				float left = input[0];
				float right = input[1];
				rms += left * left;
				rms += right * right;
				numCounted1 += 2;
				//numCounted1 += 1;//1 channel
			}

			//--

			//vu
			rms /= (float)numCounted1;
			rms = sqrt(rms);

			//if (enableSmooth) {
			//	// normalize to 1
			//	//smoothedVolume_Input = ofMap(rms, 0, 0.707 * 1, 0, 1);
			//	smoothedVolume_Input *= smoothRatio;
			//	smoothedVolume_Input += 0.07 * rms;
			//}
			//else {
			//	smoothedVolume_Input = rms;
			//}

			smoothedVolume_Input = rms * deviceIn_Volume;

			//----

			////plot2
			//float avg_power = 0.0f;
			//curVol = 0.0;

			//// samples are "interleaved"
			//int numCounted = 0;

			////lets go through each sample and calculate the root mean square which is a rough way to calculate volume
			//for (int i = 0; i < bufferSize; i++) {
			//	left[i] = input[i * 2] * 1.0f;
			//	curVol += left[i] * left[i];
			//	numCounted += 1;
			//}

			//// compute mean of rms
			//curVol /= (float)numCounted;

			//// compute root of rms
			//curVol = sqrt(curVol);

			//// normalize to 1
			//curVol = ofMap(curVol, 0, 0.707 * 1, 0, 1);

			//// compute diff
			//divVol = ofClamp(curVol - curVolPrev, 0, 999);
			//if (divVol > maxDivVol) maxDivVol = divVol;
			//derVolHistory.push_back(divVol);
			//if (derVolHistory.size() >= numHist) {
			//	derVolHistory.erase(derVolHistory.begin(), derVolHistory.begin() + 1);
			//}
			//curVolPrev = curVol;

			//smoothedVol *= 0.93;
			//smoothedVol += 0.07 * curVol;

			//bufferCounter++;
		}
		else//erase plot
		{
			for (size_t i = 0; i < input.getNumFrames(); ++i)
			{
				waveformInput[waveInputIndex] = 0;
				if (waveInputIndex < (ofGetWidth() - 1)) {
					++waveInputIndex;
				}
				else {
					waveInputIndex = 0;
				}
			}
		}
	}

	//--------------------------------------------------------------
	void audioOut(ofSoundBuffer & output) {
		std::size_t outChannels = output.getNumChannels();

		if (deviceOut_Enable)
		{
			//vu
			float rms = 0.0;
			int numCounted = 0;

			////fill buffer with noise
			//for (size_t i = 0; i < output.getNumFrames(); ++i)
			//{
			//	output[i * outChannels] = ofRandom(-1, 1)
			//		* getVolumeOutput()
			//		;
			//	output[i * outChannels + 1] = output[i * outChannels];

			//	waveformOutput[waveOutputIndex] = output[i * outChannels];
			//	if (waveOutputIndex < (ofGetWidth() - 1)) {
			//		++waveOutputIndex;
			//	}
			//	else {
			//		waveOutputIndex = 0;
			//	}

			//	//-

			//	//vu
			//	//code from here: https://github.com/edap/examplesOfxMaxim
			//	//rms calculation as explained here http://openframeworks.cc/ofBook/chapters/sound.html
			//	float left = output[0];
			//	float right = output[1];
			//	rms += left * left;
			//	rms += right * right;
			//	numCounted += 2;
			//}

			//--

			//vu
			rms /= (float)numCounted;
			rms = sqrt(rms);

			//if (enableSmooth) {
			//	smoothedVolume_Out *= smoothRatio;
			//	smoothedVolume_Out += 0.07 * rms;
			//}
			//else {
			//	smoothedVolume_Out = rms;
			//}

			smoothedVolume_Out = rms * deviceOut_Volume;
		}
		else//erase plot
		{
			for (size_t i = 0; i < output.getNumFrames(); ++i)
			{
				waveformOutput[waveOutputIndex] = 0;
				if (waveOutputIndex < (ofGetWidth() - 1)) {
					++waveOutputIndex;
				}
				else {
					waveOutputIndex = 0;
				}
			}
		}
	}

	//--------------------------------------------------------------
	void Changed_params_Control(ofAbstractParameter & e)
	{
		if (!bDISABLE_CALBACKS)
		{
			string name = e.getName();

		}
	}

	//--------------------------------------------------------------
	void Changed_params_Waveform(ofAbstractParameter & e)
	{
		if (bDISABLE_CALBACKS) return;

		{
			string name = e.getName();

			if (name == W_bReset.getName() && W_bReset) {
				W_bReset = false;

				W_WidthGap = 1;
				W_Spread = 0;
				W_Height = 500;
				W_bRectangle = false;
				W_bLine = true;
				W_bAbs = true;
				W_LineWidth = 3;
			}

			if (name == W_bLine.getName()) {
				if (W_bLine) W_bRectangle = false;
				else W_bRectangle = true;
			}

			if (name == W_bRectangle.getName()) {
				if (W_bRectangle) W_bLine = false;
				else W_bLine = true;
			}

			//in
			if (name == deviceIn_Port.getName()) {
				inStream.close();
				inSettings.setInDevice(inDevices[deviceIn_Port]);
				inStream.setup(inSettings);
			}

			//out
			if (name == deviceOut_Port.getName()) {
				outStream.close();
				if (outDevices.size() > deviceOut_Port)
					outSettings.setOutDevice(outDevices[deviceOut_Port]);
				outStream.setup(outSettings);
			}

			if (name == deviceOut_Port.getName()) {
				outStream.close();
				if (outDevices.size() > deviceOut_Port)
					outSettings.setOutDevice(outDevices[deviceOut_Port]);
				outStream.setup(outSettings);
			}

			//windows apis
			if (name == apiGuiIndex.getName()) {
				int iEnum;
				switch (apiGuiIndex)
				{
				case 0:
					iEnum = 7;//MS_WASAPI
					break;
				case 1:
					iEnum = 8;//MS_ASIO
					break;
				case 2:
					iEnum = 9;//MS_DS
					break;
				default:
					ofLogError(__FUNCTION__) << "error: iEnum: " << iEnum << endl;
					break;
				}
				ofLogVerbose(__FUNCTION__) << "iEnum:" << iEnum;
				connectAp(iEnum);
			}

			//if (name == bBars.getName() && bBars) {
			//	bBars = false;
			//	_spread = 10;
			//}
		}
	}

	//-

	//--------------------------------------------------------------
	void Changed_params_In(ofAbstractParameter & e)
	{
		if (!bDISABLE_CALBACKS)
		{
			string name = e.getName();


		}
	}

	//--------------------------------------------------------------
	void Changed_params_Out(ofAbstractParameter & e)
	{
		if (!bDISABLE_CALBACKS)
		{
			string name = e.getName();


		}
	}

	//--------------------------------------------------------------
	void drawVU(float val, int x, int y, int w, int h)
	{
		ofPushStyle();

		ofNoFill();
		ofDrawRectangle(x, y - h, w, h);

		ofFill();
		ofDrawRectangle(x, y - h, val * w, h);

		ofPopStyle();
	}

};


//NOTES

//example to convert to ofSoundBuffer
//https://github.com/firmread/ofxFftExamples/blob/master/example-eq/src/ofApp.cpp#L78

//https://forum.openframeworks.cc/t/question-around-void-audioin-ofsoundbuffer-buffer/22284/2

//https://forum.openframeworks.cc/t/openframeworks-and-speech-synthesis-tts-with-flite/12117/7
//The TTS class notifies that event.You should capture that event(look the examples of customEvents) and keep the soundBuffer.
//Then, you have to use the class ofSoundStream.When you setup up that object, it starts calling the function audioOut(you have to create it).So, you’ll have
//
//
//
////--------------------------------------------------------------  
//void testApp::newSoundBuffer(const TTSData & tts) {
//	mutex.lock();
//	soundBuffer = *tts.buffer;
//	mutex.unlock();
//}
//
////--------------------------------------------------------------  
//void testApp::audioOut(float * output, int buffersize, int nChannels, int deviceID, unsigned long long int tickCount) {
//	mutex.lock();
//	soundBuffer.copyTo(output, buffersize, nChannels, position, true);
//	if (soundBuffer.size() > 0) {
//		position += buffersize;
//		position %= soundBuffer.bufferSize();
//	}
//	mutex.unlock();
//}


//https://forum.openframeworks.cc/t/playing-audio-input-in-real-time-problems-syncing/29180/2?u=moebiussurfing
//roymacdonald
//Apr '18
//Hi.you can use the ofApp audio callbacks.look at the audioIn and audioOut examples so you can set up the audio streams.
//make an instance of ofSoundBuffer and copy into it what you get from audioIn and then pass it to audioOut´s buffer.
//
//should be something like this.
//
//// put these lines in the ofApp.h file
//ofSoundBuffer buffer;
//void audioIn(ofSoundBuffer & input);
//void audioOut(ofSoundBuffer & output);
//
//// the following ones go in the ofApp.cpp file
//void audioIn(ofSoundBuffer & input) {
//	input.copyTo(buffer);
//}
//void audioOut(ofSoundBuffer & output) {
//	buffer.copyTo(output);
//}
//if you need to resample the buffer in order to adjust speed, you can use the ofBuffer’s resampleTo() methdod.
//http://openframeworks.cc/documentation/sound/ofSoundBuffer/#!show_resampleTo 1
//hope this helps

//https://forum.openframeworks.cc/t/listen-to-ofevent-audioin-in-custom-class/27710/3?u=moebiussurfing
//Apr '19
//The way to listen to audio events is by setting them in the soundstream settings.If you have a class like :
//
//	class MyClass {
//	void audioIn(ofSoundBuffer & buffer);
//	void audioOut(ofSoundBuffer & buffer);
//}
//In ofApp :
//
////.h
//ofSoundStream soundStream;
//MyClass myobj;
//
////setup
//ofSoundStreamSettings settings;
//settings.setInListener(&myobj);
//settings.setOutListener(&myobj);
// set any other required settings
//soundStream.setup(settings);


//TODO:
//must learn how to pass a vector as reference...
//void drawWaveform(vector<float> buffer, float x, float y, float w, float h)
//{
//	//bars style
//	ofFill();
//	int _h = 1000;
//	int _width = ofGetWidth();
//	bool W_bLine, bBars;
//	W_bLine = true;
//	bBars = false;
//	int _spread;//use one bar x 20 buffer frames. ignore the others. but depends on relation beetween viewport width vs bufferSize
//	if (W_bLine)
//		_spread = 1;
//	if (bBars)
//		_spread = 10;
//	//float _maxBars = _width /
//	int W_WidthGap = 5;//space between bars
//	float y1 = 0;
//	float numBars = (bufferSize / _spread);
//	float wBars = _width / numBars;
//	float _pw = abs(wBars - W_WidthGap);
//	for (int i = 0; i < bufferSize; i++) {
//		if (i%_spread == 0)//spread spaces
//		{
//			float p = (i / (float)(bufferSize));//normalized pos in array
//			float x = p * _width;
//
//			float _ph = 1.0f + abs(buffer[i] * _h);//make positive. 1.0 for minimal line
//			float y2 = 0.1f + y1 - _ph;//0.1 for minimal line
//
//			if (W_bLine)
//				ofDrawLine(x, y1, x, y2);//line
//
//			if (bBars)
//				ofDrawRectangle(x, y1, _pw, -_ph);//bars could be nice with rms not scope style
//		}
//	}
//}
