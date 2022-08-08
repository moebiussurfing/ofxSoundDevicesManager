#pragma once

/*

TODO:

	+	draggable rectangle to draw the waveforms
	+	fix api selector. add switch API/device. without exceptions/crashes.

	+	enable output and test with an example
	+	add macOS/Linux APIs ? https://github.com/roymacdonald/ofxSoundDeviceManager
	+	add disconnect to allow use only input or output. now, enablers are only like mutes.
	+	store devices by names? just editing xml file bc sorting can change on the system?

	+	integrate with ofSoundObjects
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
//#define USE_ofBaseApp_Pointer
//enabled: addon class uses a passed by reference ofBaseApp pointer. 
//disabled: gets ofBaseApp 'locally'. not sure if this can helps on in/out callbacks..

//-----------------------

#include "ofxGui.h"
#include "ofxSurfingBoxHelpText.h"
#include "ofxSurfingImGui.h"

//#ifdef USE_ofBaseApp_Pointer
////--------------------------------------------------------------
//class ofxSoundDevicesManager {
//#endif
//
//#ifndef USE_ofBaseApp_Pointer
//	//--------------------------------------------------------------
//	class ofxSoundDevicesManager : public ofBaseApp {
//#endif

class ofxSoundDevicesManager : public ofBaseApp {
public:

	//--------------------------------------------------------------
	ofxSoundDevicesManager()
	{
		//--

		// Default audio settings

		// Internal default
		sampleRate = 44100;
		//sampleRate = 48000;

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

		apiIndex_oF = 0;

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
		saveGroup(params, pathSettings);

		ofRemoveListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);
		ofRemoveListener(params_Out.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Out);
		ofRemoveListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);
		ofRemoveListener(params_Waveform.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Waveform);

		close();
	}

public:

	//#ifdef USE_ofBaseApp_Pointer 
	//		//--------------------------------------------------------------
	//		void setup(ofBaseApp* _app, int _samplerate, int _buffersize, int _numbuffers)
	//#else
	//		//--------------------------------------------------------------
	//		void setup(int _samplerate, int _buffersize, int _numbuffers)
	//#endif

	void setup(int _samplerate, int _buffersize, int _numbuffers)
	{
		sampleRate = _samplerate;
		bufferSize = _buffersize;
		numBuffers = _numbuffers;

		//#ifdef USE_ofBaseApp_Pointer 
		//			setup(_app);
		//#else
		setup();
		//#endif
	}

	//#ifdef USE_ofBaseApp_Pointer
	//		//--------------------------------------------------------------
	//		void setup(ofBaseApp* _app, int _samplerate, int _buffersize)
	//
	//#else
	//		//--------------------------------------------------------------
	//		void setup(int _samplerate, int _buffersize)
	//#endif

	void setup(int _samplerate, int _buffersize)
	{
		sampleRate = _samplerate;
		bufferSize = _buffersize;

		//#ifdef USE_ofBaseApp_Pointer 
		//			setup(_app);
		//#else
		//			setup();
		//#endif

		setup();
	}

	//--

public:

	//#ifdef USE_ofBaseApp_Pointer
	//		//--------------------------------------------------------------
	//		void setup(ofBaseApp* _app)
	//
	//#else
	//		//--------------------------------------------------------------
	//		void setup()
	//#endif

	void setup()
	{
		//#ifdef USE_ofBaseApp_Pointer 
		//			_app_ = _app;
		//#endif

		//--

		bEnableAudio.set("Enable Audio", true);

		bGui.set("SOUND DEVICES", true);

		bGui_Main.set("SOUND DEVICES", true);
		//bGui_Main.set("MAIN", true);

		params_Control.setName("CONTROL");
		params_Control.add(bEnableAudio);
		params_Control.add(bGui_Main);
		params_Control.add(bGui_In);
		params_Control.add(bGui_Out);
		params_Control.add(bGui_Waveform);
		params_Control.add(bGui_Internal);
		params_Control.add(textBoxWidget.bGui);
		params_Control.add(guiManager.bMinimize);

		//-

		deviceIn_Enable.set("Enable", false);
		deviceIn_Volume.set("Volume", 0.5f, 0.f, 1.f);
		deviceIn_Api.set("API", 0, 0, 10);
		deviceIn_ApiName.set("", "");
		deviceIn_Port.set("Port", 0, 0, 10);
		deviceIn_PortName.set("", "");

		deviceOut_Enable.set("Enable", false);
		deviceOut_Volume.set("Volume", 0.5f, 0.f, 1.f);
		deviceOut_Api.set("API", 0, 0, 10);
		deviceOut_ApiName.set("", "");
		deviceOut_Port.set("Port", 0, 0, 10);
		deviceOut_PortName.set("", "");

		// exclude
		deviceIn_Api.setSerializable(false);
		deviceIn_ApiName.setSerializable(false);
		deviceIn_PortName.setSerializable(false);

		deviceOut_Api.setSerializable(false);
		deviceOut_ApiName.setSerializable(false);
		deviceOut_PortName.setSerializable(false);

		//-

		params_In.setName("INPUT");
		params_In.add(deviceIn_Enable);
		params_In.add(deviceIn_Volume);
		params_In.add(deviceIn_Port);
		params_In.add(deviceIn_PortName);
		//params_In.add(deviceIn_Api);
		//params_In.add(deviceIn_ApiName);//labels

		params_Out.setName("OUTPUT");
		params_Out.add(deviceOut_Enable);
		params_Out.add(deviceOut_Volume);
		params_Out.add(deviceOut_Port);
		params_Out.add(deviceOut_PortName);
		//params_Out.add(deviceOut_ApiName);//labels
		//params_Out.add(deviceOut_Api);

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

		//----

		params.setName("ofxSoundDevicesManager");
		params.add(params_Control);
		params.add(params_In);
		params.add(params_Out);
		params.add(params_Waveform);

		ofAddListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);
		ofAddListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);
		ofAddListener(params_Out.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Out);
		ofAddListener(params_Waveform.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Waveform);

		//--

		// Text box
		textBoxWidget.bGui.setName("Info");
		textBoxWidget.setTitle("\nInfo");
		textBoxWidget.setFontSize(8);
		textBoxWidget.setFontTitleSize(11);
		textBoxWidget.setup();

		//----

		//TODO:
		// For MS Windows only.
		//TODO:
		// add macOS + Linux
		ApiNames.clear();
		ApiNames.push_back("MS_WASAPI");//0
		ApiNames.push_back("MS_ASIO");//1
		ApiNames.push_back("MS_DS");//2

		//-

		// Internal Gui
		gui.setup("DEVICES");
		gui.add(params);
		gui.getGroup(params.getName()).minimizeAll();
		gui.getGroup(params.getName()).minimize();

		// Advanced
		// user gui
		position = glm::vec2(25, 25);
		gui.setPosition(position.x, position.y);

		//--

		setupGui();

		//--

		bDISABLE_CALBACKS = false;

		//--

		// API

		// TODO:
		// Hardcoded
		apiIndex_Windows = 2; // for the gui? 0-1-2

		//// TODO:
		//apiIndex_oF = 9; // API #9 on oF is Windows MS DS

		//connectToSoundAPI(apiIndex_oF); // MS_DS
		////connectToSoundAPI(_app, apiIndex_oF); // MS_DS

		//----

		startup();
	}

	//--

//#ifdef USE_ofBaseApp_Pointer 
	//private:
//		ofBaseApp* _app_;
//#endif

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
	int apiIndex_oF;
	// all oF possible apis!
	// NOT the index of the available apis on this system.!
	// eg: on this windows system could be: wasapi, asio, ds -> will be 0 to 2
	ofParameter<int> apiIndex_Windows{ "API", 0, 0, 2 };
	// shared by both input and output
	// this index will be related to all available apis ONLY on this system!
	// also in your same system, devices will change when disabling on windows sound preferences/devices

	//TODO:
	// add macOS + Linux

	std::vector<string> ApiNames;

	// Devices
	std::vector<ofSoundDevice> inDevices;
	std::vector<ofSoundDevice> outDevices;
	std::vector<string> inDevicesNames;
	std::vector<string> outDevicesNames;

	// Settings
	ofSoundStreamSettings inSettings;
	ofSoundStreamSettings outSettings;

	//-

	ofParameter<bool> bGui;
	ofParameter<bool> bGui_Main;

	glm::vec2 position;

	//-

private:

	// Parameters

	ofParameter<bool> bEnableAudio;

	// in
	ofParameterGroup params_In;
	ofParameter<bool> deviceIn_Enable;
	ofParameter<float> deviceIn_Volume;
	ofParameter<int> deviceIn_Port;
	ofParameter<string> deviceIn_PortName;
	ofParameter<int> deviceIn_Api;
	ofParameter<string> deviceIn_ApiName;

	// out
	ofParameterGroup params_Out;
	ofParameter<bool> deviceOut_Enable;
	ofParameter<float> deviceOut_Volume;
	ofParameter<int> deviceOut_Port;
	ofParameter<string> deviceOut_PortName;
	ofParameter<int> deviceOut_Api;
	ofParameter<string> deviceOut_ApiName;

	//-

	ofParameterGroup params;
	ofParameterGroup params_Control;

	ofParameter<bool> bGui_In{ "Input", true };
	ofParameter<bool> bGui_Out{ "Output", false };

	//--

	ofParameterGroup params_Waveform;

	ofParameter<bool> bGui_Waveform{ "Waveforms", false };
	ofParameter<bool> bGui_WaveIn{ "Wave In", true };
	ofParameter<bool> bGui_WaveOut{ "Wave Out", false };

	ofParameter<bool> W_bAbs{ "Abs", true };
	ofParameter<float> W_Spread{ "Spread", 0, 0, 1 };
	ofParameter<int> W_Height{ "Height", 500, 10, 1000 };
	ofParameter<bool> W_bRectangle{ "Rectangle", false };
	ofParameter<float> W_WidthGap{ "Gap", 1, 0, 1 };
	ofParameter<bool> W_bLine{ "Line", true };
	ofParameter<int> W_LineWidth{ "LineWidth", 3, 1, 100 };
	ofParameter<bool> W_bReset{ "Reset", false };

	ofParameter<bool> bGui_Internal{ "Internal", false };

	bool bDISABLE_CALBACKS = false;//to avoid callback crashes or to enable only after setup()

	ofxPanel gui;

	//--

private:

	ofxSurfing_ImGui_Manager guiManager;

	ofxSurfingBoxHelpText textBoxWidget;

	string helpInfo = "";
	string str_api;

	string pathSettings = "ofxSoundDevicesManager.xml";

	//--

public:

	// API

	//--------------------------------------------------------------
	void setVisible(bool b)
	{
		bGui = b;
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
	}

private:

	//void connectToSoundAPI(ofBaseApp* _app, int _apiIndex)

	//--------------------------------------------------------------
	void connectToSoundAPI(int _apiIndex)
	{
		if (!bEnableAudio) return;

		close();

		// TODO:
		apiIndex_oF = _apiIndex; // API #9 on oF is Windows MS DS

		if (apiIndex_oF != 7 && apiIndex_oF != 8 && apiIndex_oF != 9)
		{
			ofLogError(__FUNCTION__) << "Skip API index bc out of MS Windows range: " << apiIndex_oF << endl;

			return;
		}

		string str;

		//-

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

		//TODO:
		// API choice
		// 
		//index is related to (above enum) all OF supported apis
		//_apiEnum = MS_WASAPI;//7
		//_apiEnum = MS_ASIO;//8
		//_apiEnum = MS_DS;//9

		switch (apiIndex_oF)
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

		// APIs and devices

		inDevices.clear();
		outDevices.clear();

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

		//#ifdef USE_ofBaseApp_Pointer 
		//			inSettings.setInListener(_app_);
		//#else
		//			inSettings.setInListener(ofGetAppPtr());//?
		//#endif

		inSettings.setInListener(ofGetAppPtr());//?

		if (inDevices.size() > deviceIn_Port)
			inSettings.setInDevice(inDevices[deviceIn_Port]);

		inStream.setup(inSettings);

		//-

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

		//#ifdef USE_ofBaseApp_Pointer 
		//			outSettings.setOutListener(_app_);
		//#else
		//			outSettings.setOutListener(ofGetAppPtr());//?
		//#endif

		outSettings.setOutListener(ofGetAppPtr());//?

		if (outDevices.size() > deviceOut_Port)
			outSettings.setOutDevice(outDevices[deviceOut_Port]);

		outStream.setup(outSettings);

		//-

		// max ports
		deviceIn_Port.setMax(inDevices.size() - 1);
		deviceOut_Port.setMax(outDevices.size() - 1);

		// in
		deviceIn_Api = _apiEnum;
		deviceIn_ApiName = str_api;
		if (inDevices.size() > deviceIn_Port)
			deviceIn_PortName = inDevices[deviceIn_Port].name;

		// out
		deviceOut_Api = _apiEnum;
		deviceOut_ApiName = str_api;
		if (outDevices.size() > deviceOut_Port)
			deviceOut_PortName = outDevices[deviceOut_Port].name;

		//--

		buildHelpInfo();
	}

private:

	//--------------------------------------------------------------
	void drawImGui()
	{
		if (!bGui) return;

		guiManager.begin();
		{
			if (guiManager.beginWindowSpecial(bGui_Main))
			{
				//guiManager.AddLabelBig(bGui_Main.getName());
				//guiManager.AddGroup(params_Control);

				guiManager.Add(guiManager.bMinimize, OFX_IM_TOGGLE_ROUNDED);
				guiManager.AddSpacing();

				guiManager.Add(bEnableAudio, OFX_IM_TOGGLE_BIG);
				guiManager.AddSpacing();

				guiManager.Add(bGui_In, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				guiManager.Add(bGui_Out, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				guiManager.Add(bGui_Waveform, OFX_IM_TOGGLE_ROUNDED_MEDIUM);
				guiManager.AddSpacing();

				guiManager.Add(textBoxWidget.bGui, OFX_IM_TOGGLE_ROUNDED);
				guiManager.Add(bGui_Internal, OFX_IM_TOGGLE_ROUNDED_MINI);
				guiManager.endWindowSpecial();
			}
			if (guiManager.beginWindowSpecial(bGui_In))
			{
				//guiManager.AddLabelBig(bGui_In.getName());
				guiManager.AddGroup(params_In);
				guiManager.endWindowSpecial();
			}

			if (guiManager.beginWindowSpecial(bGui_Out))
			{
				//guiManager.AddLabelBig(bGui_Out.getName());
				guiManager.AddGroup(params_Out);
				guiManager.endWindowSpecial();
			}

			if (guiManager.beginWindowSpecial(bGui_Waveform))
			{
				//guiManager.AddLabelBig(bGui_Waveform.getName());
				guiManager.AddGroup(params_Waveform);
				guiManager.endWindowSpecial();
			}
		}
		guiManager.end();
	}

public:

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

private:

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
		helpInfo = "";

		// Log
		helpInfo += "\n> SETUP DEVICE\n\n";
		helpInfo += "\nSAMPLERATE  : " + ofToString(sampleRate);
		helpInfo += "\nBUFFERSIZE  : " + ofToString(bufferSize);
		helpInfo += "\nNUM BUFFERS : " + ofToString(numBuffers);
		helpInfo += "\nNUM INPUTS  : " + ofToString(numInputs);
		helpInfo += "\nNUM OUTPUTS : " + ofToString(numOutputs) + "\n";

		//------------------

		if (!guiManager.bMinimize)
		{
			helpInfo += "\n> LIST AUDIO DEVICES FROM ALL APIS\n\n";
		}

		// List all APIS

		// WASAPI
		if (apiIndex_oF == 7 || !guiManager.bMinimize) // MS_WASAPI
		{
			helpInfo += "WASAPI \n\n";
			auto devicesWs = inStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
			for (auto dev : devicesWs)
			{
				helpInfo += ofToString(dev);
				helpInfo += "\n";
			}
			helpInfo += "\n";
		}

		// ASIO
		if (apiIndex_oF == 8 || !guiManager.bMinimize) // MS_ASIO
		{
			helpInfo += "ASIO \n\n";
			auto devicesAsio = inStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
			for (auto dev : devicesAsio)
			{
				helpInfo += ofToString(dev);
				helpInfo += "\n";
			}
			helpInfo += "\n";
		}

		// DS
		if (apiIndex_oF == 9 || !guiManager.bMinimize) // MS_DS
		{
			helpInfo += "DS \n\n";
			auto devicesDs = inStream.getDeviceList(ofSoundDevice::Api::MS_DS);
			for (auto dev : devicesDs)
			{
				helpInfo += ofToString(dev);
				helpInfo += "\n";
			}
			helpInfo += "\n";
		}

		//-

		if (!guiManager.bMinimize)
		{
			helpInfo += "------------------------------------------";
			helpInfo += "\n\n> CONNECTED DEVICE\n\n";
		}

		helpInfo += "API    \n" + str_api + "\n";

		helpInfo += "Input  " + ofToString(deviceIn_Port) + "\n";
		helpInfo += deviceIn_PortName;
		helpInfo += "\n";

		helpInfo += "Output " + ofToString(deviceOut_Port) + "\n";
		helpInfo += deviceOut_PortName;
		helpInfo += "\n";

		if (!guiManager.bMinimize)
		{
			helpInfo += "------------------------------------------";
			helpInfo += "\n\n\n";
		}

		//--

		textBoxWidget.setText(helpInfo);
	}

private:

	//--------------------------------------------------------------
	void startup()
	{
		// Startup
		// Load Settings
		loadGroup(params, pathSettings);
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
		if (deviceIn_Enable) return deviceIn_Volume;
		else return 0.0f;
	}

	//--------------------------------------------------------------
	float getVolumeOutput()
	{
		if (deviceOut_Enable) return deviceOut_Volume;
		else return 0.0f;
	}

	//--------------------------------------------------------------
	void setPosition(glm::vec2 _position)
	{
		position = _position;
	}

	//--------------------------------------------------------------
	void loadGroup(ofParameterGroup& g, string path)
	{
		ofLogNotice(__FUNCTION__) << "loadGroup: " << g.getName() << " to " << path;
		ofLogVerbose(__FUNCTION__) << "parameters: " << g.toString();
		ofXml settings;
		settings.load(path);
		ofDeserialize(settings, g);
	}

	//--------------------------------------------------------------
	void saveGroup(ofParameterGroup& g, string path)
	{
		ofLogNotice(__FUNCTION__) << "saveGroup: " << g.getName() << " to " << path;
		ofLogVerbose(__FUNCTION__) << "parameters: " << g.toString();
		ofXml settings;
		ofSerialize(settings, g);
		settings.save(path);
	}

	//-

private:

#define SIZE_BUFFER 4096

	float waveformInput[SIZE_BUFFER]; //make this bigger, just in case
	int waveInputIndex = 0;
	float waveformOutput[SIZE_BUFFER]; //make this bigger, just in case
	int waveOutputIndex = 0;

	float smoothedVolume_Input = 0;//rms signal to use on VU
	float smoothedVolume_Out = 0;//rms signal to use on VU

	//--------------------------------------------------------------
	void drawWaveforms()
	{
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
				// bars style

				int __spread = W_Spread * bufferSize;
				__spread = ofClamp(__spread / 20, 1, bufferSize);

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

						if (W_bLine) ofDrawLine(x, 0, x, -_ph);//line
						if (W_bRectangle) ofDrawRectangle(x, 0, _pw, -_ph);//rectangle
					}
				}

				// horizontal line
				ofSetLineWidth(1);
				ofDrawLine(0, 0, ofGetWidth(), 0);

				ofDrawBitmapStringHighlight("INPUT ", ofGetWidth() - _margin, 5);
			}

			//--

			//TODO: WIP:
			if (bGui_WaveOut)
			{
				// output
				ofTranslate(0, 2 * ofGetHeight() / 4);
				ofSetLineWidth(W_LineWidth);

				ofDrawLine(0, 0, 1, waveformOutput[1] * _max); // first line

				for (int i = 1; i < (ofGetWidth() - 1); ++i) {
					ofDrawLine(i, waveformOutput[i] * _max, i + 1, waveformOutput[i + 1] * _max);
				}

				// horizontal line
				ofSetLineWidth(1);
				ofDrawLine(0, 0, ofGetWidth(), 0);

				ofDrawBitmapStringHighlight("OUTPUT", ofGetWidth() - _margin, +5);
			}
		}
		ofPopMatrix();
		ofPopStyle();
	}

public:

	//--------------------------------------------------------------
	void audioIn(ofSoundBuffer& input) {

		std::size_t nChannels = input.getNumChannels();

		if (deviceIn_Enable)
		{
			// vu
			float rms = 0.0;
			int numCounted1 = 0;

			for (size_t i = 0; i < input.getNumFrames(); i++)
			{
				waveformInput[waveInputIndex] = input[i * nChannels]
					* getVolumeInput();

				if (waveInputIndex < (ofGetWidth() - 1)) {
					++waveInputIndex;
				}
				else {
					waveInputIndex = 0;
				}

				//-

				// vu
				// code from here: https://github.com/edap/examplesOfxMaxim
				// rms calculation as explained here http://openframeworks.cc/ofBook/chapters/sound.html
				float left = input[0];
				float right = input[1];
				rms += left * left;
				rms += right * right;
				numCounted1 += 2;
				//numCounted1 += 1;//1 channel
			}

			//--

			// vu
			rms /= (float)numCounted1;
			rms = sqrt(rms);

			smoothedVolume_Input = rms * deviceIn_Volume;
		}
		else // erase plot
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
	void audioOut(ofSoundBuffer& output) {
		std::size_t outChannels = output.getNumChannels();

		if (deviceOut_Enable)
		{
			float rms = 0.0;
			int numCounted = 0;

			rms /= (float)numCounted;
			rms = sqrt(rms);

			smoothedVolume_Out = rms * deviceOut_Volume;
		}
		else // erase plot
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

private:

	//--------------------------------------------------------------
	void Changed_params_Control(ofAbstractParameter& e)
	{
		//return;

		// hardcoded

		if (bDISABLE_CALBACKS) return;

		string name = e.getName();

		if (0) {}

		// MS Windows sound APIs
		else if (name == guiManager.bMinimize.getName())
		{
			buildHelpInfo();

			return;
		}

		else if (name == bEnableAudio.getName())
		{
			//return;

			if (bEnableAudio)
			{

			}
			else
			{
				close();
			}

			return;
		}

		//TODO:
		// add macOS + Linux

		else if (name == apiIndex_Windows.getName())
		{
			switch (apiIndex_Windows)
			{
			case 0:
				apiIndex_oF = 7; // MS_WASAPI
				break;

			case 1:
				apiIndex_oF = 8; // MS_ASIO
				break;

			case 2:
				apiIndex_oF = 9; // MS_DS
				break;

			default:
				ofLogError(__FUNCTION__) << "Error: apiIndex_oF: " << apiIndex_oF << endl;
				break;
			}
			ofLogVerbose(__FUNCTION__) << "apiIndex_oF:" << apiIndex_oF;

			connectToSoundAPI(apiIndex_oF);

			return;
		}
	}

	//-

	//--------------------------------------------------------------
	void Changed_params_In(ofAbstractParameter& e)
	{
		//return;

		if (bDISABLE_CALBACKS) return;

		string name = e.getName();

		if (0) {}

		//else if (name == deviceIn_Enable.getName())
		//{
		//	if (deviceIn_Port)
		//	{
		//		inStream.close();
		//	}
		//	else
		//	{
		//		inStream.close();
		//		deviceIn_PortName = "";
		//	}

		//	return;
		//}

		else if (name == deviceIn_Port.getName())
		{
			inStream.close();
			inSettings.setInDevice(inDevices[deviceIn_Port]);
			inStream.setup(inSettings);

			if (inDevices.size() > deviceIn_Port)
				deviceIn_PortName = inDevices[deviceIn_Port].name;

			return;
		}

		////TODO:
		//if (name == apiIndex_Windows.getName()) {
		//	connectToSoundAPI(apiIndex_Windows + 7);
		//}
	}

	//--------------------------------------------------------------
	void Changed_params_Out(ofAbstractParameter& e)
	{
		//return;

		if (bDISABLE_CALBACKS) return;

		string name = e.getName();
		
		if (0) {}

		//else if (name == deviceOut_Enable.getName())
		//{
		//	return;

		//	if (deviceOut_Enable)
		//	{
		//		deviceOut_Port = deviceOut_Port;//refresh
		//	}
		//	else
		//	{
		//		outStream.close();
		//		deviceOut_PortName = "";
		//	}

		//	return;
		//}

		else if (name == deviceOut_Port.getName())
		{
			outStream.close();
			if (outDevices.size() > deviceOut_Port)
				outSettings.setOutDevice(outDevices[deviceOut_Port]);
			outStream.setup(outSettings);

			if (outDevices.size() > deviceOut_Port)
				deviceOut_PortName = outDevices[deviceOut_Port].name;

			return;
		}

		////TODO:
		//if (name == apiIndex_Windows.getName()) {
		//	connectToSoundAPI(apiIndex_Windows + 7);
		//}
	}

	//--------------------------------------------------------------
	void Changed_params_Waveform(ofAbstractParameter& e)
	{
		//return;

		if (bDISABLE_CALBACKS) return;

		string name = e.getName();
		
		if (0) {}

		else if (name == W_bReset.getName() && W_bReset) {
			W_bReset = false;

			W_WidthGap = 1;
			W_Spread = 0;
			W_Height = 500;
			W_bRectangle = false;
			W_bLine = true;
			W_bAbs = true;
			W_LineWidth = 3;
		}

		else if (name == W_bLine.getName()) {
			if (W_bLine) W_bRectangle = false;
			else W_bRectangle = true;
		}

		else if (name == W_bRectangle.getName()) {
			if (W_bRectangle) W_bLine = false;
			else W_bLine = true;
		}
	}

};

// NOTES
//https://github.com/firmread/ofxFftExamples/blob/master/example-eq/src/ofApp.cpp#L78
//https://forum.openframeworks.cc/t/question-around-void-audioin-ofsoundbuffer-buffer/22284/2
//https://forum.openframeworks.cc/t/openframeworks-and-speech-synthesis-tts-with-flite/12117/7
//https://forum.openframeworks.cc/t/playing-audio-input-in-real-time-problems-syncing/29180/2?u=moebiussurfing
//http://openframeworks.cc/documentation/sound/ofSoundBuffer/#!show_resampleTo
//https://forum.openframeworks.cc/t/listen-to-ofevent-audioin-in-custom-class/27710/3?u=moebiussurfing