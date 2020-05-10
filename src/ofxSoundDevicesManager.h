#pragma once

///TODO:
///+ store/recall settings
///+ switch api/device without exceptions
///+ add macOS/linux apis?

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxSimpleFloatingGui.h"
#include "ofxTextFlow.h"
//#include "ofxXmlSettings.h"

class ofxSoundDevicesManager : public ofBaseApp {
public:
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

private:
	int inDeviceIndex;
	int outDeviceIndex;

	std::vector<ofSoundDevice> inDevices;
	std::vector<ofSoundDevice> outDevices;

	std::vector<string> inDevicesNames;
	std::vector<string> outDevicesNames;

	ofSoundStreamSettings inSettings;
	ofSoundStreamSettings outSettings;

	std::vector<string> ApiNames;
	int apiIndex = 0;

	//-

private:
	//gui
	DropDown GUI_inDevices;
	DropDown GUI_outDevices;
	DropDown GUI_Api;

	ofColor backgroundColor;
	ofColor outlineColor;
	ofColor contentColor;
	int drawCounter;

	// font
	ofTrueTypeFont fontSmall;
	ofTrueTypeFont fontMedium;
	ofTrueTypeFont fontBig;

	//-

	//parameters
public:
	//in
	ofParameterGroup params_In;
	ofParameter<bool> deviceIn_Connect;
	ofParameter<int> deviceIn_Api;
	ofParameter<string> deviceIn_ApiName;
	ofParameter<int> deviceIn_Port;
	ofParameter<string> deviceIn_PortName;

	//out
	ofParameterGroup params_Out;
	ofParameter<bool> deviceOut_Connect;
	ofParameter<int> deviceOut_Api;
	ofParameter<string> deviceOut_ApiName;
	ofParameter<int> deviceOut_Port;
	ofParameter<string> deviceOut_PortName;

	//-

	ofParameterGroup params;
	ofxPanel gui;

	//--

	ofxSoundDevicesManager()
	{
		//default audio seetings

		sampleRate = 44100;
		bufferSize = 512;
		//sampleRate = 48000;
		//bufferSize = 256;
		//numBuffers = 4;
		numBuffers = 2;

		//-

		//params
		deviceIn_Connect.set("CONNECT", false);
		deviceIn_Api.set("API", 0, 0, 10);
		deviceIn_ApiName.set("", "");
		deviceIn_Port.set("PORT", 0, 0, 10);
		deviceIn_PortName.set("", "");

		deviceOut_Connect.set("CONNECT", false);
		deviceOut_Api.set("API", 0, 0, 10);
		deviceOut_ApiName.set("", "");
		deviceOut_Port.set("PORT", 0, 0, 10);
		deviceOut_PortName.set("", "");

		//-

		params_In.setName("INPUT");
		params_In.add(deviceIn_Connect);
		params_In.add(deviceIn_Api);
		params_In.add(deviceIn_ApiName);
		params_In.add(deviceIn_Port);
		params_In.add(deviceIn_PortName);

		params_Out.setName("OUTPUT");
		params_Out.add(deviceOut_Connect);
		params_Out.add(deviceOut_Api);
		params_Out.add(deviceOut_ApiName);
		params_Out.add(deviceOut_Port);
		params_Out.add(deviceOut_PortName);

		params.setName("AUDIO MANAGER");
		params.add(params_In);
		params.add(params_Out);

		gui.setup();
		gui.add(params);
		gui.setPosition(700, 700);
		//gui.setSize(500, 500);

		//-

		//gui
		backgroundColor = ofColor::black;
		outlineColor = ofColor(128);
		contentColor = ofColor::white;

		drawCounter = 0;
		inDeviceIndex = 0;
		outDeviceIndex = 0;
	};

	~ofxSoundDevicesManager() {};

	//--------------------------------------------------------------

public:

