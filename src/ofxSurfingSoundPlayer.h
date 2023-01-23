
/*

	TODO

	add fade in to avoid clicks when dragging position

*/


//--

#pragma once

#include "ofMain.h"

#include "ofxSurfingHelpers.h"
#include "ofxSurfingImGui.h"

class ofxSurfingSoundPlayer
{
public:

	ofxSurfingSoundPlayer::ofxSurfingSoundPlayer() {
	};

	ofxSurfingSoundPlayer::~ofxSurfingSoundPlayer() {
		exit();
	};

	//----

	// Fix noises when scrubbing
	// adding fades

private:

	enum PlayerState_
	{
		PlayerState_STOP_OUT = 0,
		PlayerState_STOPPED,
		PlayerState_PLAY_IN,
		PlayerState_PLAYING,
		PlayerState_PLAY_OUT,
		PlayerState_PAUSED,
		PlayerState_AMOUNT,
	};

	PlayerState_ playerState;
	PlayerState_ playerState_PRE = PlayerState_(-1);

	float fadeVal = 0;
	float fadeVal_PRE = -1;
	float fadeStep = 0.05;

	//----

private:

	string path_GLOBAL = "ofxSurfingSoundPlayer";
	string path_AppSettings = "ofxSurfingSoundPlayer.json";

	ofSoundStreamSettings outSettings;
	ofSoundStream outStream;
	std::vector<ofSoundDevice> outDevices;

	bool bLoaded = false;

	//--

public:

	ofParameter<bool> bGui;

	ofParameter<string> path{ "Path", "" };

private:

	ofParameterGroup params{ "PLAYER" };

	ofParameterGroup params_Audio{ "AUDIO" };
	ofParameter<float> volume{ "Volume", 0.5, 0, 1 };
	ofParameter<bool> bPlay{ "PLAY", false };
	ofParameter<void> bStop{ "STOP" };
	ofParameter<bool> bLoop{ "Loop", false };
	ofParameter<float> position{ "Position", 0, 0, 1 };

	string name_Audio = "NO FILE";

	bool bStopped = true;

public:

	void setup()
	{
		setup_Audio();

		ofAddListener(params.parameterChangedE(), this, &ofxSurfingSoundPlayer::Changed_Params);

		startup();
	};

private:

	void startup() {
		ofxSurfingHelpers::loadGroup(params, path_GLOBAL + "/" + path_AppSettings);
	};

	void setup_Audio()
	{
		bGui.set("PLAYER", true);

		params_Audio.add(bGui);
		params_Audio.add(path);
		params_Audio.add(bPlay);
		params_Audio.add(bStop);
		params_Audio.add(bLoop);
		params_Audio.add(position);
		params_Audio.add(volume);

		params.add(params_Audio);

		// to starts at zero always!
		//position.setSerializable(false);

		//--

		//TODO:
		//add device selector into ofxSoundDeviceManager

		// hardcoded device
		int deviceOut_Port = 0;

		outDevices.clear();
		outDevices = outStream.getDeviceList(ofSoundDevice::Api::MS_DS);
		ofLogNotice("ofxSurfingSoundPlayer") << outDevices;

		size_t sampleRate = 48000;
		size_t bufferSize = 256;
		size_t numBuffers = 4;
		size_t numInputChannels = 0;
		size_t numOutputChannels = 2;

		outSettings.bufferSize = bufferSize;
		outSettings.numBuffers = numBuffers;
		outSettings.sampleRate = sampleRate;
		outSettings.numInputChannels = numInputChannels;
		outSettings.numOutputChannels = numOutputChannels;
		outSettings.setOutListener(ofGetAppPtr());
		outSettings.setOutDevice(outDevices[deviceOut_Port]);

		outStream.setup(outSettings);
	};

public:

