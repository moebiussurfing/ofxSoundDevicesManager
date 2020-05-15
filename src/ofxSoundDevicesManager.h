#pragma once

/// TODO:
/// ++	add disconnectors to use only input or output. now, enablers are only like mutes. 
/// +	add user gui toggle for advanced mode. key to 'G'
/// +	add switch api/device. without exceptions.
/// +	add macOS/linux apis?
/// +	samplerate and other settings to gui selectors. store to xml too. restar required maybe
/// +	store devices by names? just editing xml file bc sorting can change on the system?
/// +	alternative waveforms plotting: https://github.com/Feliszt/sound-analyzer-OF
/// +	better vumeter usign other rms snippets

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxSimpleFloatingGui.h"

#define USE_AUDIO_CALLBACKS

//#define USE_Log //can be commented to avoid ofxTextFlow(cool on window logger) dependecy 
#ifdef USE_Log
#include "ofxTextFlow.h"
#endif

//class ofxSoundDevicesManager : public ofBaseApp {
class ofxSoundDevicesManager {

public:

	string pathSettings = "ofxSoundDevicesManager.xml";
	string str_error = "";

	//ofBaseApp
	ofBaseApp* _app_;

	//-

#ifdef USE_Log
	ofParameter<bool> SHOW_Log;
#endif
	ofParameter<bool> SHOW_Active;
	ofParameter<bool> SHOW_Advanced;

	glm::vec2 position;

	//-

private:
	//sound devices
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

	//api
	int apiOnAllOFSystemsIndex;//all OF possile apis!
	//NOT the index of the available apis on this system.!
	//ie: on this windows system could be: wasapi, asio, ds -> will be 0 to 2
	int apiGuiIndex = 0;//this index will be related to all available apis ONLY on this system!
	//also in your same system, devices will change when disabling on windows sound preferences/devices
	std::vector<string> ApiNames;

	//devices
	std::vector<ofSoundDevice> inDevices;
	std::vector<ofSoundDevice> outDevices;

	std::vector<string> inDevicesNames;
	std::vector<string> outDevicesNames;

	//settings
	ofSoundStreamSettings inSettings;
	ofSoundStreamSettings outSettings;

	//-

private:

	//gui

	//widgets
	DropDown GUI_deviceIndexInput;
	DropDown GUI_deviceIndexOutput;
	DropDown GUI_Api;
	Toggle GUI_enableInput;
	SliderB GUI_volumeInput;
	Toggle GUI_enableOutput;
	SliderB GUI_volumeOutput;

	//layout
	ofColor colorDark;
	ofColor colorGrey;
	ofColor colorWhite;
	int drawCounter;
	int _widgetW;
	int _widgetH;

	//fonts
	ofTrueTypeFont fontSmall;
	ofTrueTypeFont fontMedium;
	ofTrueTypeFont fontBig;

	//-

private:

	//parameters

	//in
	ofParameterGroup params_In;
	ofParameter<bool> deviceIn_Enable;
	ofParameter<float> deviceIn_Volume;
	ofParameter<int> deviceIn_Api;
	ofParameter<string> deviceIn_ApiName;
	ofParameter<int> deviceIn_Port;
	ofParameter<string> deviceIn_PortName;

	//out
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
	bool DISABLE_Callbacks = false;//to avoid callback crashes or to enable only after setup()

	ofxPanel gui;
	bool bSHOW_Gui = false;

	//-

public:

	//api

	void setVisibleGui(bool b)
	{
		bSHOW_Gui = b;
	}
	void toggleVisibleGui()
	{
		bSHOW_Gui = !bSHOW_Gui;
	}
#ifdef USE_Log
	void setVisibleLog(bool b)
	{
		SHOW_Log = b;
	}
	void toggleVisibleLog()
	{
		SHOW_Log = !SHOW_Log;
	}
#endif

	void setActive(bool b)
	{
		SHOW_Active = b;
	}
	void toggleActive()
	{
		SHOW_Active = !SHOW_Active;
	}

	void setVisibleAdvanced(bool b)
	{
		SHOW_Advanced = b;
	}
	void toggleVisibleAdvanced()
	{
		SHOW_Advanced = !SHOW_Advanced;
	}

	//--