	void update()
	{
		//simple callbacks

		//in
		if (GUI_inDevices.getValueInt() != inDeviceIndex)
		{
			inStream.close();
			inSettings.setInDevice(inDevices[GUI_inDevices.getValueInt()]);
			inStream.setup(inSettings);
		}
		inDeviceIndex = GUI_inDevices.getValueInt();

		//out
		if (GUI_outDevices.getValueInt() != outDeviceIndex)
		{
			outStream.close();
			outSettings.setOutDevice(outDevices[GUI_outDevices.getValueInt()]);
			outStream.setup(outSettings);
		}
		outDeviceIndex = GUI_outDevices.getValueInt();

		//api
		if (GUI_Api.getValueInt() != apiIndex)
		{
			int iEnum;
			int _i = GUI_Api.getValueInt();
			//cout << "GUI_Api.getValueInt() " << _i << endl;

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
				cout << "error: " << iEnum << endl;
				break;
			}
			cout << "iEnum:" << iEnum << endl;

			connectAp(iEnum);
		}
		apiIndex = GUI_Api.getValueInt();
	}


	void connectAp(int _apiIndex)
	{
		string str;

		//-

		//TODO:

		close();

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
		//inSettings.numInputChannels = inDevices[inDeviceIndex].inputChannels;//all

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

		inSettings.setInListener(ofGetAppPtr());//?

		inSettings.setInDevice(inDevices[inDeviceIndex]);
		inStream.setup(inSettings);

		//inStream.setInput(input);

		//-

		//output

		outSettings.bufferSize = bufferSize;
		outSettings.numBuffers = numBuffers;
		outSettings.sampleRate = sampleRate;//set the samplerate of both devices to be the same
		outSettings.numInputChannels = 0;
		outSettings.numOutputChannels = numOutputs;
		//outSettings.numOutputChannels = outDevices[outDeviceIndex].outputChannels;//all

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

		outSettings.setOutListener(ofGetAppPtr());//?

		outSettings.setOutDevice(outDevices[outDeviceIndex]);
		outStream.setup(outSettings);

		//outStream.setOutput(output);

		//-

		str = "API    [ " + str_api + " ]";
		logLine(str);

		str = "Input  [ " + ofToString(inDeviceIndex) + " ]";
		logLine(str);

		str = "Output [ " + ofToString(outDeviceIndex) + " ]";
		logLine(str);

		//-

		str = "------------------------------------------";
		logLine(str);

		//-

		//update params
		deviceIn_Connect = true;
		deviceIn_Api = _apiEnum;
		deviceIn_ApiName = str_api;
		deviceIn_Port = inDeviceIndex;
		deviceIn_PortName = inDevices[deviceIn_Port].name;

		deviceOut_Connect = true;
		deviceOut_Api = _apiEnum;
		deviceOut_ApiName = str_api;
		deviceOut_Port = outDeviceIndex;
		deviceOut_PortName = outDevices[deviceOut_Port].name;
	}
	
	void draw()
	{
		position = glm::vec2(50, 50);
		//position = glm::vec2(780, 50);

		//-

		//gui

		ofPushStyle();

		//anchor pos
		int _x0 = position.x;
		int _y0 = position.y;
		int _w = 500;//space between in/out

		int _x = _x0;
		int _y = _y0;
		int _spacer = 15;

		ofMatrix4x4 translation;
		translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(0, 0));

		////translation = ofMatrix4x4::newTranslationMatrix(ofVec3f(LW, UH + HW + INTW));
		//ofPushMatrix();
		//// translation
		//ofMultMatrix(translation);

		////window outline
		//ofSetLineWidth(2);
		////ofDrawRectangle(0, 0, WW, HW);
		//ofDrawRectangle(_x0, _y0, WW, HW);

		//-

		//api
		_x = _x0;
		_y = _y0;
		ofSetColor(outlineColor);
		fontMedium.drawString("API", _x, _y);
		_y += _spacer;
		GUI_Api.draw(_x, _y, translation);

		//-

		//in
		_x += 145;
		_y = _y0;
		ofSetColor(outlineColor);
		fontMedium.drawString("INPUT", _x, _y);
		_y += _spacer;
		GUI_inDevices.draw(_x, _y, translation);

		//-

		//out
		_x += _w;
		_y = _y0;
		ofSetColor(outlineColor);
		fontMedium.drawString("OUTPUT", _x, _y);
		_y += _spacer;
		GUI_outDevices.draw(_x, _y, translation);

		//-

		drawCounter++;

		ofPopStyle();

		//----

		//gui.draw();
	}

	void close()
	{
		inStream.stop();
		inStream.close();
		deviceIn_Connect = false;

		outStream.stop();
		outStream.close();
		deviceOut_Connect = false;

		ofSoundStreamStop();
		ofSoundStreamClose();
	}

	void setup(int _samplerate, int _buffersize, int _numbuffers)
	{
		sampleRate = _samplerate;
		bufferSize = _buffersize;
		numBuffers = _numbuffers;

		setup();
	}

	void setup() {
		
		//--

		//main setings
	
		////audio seetings

		////sampleRate = 48000;
		//sampleRate = 44100;
		//bufferSize = 512;
		////bufferSize = 256;
		////numBuffers = 4;
		//numBuffers = 2;

		numInputs = 2;
		numOutputs = 2;

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

		str = "------------------------------------------";
		logLine(str);

		str = "\n> CONNECTED SOUND DEVICES\n\n";
		logLine(str);

		//-

		//TODO:
		//hardcoded
		inDeviceIndex = 9;//audio cable
		outDeviceIndex = 4;//tv
		connectAp(9);//MS_DS
		apiIndex = 2;//gui

		//--

		//gui 

		//fonts
		string _font = "fonts/telegrama_render.otf";
		//string _font = "fonts/overpass-mono-regular.otf";
		fontSmall.load(_font, 8);
		fontMedium.load(_font, 10);
		fontBig.load(_font, 12);

		//in
		for (int i = 0; i < inDevices.size(); i++) {
			inDevicesNames.push_back(inDevices[i].name);
		}
		GUI_inDevices.setup(inDevicesNames, inDeviceIndex, fontSmall, outlineColor, ofColor::whiteSmoke);

		//out
		for (int i = 0; i < outDevices.size(); i++) {
			outDevicesNames.push_back(outDevices[i].name);
		}
		GUI_outDevices.setup(outDevicesNames, outDeviceIndex, fontSmall, outlineColor, ofColor::whiteSmoke);

		//api
		GUI_Api.setup(ApiNames, apiIndex, fontSmall, outlineColor, ofColor::whiteSmoke);
	}

	//--------------------------------------------------------------
	void logLine(string s)
	{
		ofxTextFlow::addText(s);
		ofLogNotice(__FUNCTION__) << s;
	}
};