	void update()
	{
		if (!bLoaded) return;

		//--

		// Fade volume transitions
		// to avoid glitches
		{
			// Sate changed
			if (playerState != playerState_PRE)
			{
				playerState_PRE = playerState;

				switch (playerState)
				{
				case ofxSurfingSoundPlayer::PlayerState_STOP_OUT:
					break;
				case ofxSurfingSoundPlayer::PlayerState_STOPPED:
					fadeVal = 0;
					break;
				case ofxSurfingSoundPlayer::PlayerState_PLAY_IN:
					fadeVal = 0;
					break;
				case ofxSurfingSoundPlayer::PlayerState_PLAYING:
					break;
				case ofxSurfingSoundPlayer::PlayerState_PLAY_OUT:
					break;
				case ofxSurfingSoundPlayer::PlayerState_PAUSED:
					break;
				case ofxSurfingSoundPlayer::PlayerState_AMOUNT:
					break;
				}
			}

			// Update on states
			switch (playerState)
			{
			case ofxSurfingSoundPlayer::PlayerState_STOP_OUT:
				fadeVal -= fadeStep;
				fadeVal = ofClamp(fadeVal, 0, 1);
				break;
			case ofxSurfingSoundPlayer::PlayerState_STOPPED:
				fadeVal = 0;
				break;
			case ofxSurfingSoundPlayer::PlayerState_PLAY_IN:
				fadeVal += fadeStep;
				fadeVal = ofClamp(fadeVal, 0, 1);
				break;
			case ofxSurfingSoundPlayer::PlayerState_PLAYING:
				break;
			case ofxSurfingSoundPlayer::PlayerState_PLAY_OUT:
				fadeVal -= fadeStep;
				fadeVal = ofClamp(fadeVal, 0, 1);
				break;
			case ofxSurfingSoundPlayer::PlayerState_PAUSED:
				break;
			case ofxSurfingSoundPlayer::PlayerState_AMOUNT:
				break;
			}

			if (fadeVal != fadeVal_PRE) {
				fadeVal_PRE = fadeVal;
				playerAudio.setVolume(fadeVal * volume.get());
			}
		}

		//--

		if (position != playerAudio.getPosition())
		{
			position.setWithoutEventNotifications(playerAudio.getPosition());
		}

		ofSoundUpdate();
	};

private:

	void exit() {
		outStream.close();

		ofRemoveListener(params.parameterChangedE(), this, &ofxSurfingSoundPlayer::Changed_Params);

		ofxSurfingHelpers::CheckFolder(path_GLOBAL);
		ofxSurfingHelpers::saveGroup(params, path_GLOBAL + "/" + path_AppSettings);
	};

	//--

public:

	// Setters

	void setPaused(bool b) {
		playerAudio.setPaused(b);
	};
	void setPosition(float pos) {
		playerAudio.setPosition(pos);
	};
	void setVolume(float vol) {
		playerAudio.setVolume(vol);
	};
	void setSpeed(float speed) {
		playerAudio.setSpeed(speed);
	};
	void setLoopState(ofLoopType state) {
		playerAudio.setLoop(state);
	};

	void setVolumenUp()
	{
		volume = MIN(volume + 0.1, 1);
	};
	void setVolumenDown()
	{
		volume = MAX(volume - 0.1, 0);
	};

	//--

	// Getters

	float getPositionAudio() const {
		return playerAudio.getPosition();
	};
	int getPositionAudioMS() const {
		return playerAudio.getPositionMS();
	};
	bool isLoadedAudio() const {
		return playerAudio.isLoaded();
	};
	//float getDurationAudio() const {
	//	return playerAudio.getPosition();//?
	//};

	string getPath() const {
		if (isLoadedAudio()) return path.get();
		else return string("NO FILE");
	};

	//--

private:

	void Changed_Params(ofAbstractParameter& e)
	{
		std::string name = e.getName();

		if (name == path.getName())
		{
			this->load(path);
		}
		if (name == volume.getName())
		{
			volume.setWithoutEventNotifications(ofClamp(volume, 0, 1));
			playerAudio.setVolume(volume.get());
		}
		if (name == bLoop.getName())
		{
			playerAudio.setLoop(bLoop);
		}

		//--

		if (name == position.getName())
		{
			//workflow
			//if (!bPlay && !playerAudio.getIsPlaying()) bPlay = true;

			playerState = PlayerState_PLAY_IN;
			fadeVal = 0;
			playerAudio.setVolume(0);

			playerAudio.setPosition(position);

			if (bStopped) bPlay = true;
		}

		//--

		// Play

		if (name == bPlay.getName())
		{
			if (bPlay.get())
			{
				if (playerAudio.getIsPlaying())
				{
					playerAudio.setPaused(false);
					//if (playerAudio.isPlaying()) playerAudio.setPaused(false);
					playerState = PlayerState_PLAY_OUT;
				}
				else
				{
					playerAudio.play();
					playerState = PlayerState_PLAY_IN;
					fadeVal = 0;
					playerAudio.setVolume(0);
				}
				bStopped = false;
			}
			//TODO: some small bug
			else
			{
				if (playerAudio.isPlaying())
				{
					playerAudio.setPaused(true);
					playerState = PlayerState_PLAY_OUT;
				}
				else
				{
					//playerAudio.stop();
				}

				bStopped = false;
			}
		}

		if (name == bStop.getName())
		{
			//bPlay.set(false);
			bPlay.setWithoutEventNotifications(false);
			playerAudio.stop();
			bStopped = true;
			playerState = PlayerState_STOP_OUT;
		}

		//--
	};

public:

	void doOpenDialogPath()
	{
		ofLogNotice("ofxSurfingSoundPlayer") << (__FUNCTION__) << " Set audio path";

		// Open the Open File Dialog
		std::string str = "Select an audio file.";
		ofFileDialogResult openFileResult = ofSystemLoadDialog(str, false);

		// Check if the user opened a file
		if (openFileResult.bSuccess)
		{
			//processOpenFileSelection(openFileResult);

			ofFile file(openFileResult.getPath());
			name_Audio = file.getBaseName();
			path = file.getAbsolutePath();

			ofLogNotice("ofxSurfingSoundPlayer") << "Name: " << name_Audio;
			ofLogNotice("ofxSurfingSoundPlayer") << "Path: " << path;

			load(path);
		}
		else
		{
			ofLogNotice("ofxSurfingSoundPlayer") << "User hit cancel";
		}
	};

	//--

private:

	ofSoundPlayer playerAudio;

public:

	bool load(std::string _path) {
		bool b = playerAudio.load(_path);
		bLoaded = b;

		if (path.get() != _path) path.setWithoutEventNotifications(_path);

		if (b) ofLogNotice("ofxSurfingSoundPlayer") << "Success: Loaded Audio file " << _path;
		else ofLogError("ofxSurfingSoundPlayer") << "Error: Audio File not found " << _path;

		playerAudio.setVolume(volume);
		playerAudio.setMultiPlay(false);
		playerAudio.setLoop(bLoop);

		// labels
		ofFile file(path.get());
		name_Audio = file.getBaseName();

		return b;
	};

	//--

private:

	ofxSurfingGui* ui;

public:

	void setUiPtr(ofxSurfingGui* _ui) { ui = _ui; }

	void drawImGuiWidgets()
	{
		bool bNotLoadedAudio = !isLoadedAudio();

		ui->Add(ui->bMinimize, OFX_IM_TOGGLE_ROUNDED_SMALL);
		ui->AddSpacingSeparated();
		if (!ui->bMinimize)
		{
			SurfingGuiTypes st = OFX_IM_BUTTON_SMALL;
			if (ui->AddButton("OPEN FILE", st))
			{
				doOpenDialogPath();
			}
			ui->AddSpacing();

			ui->BeginBlinkText(bNotLoadedAudio);
			{
				ui->AddLabelBig(name_Audio);

				string s = getPath();
				ui->AddTooltip(s);
			}
			ui->EndBlinkText(bNotLoadedAudio);

			ui->AddSpacingSeparated();
		}

		if (!bNotLoadedAudio)
		{
			// time label

			bool b = !bPlay && !bStopped;
			bool b2 = bStopped;
			if (b2) ui->BeginDarkenText();
			ui->BeginBlinkText(b);

			float t = getPositionAudioMS() / 1000.f;
			string s = ofxSurfingHelpers::calculateTime(t);
			ui->AddLabelHugeXXL(s);

			ui->EndBlinkText(b);
			if (b2) ui->EndDarkenText();

			ui->AddSpacing();

			ui->PushFont(OFX_IM_FONT_BIG);
			ui->Add(bPlay, OFX_IM_TOGGLE_BIG_BORDER_BLINK, 2, 0.5f);
			ui->SameLine();
			ui->Add(bStop, OFX_IM_BUTTON_BIG, 2, 0.5f);
			ui->PopFont();

			ui->AddSpacing();

			ui->Add(position, OFX_IM_HSLIDER_NO_LABELS);

			if (!ui->bMinimize)
			{
				ui->AddSpacing();
				ui->Add(bLoop, OFX_IM_TOGGLE_ROUNDED_SMALL);
				ui->AddSpacingSeparated();

				//TODO:
				//ui->AddComboBundle(volume);
				//ui->Add(volume, OFX_IM_COMBO_MULTI);

				// Center a single widget
				{
					float w = ui->getWidgetsWidth(2) / 2;
					// Pass the expected widget width divided by two
					AddSpacingPad(w);
					ui->Add(volume, OFX_IM_KNOB_DOTKNOB, 2);
				}
			}

			//--

			ui->AddSpacing();
			ui->AddDebugToggle(false);
			if (ui->bDebug) {
				string s; 
				
				s = ofToString(fadeVal, 3);
				ui->AddLabel(s);

				s = ofToString(playerState);
				ui->AddLabel(s);
			}
		}
	};

	void drawImGui()
	{
		if (!bGui) return;

		if (ui == NULL) {
			ofLogError("ofxSurfingSoundPlayer") <<
				" ofxImGui is not instantiated. You should set the parent ui instance as reference!";
			ofLogError("ofxSurfingSoundPlayer") <<
				" Usually: player.setUiPtr(&ui);";
			return;
		}

		ImVec2 size_min = ImVec2(180, 100);
		ImVec2 size_max = ImVec2(FLT_MAX, FLT_MAX);
		ImGui::SetNextWindowSizeConstraints(size_min, size_max);

		if (ui->BeginWindow(bGui))
		{
			drawImGuiWidgets();

			ui->EndWindow();
		}
	}
};