	//--------------------------------------------------------------
	ofxSoundDevicesManager()
	{
		params_Control.setName("CONTROL");

		SHOW_Active.set("ACTIVE", true);
		SHOW_Advanced.set("ADVANCED", true);
		params_Control.add(SHOW_Active);
		params_Control.add(SHOW_Advanced);
#ifdef USE_Log
		SHOW_Log.set("LOG", true);
		params_Control.add(SHOW_Log);
#endif
		//-

		DISABLE_Callbacks = true;

		//default audio seetings

		sampleRate = 44100;//internal default
		//sampleRate = 48000;

		bufferSize = 512;
		//bufferSize = 256;//internal default

		numBuffers = 4;//internal default
		//numBuffers = 2;

		//-

		//channels
		numInputs = 2;
		numOutputs = 2;

		//-

		//params
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

		//serializers
		//deviceIn_Enable.setSerializable(false);
		deviceIn_Api.setSerializable(false);
		deviceIn_ApiName.setSerializable(false);
		//deviceIn_Port.setSerializable(false);
		deviceIn_PortName.setSerializable(false);

		//deviceOut_Enable.setSerializable(false);
		deviceOut_Api.setSerializable(false);
		deviceOut_ApiName.setSerializable(false);
		//deviceOut_Port.setSerializable(false);
		deviceOut_PortName.setSerializable(false);

		//-

		//TODO:
		apiOnAllOFSystemsIndex = 9;//api #9 on OF is MS-DS

		//-

		params_In.setName("INPUT");
		params_In.add(deviceIn_Enable);
		params_In.add(deviceIn_Volume);
		params_In.add(deviceIn_Api);
		params_In.add(deviceIn_Port);
		//params_In.add(deviceIn_ApiName);//hide labels
		//params_In.add(deviceIn_PortName);

		params_Out.setName("OUTPUT");
		params_Out.add(deviceOut_Enable);
		params_Out.add(deviceOut_Volume);
		params_Out.add(deviceOut_Api);
		params_Out.add(deviceOut_Port);
		//params_Out.add(deviceOut_ApiName);//hide labels
		//params_Out.add(deviceOut_PortName);

		params.setName("ofxSoundDevicesManager");
		params.add(params_In);
		params.add(params_Out);
		params.add(params_Control);
		ofAddListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);
		ofAddListener(params_Out.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Out);

#ifdef USE_AUDIO_CALLBACKS
		//setupWaveforms();
		//TODO:
		//hide smooth bc must be improved
		//params.add(paramsWaveforms);
		ofAddListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);
#endif

		//-

		//layout

		//user gui
		position = glm::vec2(25, 25);

		//-

		colorDark = ofColor::black;
		colorGrey = ofColor(128);
		colorWhite = ofColor::white;
		drawCounter = 0;

		//-

		gui.setup();
		gui.add(params);

		//advanced gui
		gui.setPosition(700, 400);
	};

	//--------------------------------------------------------------
	~ofxSoundDevicesManager() {
		//exit();
	};

	//--------------------------------------------------------------
	void exit() {
		saveGroup(params, pathSettings);

		ofRemoveListener(params_In.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_In);
		ofRemoveListener(params_Out.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Out);
#ifdef USE_AUDIO_CALLBACKS
		ofRemoveListener(params_Control.parameterChangedE(), this, &ofxSoundDevicesManager::Changed_params_Control);
#endif
		close();
	};

	//----

