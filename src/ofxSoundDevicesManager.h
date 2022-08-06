#pragma once

/*

TODO:

	+	draggable rectangle to draw the waveforms
	+	fix api selector. add switch API/device. without exceptions/crashes.

	+	enable output and test with an example
	+	add macOS/Linux apis ? https://github.com/roymacdonald/ofxSoundDeviceManager
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

#ifdef USE_ofBaseApp_Pointer
//--------------------------------------------------------------
class ofxSoundDevicesManager
#endif

#ifndef USE_ofBaseApp_Pointer
	//--------------------------------------------------------------
	class ofxSoundDevicesManager : public ofBaseApp
#endif

{
public:

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
		bGui_Main.set("SOUND DEVICES", true);
		//bGui_Main.set("MAIN", true);

		params_Control.setName("CONTROL");
		params_Control.add(bGui_Main);
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

		params_In.setName("INPUT");
		params_In.add(deviceIn_Enable);
		params_In.add(deviceIn_Volume);
		params_In.add(deviceIn_Port);
		params_In.add(deviceIn_PortName);
		//params_In.add(apiIndex_Windows);
		//params_In.add(deviceIn_Api);
		//params_In.add(deviceIn_ApiName);//labels

		params_Out.setName("OUTPUT");
		params_Out.add(deviceOut_Enable);
		params_Out.add(deviceOut_Volume);
		params_Out.add(deviceOut_Port);
		params_Out.add(deviceOut_PortName);
		//params_Out.add(deviceOut_ApiName);//labels
		//params_Out.add(deviceOut_Api);
		//params_Out.add(apiIndex_Windows);

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
		textBoxWidget.bGui.setName("DEVICES INFO");
		textBoxWidget.setTitle("\nDEVICES INFO");
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

	ofxPanel gui;

	bool bDISABLE_CALBACKS = false;//to avoid callback crashes or to enable only after setup()

	//--

private:

	ofxSurfing_ImGui_Manager guiManager;

	ofxSurfingBoxHelpText textBoxWidget;

	string helpInfo = "";

	//--------------------------------------------------------------
	void logLine(string text)
	{
		helpInfo += text + "\n";
	}

	string pathSettings = "ofxSoundDevicesManager.xml";
	string str_error = "";

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
	// 
	//--------------------------------------------------------------
	void connectToSoundAPI(int _apiIndex)
	{
		close();

		string str;

		//-

		// clean
		ofSoundStreamSettings _settings;
		inSettings = _settings;
		outSettings = _settings;

		//----

		// supported APIs by oF

		enum Api
		{
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
		// API choice
		// 
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

		// APIs and devices

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

public:

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
	// devices
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

	//--

private:

	//--------------------------------------------------------------
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

	//--

public:

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
		// For MS Windows only.

		//TODO:
		// add macOS + Linux

		ApiNames.clear();
		ApiNames.push_back("MS_WASAPI");//0
		ApiNames.push_back("MS_ASIO");//1
		ApiNames.push_back("MS_DS");//2

		//--

		setupHelpInfo();


		//--

		bDISABLE_CALBACKS = false;

		//-

		// Internal Gui
		gui.setup("DEVICES");
		gui.add(params);
		gui.getGroup("ofxSoundDevicesManager").minimizeAll();
		gui.getGroup("ofxSoundDevicesManager").minimize();

		// Advanced
		gui.setPosition(700, 400);

		//--

		setupGui();

		//--

		// API

		// TODO:
		// Hardcoded
		apiIndex_Windows = 2; // for the gui?

		//TODO:
		apiIndex_oF = 9; // API #9 on oF is Windows MS DS
		connectToSoundAPI(apiIndex_oF);//MS_DS
		//connectToSoundAPI(_app, apiIndex_oF);//MS_DS

		//--

		// Startup
		// Load Settings
		loadGroup(params, pathSettings);
	}

private:

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
		else return 0.0;
	}

	//--------------------------------------------------------------
	float getVolumeOutput()
	{
		if (deviceOut_Enable) return deviceOut_Volume;
		else return 0.0;
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

private:

#define SIZE_BUFFER 4096

	float waveformInput[SIZE_BUFFER]; //make this bigger, just in case
	int waveInputIndex;
	float waveformOutput[SIZE_BUFFER]; //make this bigger, just in case
	int waveOutputIndex;

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
		else //erase plot
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
	void Changed_params_Control(ofAbstractParameter & e)
	{
		//return;//hardcoded

		if (bDISABLE_CALBACKS) return;

		string name = e.getName();

		// MS Windows sound APIs
		if (name == apiIndex_Windows.getName())
		{
			int iEnum;
			switch (apiIndex_Windows)
			{
			case 0:
				iEnum = 7; // MS_WASAPI
				break;
			case 1:
				iEnum = 8; // MS_ASIO
				break;
			case 2:
				iEnum = 9; // MS_DS
				break;
			default:
				ofLogError(__FUNCTION__) << "error: iEnum: " << iEnum << endl;
				break;
			}
			ofLogVerbose(__FUNCTION__) << "iEnum:" << iEnum;

			apiIndex_oF = iEnum;
			connectToSoundAPI(apiIndex_oF);
		}

		//TODO:
		// add macOS + Linux
	}

	//-

	//--------------------------------------------------------------
	void Changed_params_In(ofAbstractParameter & e)
	{
		if (bDISABLE_CALBACKS) return;

		string name = e.getName();

		if (name == deviceIn_Port.getName()) {
			inStream.close();
			inSettings.setInDevice(inDevices[deviceIn_Port]);
			inStream.setup(inSettings);

			if (inDevices.size() > deviceIn_Port)
				deviceIn_PortName = inDevices[deviceIn_Port].name;
		}

		////TODO:
		//if (name == apiIndex_Windows.getName()) {
		//	connectToSoundAPI(apiIndex_Windows + 7);
		//}
	}

	//--------------------------------------------------------------
	void Changed_params_Out(ofAbstractParameter & e)
	{
		if (bDISABLE_CALBACKS) return;

		string name = e.getName();

		if (name == deviceOut_Port.getName()) {
			outStream.close();
			if (outDevices.size() > deviceOut_Port)
				outSettings.setOutDevice(outDevices[deviceOut_Port]);
			outStream.setup(outSettings);

			if (outDevices.size() > deviceOut_Port)
				deviceOut_PortName = outDevices[deviceOut_Port].name;
		}

		////TODO:
		//if (name == apiIndex_Windows.getName()) {
		//	connectToSoundAPI(apiIndex_Windows + 7);
		//}
	}

	//--------------------------------------------------------------
	void Changed_params_Waveform(ofAbstractParameter& e)
	{
		if (bDISABLE_CALBACKS) return;

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
	}

	//TODO:
	//--------------------------------------------------------------
	void drawVU(float val, int x, int y, int w, int h)
	{
		ofPushStyle();

		ofNoFill();
		ofSetColor(ofColor::white);
		ofDrawRectangle(x, y - h, w, h);

		ofFill();
		ofSetColor(ofColor::white);
		ofDrawRectangle(x, y - h, val * w, h);

		ofPopStyle();
	}

	};

// NOTES
//https://github.com/firmread/ofxFftExamples/blob/master/example-eq/src/ofApp.cpp#L78
//https://forum.openframeworks.cc/t/question-around-void-audioin-ofsoundbuffer-buffer/22284/2
//https://forum.openframeworks.cc/t/openframeworks-and-speech-synthesis-tts-with-flite/12117/7
//https://forum.openframeworks.cc/t/playing-audio-input-in-real-time-problems-syncing/29180/2?u=moebiussurfing
//http://openframeworks.cc/documentation/sound/ofSoundBuffer/#!show_resampleTo
//https://forum.openframeworks.cc/t/listen-to-ofevent-audioin-in-custom-class/27710/3?u=moebiussurfing