public:

	//--------------------------------------------------------------
	void updateGuiUser()
	{
		//simple callbacks

		//in

		//enable
		if (GUI_enableInput.getValue() != deviceIn_Enable)
		{
			deviceIn_Enable = GUI_enableInput.getValue();
		}
		//volumen
		if (GUI_volumeInput.getValue() != deviceIn_Volume)
		{
			deviceIn_Volume = GUI_volumeInput.getValue();
		}
		//port
		if (GUI_deviceIndexInput.getValueInt() != deviceIn_Port)
		{
			inStream.close();
			inSettings.setInDevice(inDevices[GUI_deviceIndexInput.getValueInt()]);
			inStream.setup(inSettings);
			deviceIn_Port = GUI_deviceIndexInput.getValueInt();
		}

		//-

		//out

		//enable
		if (GUI_enableOutput.getValue() != deviceOut_Enable)
		{
			deviceOut_Enable = GUI_enableOutput.getValue();
		}

		//volumen
		if (GUI_volumeOutput.getValue() != deviceOut_Volume)
		{
			deviceOut_Volume = GUI_volumeOutput.getValue();
		}

		//port
		if (GUI_deviceIndexOutput.getValueInt() != deviceOut_Port)
		{
			outStream.close();
			outSettings.setOutDevice(outDevices[GUI_deviceIndexOutput.getValueInt()]);
			outStream.setup(outSettings);
			deviceOut_Port = GUI_deviceIndexOutput.getValueInt();
		}

		//-

		//api
		if (GUI_Api.getValueInt() != apiGuiIndex)
		{
			int iEnum;
			int _i = GUI_Api.getValueInt();
			ofLogVerbose(__FUNCTION__) << "GUI_Api.getValueInt() " << _i << endl;

			//windows apis
			switch (_i)
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

			//-

			connectAp(iEnum);
			apiGuiIndex = GUI_Api.getValueInt();
		}
	}

	//--------------------------------------------------------------
	void update()
	{
		//updateWaveforms();//for using alternative plot engines

		//-

		//user gui
		updateGuiUser();
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
			DEFAULT,
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
		//api choice

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

		//apis and devices

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

		//inSettings.setInListener(ofGetAppPtr());//?
		inSettings.setInListener(_app_);
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

		//outSettings.setOutListener(ofGetAppPtr());//?
		outSettings.setOutListener(_app_);
		outSettings.setOutDevice(outDevices[deviceOut_Port]);

		outStream.setup(outSettings);

		//-

		str = "------------------------------------------";
		logLine(str);

		str = "\n> CONNECTED SOUND DEVICES\n\n";
		logLine(str);

		str = "API    [ " + str_api + " ]";
		logLine(str);

		str = "Input  [ " + ofToString(deviceIn_Port) + " ]";
		logLine(str);

		str = "Output [ " + ofToString(deviceOut_Port) + " ]";
		logLine(str);

		str = "------------------------------------------";
		logLine(str);

		//-

		//update params

		//max ports
		deviceIn_Port.setMax(inDevices.size() - 1);
		deviceOut_Port.setMax(outDevices.size() - 1);

		deviceIn_Api = _apiEnum;
		deviceIn_ApiName = str_api;
		deviceIn_PortName = inDevices[deviceIn_Port].name;

		deviceOut_Api = _apiEnum;
		deviceOut_ApiName = str_api;
		deviceOut_PortName = outDevices[deviceOut_Port].name;
	}

	//--------------------------------------------------------------
	void drawGuiUser()
	{
		ofPushStyle();

		//anchor pos
		int _x0 = position.x;//column 0
		int _y0 = position.y;

		int _wColumn = 500;//space between in/out (column 1-2)
		int _spacerX = 60;
		int _spacerY = 20;

		//vu's
		int _ww = 120;
		int _hh = 10;

		int _x, _y;
		int _vuMargin = 30;
		int _wVu;
		string __str;

		ofMatrix4x4 translation;
		translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(_x0, _y0));
		ofPushMatrix();
		ofMultMatrix(translation);

		//--

		//column 1
		_x = 0;
		//row 3
		_y = 6 * _spacerY;

		//settings
		ofSetColor(colorGrey);
		string _str;
		float _strW;
		int _margin = 20;

		_str = "SampleRate [" + ofToString(sampleRate) + "]";
		_strW = fontSmall.getStringBoundingBox(_str, 0, 0).getWidth() + _margin;
		fontSmall.drawString(_str, _x, _y);
		_x += _strW;

		_str = "BufferSize [" + ofToString(bufferSize) + "]";
		_strW = fontSmall.getStringBoundingBox(_str, 0, 0).getWidth() + _margin;
		fontSmall.drawString(_str, _x, _y);
		_x += _strW;

		_str = "InPort [" + ofToString(deviceIn_Port) + "]";
		_strW = fontSmall.getStringBoundingBox(_str, 0, 0).getWidth() + _margin;
		fontSmall.drawString(_str, _x, _y);
		_x += _strW;

		_str = "OutPort [" + ofToString(deviceOut_Port) + "]";
		_strW = fontSmall.getStringBoundingBox(_str, 0, 0).getWidth() + _margin;
		fontSmall.drawString(_str, _x, _y);
		//_x += _strW;

		//---

		//api

		ofSetColor(colorGrey);

		//column 1
		_x = 0;
		//row 1
		_y = 0;

		fontMedium.drawString("API", _x, _y);
		_y += _spacerY;
		GUI_Api.draw(_x, _y, translation);

		//----

		//input

		ofSetColor(colorGrey);

		//column 2
		_x = 145;
		//row 2
		_y = 0;

		//label
		__str = "INPUT";
		fontMedium.drawString(__str, _x, _y);

		//vu
#ifdef USE_AUDIO_CALLBACKS
		_wVu = fontMedium.getStringBoundingBox(__str, 0, 0).getWidth() + _vuMargin;
		drawVU(smoothedVolume_Input, _x + _wVu, _y, _ww, _hh);
#endif
		_y += _spacerY;
		//enable toggle
		GUI_enableInput.draw(_x, _y, translation);
		_x += _spacerX;

		//input volume 
		GUI_volumeInput.draw(_x, _y, translation);//_y + _widgetH*1.f
		_y += _spacerY;

		//-

		//dropdown
		_x = +145;
		_y += _spacerY;
		GUI_deviceIndexInput.draw(_x, _y, translation);

		//---

		//out

		ofSetColor(colorGrey);

		//row 3
		_y = 0;
		//column 3
		_x += _wColumn;

		//label
		__str = "OUTPUT";
		fontMedium.drawString(__str, _x, _y);

		//vu
#ifdef USE_AUDIO_CALLBACKS
		_wVu = fontMedium.getStringBoundingBox(__str, 0, 0).getWidth() + _vuMargin;
		drawVU(smoothedVolume_Out, _x + _wVu, _y, _ww, _hh);
#endif
		_y += _spacerY;
		//enable toggle
		GUI_enableOutput.draw(_x, _y, translation);
		_x += _spacerX;

		//output volume 
		GUI_volumeOutput.draw(_x, _y, translation);
		_y += _spacerY;

		//-

		//dropdown
		_x = +145 + _wColumn;
		_y += _spacerY;
		GUI_deviceIndexOutput.draw(_x, _y, translation);

		//---

		drawCounter++;

		ofPopStyle();
		ofPopMatrix();
	}

	//--------------------------------------------------------------
	void draw()
	{
		if (SHOW_Active)
		{

#ifdef USE_AUDIO_CALLBACKS
			drawWaveforms();
#endif
			//--

			//gui
			drawGuiUser();

			//--

			if (bSHOW_Gui)
				gui.draw();
		}
	}

	//--------------------------------------------------------------
	void close()
	{
		//TODO:
		//inStream.stop();
		//inStream.close();

		//outStream.stop();
		//outStream.close();

		//ofSoundStreamStop();
		//ofSoundStreamClose();
	}

	//settings
	//--------------------------------------------------------------
	//void setup(int _samplerate, int _buffersize, int _numbuffers)
	void setup(ofBaseApp* _app, int _samplerate, int _buffersize, int _numbuffers)
	{
		sampleRate = _samplerate;
		bufferSize = _buffersize;
		numBuffers = _numbuffers;

		setup(_app);
		//setup();
	}

	//--------------------------------------------------------------
	//void setup(int _samplerate, int _buffersize)
	void setup(ofBaseApp* _app, int _samplerate, int _buffersize)
	{
		sampleRate = _samplerate;
		bufferSize = _buffersize;

		setup(_app);
		//setup();
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

//--------------------------------------------------------------
	//void setup() {
	void setup(ofBaseApp* _app) {

		_app_ = _app;

		//--

		string _str;
		string str;

		//log
		_str = "\n> SETUP SOUND DEVICES\n\n";
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

		//-

		//TODO:
		//windows only
		ApiNames.clear();
		ApiNames.push_back("MS_WASAPI");//0
		ApiNames.push_back("MS_ASIO");//1
		ApiNames.push_back("MS_DS");//2

		//------------------

		//_str = "> SETUP SOUND DEVICES";
		//logLine(_str);

		str = "\n> LIST AUDIO DEVICES FROM ANY API\n\n";
		logLine(str);

		//list all apis
		auto devicesAsio = inStream.getDeviceList(ofSoundDevice::Api::MS_ASIO);
		for (auto dev : devicesAsio)
		{
			str = ofToString(dev);
			logLine(str);
		}
		str = "\n";
		logLine(str);

		auto devicesDs = inStream.getDeviceList(ofSoundDevice::Api::MS_DS);
		for (auto dev : devicesDs)
		{
			str = ofToString(dev);
			logLine(str);
		}

		str = "\n";
		logLine(str);

		auto devicesWs = inStream.getDeviceList(ofSoundDevice::Api::MS_WASAPI);
		for (auto dev : devicesWs)
		{
			str = ofToString(dev);
			logLine(str);
		}

		str = "\n";
		logLine(str);

		//-

		DISABLE_Callbacks = false;

		//settings
		loadGroup(params, pathSettings);

		//-

		//api

		//connectAp(_app, apiOnAllOFSystemsIndex);//MS_DS
		connectAp(apiOnAllOFSystemsIndex);//MS_DS

		//TODO:
		//harcoded
		apiGuiIndex = 2;//for gui

		//--

		setupGuiUser();

#ifdef USE_Log
		ofxTextFlow::setShowing(SHOW_Log);
#endif

	}

	//--------------------------------------------------------------
	void setupGuiUser()
	{
		//--

		//gui 

		//fonts
		string _font = "fonts/telegrama_render.otf";
		fontSmall.load(_font, 8);
		fontMedium.load(_font, 10);
		fontBig.load(_font, 12);

		//dropdowns

		//1. api
		GUI_Api.setup(ApiNames, apiGuiIndex, fontSmall, colorGrey, ofColor::whiteSmoke);

		//2. in
		for (int i = 0; i < inDevices.size(); i++) {
			inDevicesNames.push_back(inDevices[i].name);
		}
		GUI_deviceIndexInput.setup(inDevicesNames, deviceIn_Port, fontSmall, colorGrey, ofColor::whiteSmoke);

		//3. out
		for (int i = 0; i < outDevices.size(); i++) {
			outDevicesNames.push_back(outDevices[i].name);
		}
		GUI_deviceIndexOutput.setup(outDevicesNames, deviceOut_Port, fontSmall, colorGrey, ofColor::whiteSmoke);

		//-

		_widgetW = 200;
		_widgetH = 15;

		GUI_enableInput.setup(deviceIn_Enable, colorDark);
		GUI_volumeInput.setup(deviceIn_Volume, _widgetW, _widgetH, _widgetH*.7f, colorDark, colorGrey);
		GUI_enableOutput.setup(deviceOut_Enable, colorDark);
		GUI_volumeOutput.setup(deviceOut_Volume, _widgetW, _widgetH, _widgetH*.7f, colorDark, colorGrey);
	}

	//--------------------------------------------------------------
	float getVolumeInput()
	{
		if (GUI_enableInput.getValue())
			return GUI_volumeInput.getValue();
		else
			return 0.0;
	}

	//--------------------------------------------------------------
	float getVolumeOutput()
	{
		if (GUI_enableOutput.getValue())
			return GUI_volumeOutput.getValue();
		else
			return 0.0;
	}

	//--------------------------------------------------------------
	void setPosition(glm::vec2 _position)
	{
		position = _position;
	}

	//--------------------------------------------------------------
	void loadGroup(ofParameterGroup &g, string path)
	{
		ofLogNotice(__FUNCTION__) << "loadGroup: " << g.getName() << " to " << path;
		ofLogVerbose(__FUNCTION__) << "parameters: " << g.toString();
		ofXml settings;
		settings.load(path);
		ofDeserialize(settings, g);

		//-

		//startup

		//mirror variables that could not be 'loaded'

		//in
		GUI_enableInput.setValue(deviceIn_Enable);
		GUI_volumeInput.setValue(deviceIn_Volume);

		//out
		GUI_enableOutput.setValue(deviceOut_Enable);
		GUI_volumeOutput.setValue(deviceOut_Volume);
	}

	//--------------------------------------------------------------
	void saveGroup(ofParameterGroup &g, string path)
	{
		ofLogNotice(__FUNCTION__) << "saveGroup: " << g.getName() << " to " << path;
		ofLogVerbose(__FUNCTION__) << "parameters: " << g.toString();
		ofXml settings;
		ofSerialize(settings, g);
		settings.save(path);
	}

	//-

#ifdef USE_AUDIO_CALLBACKS

	float waveformInput[4096]; //make this bigger, just in case
	int waveInputIndex;
	float waveformOutput[4096]; //make this bigger, just in case
	int waveOutputIndex;

	//alternative plot 1
	//rms, vu's
	//ofParameter<bool> enableSmooth;//enable smooth VU
	//ofParameter<float> smoothRatio;
	float smoothedVolume_Input = 0;//rms signal to use on VU
	float smoothedVolume_Out = 0;//rms signal to use on VU
	ofParameterGroup paramsWaveforms;
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

		ofFill();
		ofSetColor(0, 225);
		ofSetLineWidth(3.0f);
		float _max = 200;
		int _margin = 50;

		//input
		ofTranslate(0, ofGetHeight() / 4.f);
		ofDrawLine(0, 0, 1, waveformInput[1] * _max); //first line
		for (int i = 1; i < (ofGetWidth() - 1); ++i) {
			ofDrawLine(i, waveformInput[i] * _max, i + 1, waveformInput[i + 1] * _max);
		}
		ofDrawBitmapStringHighlight("INPUT ", ofGetWidth() - _margin, +5);

		//output
		ofTranslate(0, 2 * ofGetHeight() / 4);
		ofDrawLine(0, 0, 1, waveformOutput[1] * _max); //first line
		for (int i = 1; i < (ofGetWidth() - 1); ++i) {
			ofDrawLine(i, waveformOutput[i] * _max, i + 1, waveformOutput[i + 1] * _max);
		}
		ofDrawBitmapStringHighlight("OUTPUT", ofGetWidth() - _margin, +5);

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

		ofPopMatrix();
		ofPopStyle();
	}

	//--------------------------------------------------------------
	void audioIn(ofSoundBuffer& input) {

		std::size_t nChannels = input.getNumChannels();

		if (deviceIn_Enable)
		{
			//vu
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
	void audioOut(ofSoundBuffer& output) {
		std::size_t outChannels = output.getNumChannels();

		if (deviceOut_Enable)
		{
			//vu
			float rms = 0.0;
			int numCounted = 0;

			for (size_t i = 0; i < output.getNumFrames(); ++i)
			{
				output[i * outChannels] = ofRandom(-1, 1)
					* getVolumeOutput()
					;
				output[i * outChannels + 1] = output[i * outChannels];

				waveformOutput[waveOutputIndex] = output[i * outChannels];
				if (waveOutputIndex < (ofGetWidth() - 1)) {
					++waveOutputIndex;
				}
				else {
					waveOutputIndex = 0;
				}

				//-

				//vu
				//code from here: https://github.com/edap/examplesOfxMaxim
				//rms calculation as explained here http://openframeworks.cc/ofBook/chapters/sound.html
				float left = output[0];
				float right = output[1];
				rms += left * left;
				rms += right * right;
				numCounted += 2;
			}

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
	void Changed_params_Control(ofAbstractParameter &e)
	{
		if (!DISABLE_Callbacks)
		{
			string name = e.getName();

#ifdef USE_Log
			if (name == "LOG")
			{
				ofxTextFlow::setShowing(SHOW_Log);
			}
#endif
			if (name == "ACTIVE")
			{
				if (SHOW_Active)
				{

				}
				else
				{
#ifdef USE_Log
					SHOW_Log = false;
#endif

					//bSHOW_Gui = false;
				}
			}
			if (name == "ADVANCED")
			{
				bSHOW_Gui = SHOW_Advanced;
			}
		}
				}

#endif // USE_AUDIO_CALLBACKS

	//-

	//--------------------------------------------------------------
	void Changed_params_In(ofAbstractParameter &e)
	{
		if (!DISABLE_Callbacks)
		{
			string name = e.getName();

			if (name == "ENABLE")
			{
				GUI_enableInput.setValue(deviceIn_Enable);
			}
			else if (name == "VOLUME")
			{
				GUI_volumeInput.setValue(deviceIn_Volume);
			}
			else if (name == "PORT")
			{
				GUI_deviceIndexInput.setValueInt(deviceIn_Port);
			}
		}
	}

	//--------------------------------------------------------------
	void Changed_params_Out(ofAbstractParameter &e)
	{
		if (!DISABLE_Callbacks)
		{
			string name = e.getName();

			if (name == "ENABLE")
			{
				GUI_enableOutput.setValue(deviceOut_Enable);
			}
			else if (name == "VOLUME")
			{
				GUI_volumeOutput.setValue(deviceOut_Volume);
			}
			else if (name == "PORT")
			{
				GUI_deviceIndexOutput.setValueInt(deviceOut_Port);
			}
		}
	}

#ifdef USE_AUDIO_CALLBACKS
	//--------------------------------------------------------------
	void drawVU(float val, int x, int y, int w, int h)
	{
		ofPushStyle();

		ofSetColor(colorGrey);
		ofNoFill();
		ofDrawRectangle(x, y - h, w, h);

		ofSetColor(colorWhite);
		ofFill();
		ofDrawRectangle(x, y - h, val*w, h);

		ofPopStyle();
	}
#endif

	//--------------------------------------------------------------
	void logLine(string s)
	{
#ifdef USE_Log
		if (SHOW_Log)
		{
			ofxTextFlow::addText(s);
			}
#endif
		ofLogNotice(__FUNCTION__) << s;
		}

	};