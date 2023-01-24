
/*

	TODO

	add fade in to avoid clicks when dragging position

*/


#define USE_EXTERNAL_DEVICE_OUT_MANAGER

//--

#pragma once

#include "ofMain.h"

#include "ofxSurfingHelpers.h"
#include "ofxSurfingImGui.h"

//#define stringify( name ) #name
//#define stringify( name ) #namebDe

//class ofxSurfingSoundPlayer : public ofBaseApp
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
		PlayerState_STOP_IN = 0,
		PlayerState_STOPPED,
		PlayerState_PLAY_IN,
		PlayerState_PLAYING,
		PlayerState_PLAY_OUT,
		PlayerState_PAUSED,
		PlayerState_AMOUNT,
	};

	PlayerState_ playerState;
	PlayerState_ playerState_PRE = PlayerState_(-1);
	string namePlayerState = "";

	float fadeVal = 0;
	float fadeVal_PRE = -1;
	ofParameter<float> fadeStep{ "Step", 0.05, 0, 0.5 };

	//TODO: it seems that ofPlayer do not show if it's played..
	bool bPlaying = false;

	//----

private:

	string path_GLOBAL = "ofxSurfingSoundPlayer";
	string path_AppSettings = "ofxSurfingSoundPlayer.json";

#ifndef USE_EXTERNAL_DEVICE_OUT_MANAGER
	ofSoundStreamSettings outSettings;
	ofSoundStream outStream;
	std::vector<ofSoundDevice> outDevices;
#endif

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

	void startup() 
	{
		playerState = ofxSurfingSoundPlayer::PlayerState_STOPPED;
		namePlayerState = "STOPPED";
		fadeVal = 0;
		
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

		params_Audio.add(fadeStep);

		params.add(params_Audio);

		// to starts at zero always!
		//position.setSerializable(false);

		//--

		//TODO:
		// add device selector into ofxSoundDeviceManager
#ifndef USE_EXTERNAL_DEVICE_OUT_MANAGER

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
#endif
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
				case ofxSurfingSoundPlayer::PlayerState_STOP_IN:
					namePlayerState = "STOP_IN";
					break;
				case ofxSurfingSoundPlayer::PlayerState_STOPPED:
					namePlayerState = "STOPPED";
					fadeVal = 0;
					break;
				case ofxSurfingSoundPlayer::PlayerState_PLAY_IN:
					namePlayerState = "PLAY_IN";
					fadeVal = 0;
					//fix bug
					bPlay = true;
					break;
				case ofxSurfingSoundPlayer::PlayerState_PLAYING:
					namePlayerState = "PLAYING";
					break;
				case ofxSurfingSoundPlayer::PlayerState_PLAY_OUT:
					namePlayerState = "PLAY_OUT";
					break;
				case ofxSurfingSoundPlayer::PlayerState_PAUSED:
					namePlayerState = "PAUSED";
					fadeVal = 0;
					break;
				}

				//namePlayerState = stringify(playerState);
			}

			// Update on states
			switch (playerState)
			{
			case ofxSurfingSoundPlayer::PlayerState_STOP_IN:
				fadeVal -= fadeStep;
				fadeVal = ofClamp(fadeVal, 0, 1);
				if (fadeVal == 0) playerState = PlayerState_STOPPED;
				break;
			case ofxSurfingSoundPlayer::PlayerState_STOPPED:
				fadeVal = 0;
				break;
			case ofxSurfingSoundPlayer::PlayerState_PLAY_IN:
				fadeVal += fadeStep;
				fadeVal = ofClamp(fadeVal, 0, 1);
				if (fadeVal == 1) playerState = PlayerState_PLAYING;
				break;
			case ofxSurfingSoundPlayer::PlayerState_PLAYING:
				break;
			case ofxSurfingSoundPlayer::PlayerState_PLAY_OUT:
				fadeVal -= fadeStep;
				fadeVal = ofClamp(fadeVal, 0, 1);
				if (fadeVal == 0) playerState = PlayerState_PAUSED;
				break;
			case ofxSurfingSoundPlayer::PlayerState_PAUSED:
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

	void exit()
	{
#ifndef USE_EXTERNAL_DEVICE_OUT_MANAGER
		outStream.close();
#endif

		ofRemoveListener(params.parameterChangedE(), this, &ofxSurfingSoundPlayer::Changed_Params);

		ofxSurfingHelpers::CheckFolder(path_GLOBAL);
		ofxSurfingHelpers::saveGroup(params, path_GLOBAL + "/" + path_AppSettings);
	};

	//--

public:

	// Setters

	void setPaused(bool b) {
		playerAudio.setPaused(b);
		bPlaying = false;
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

		//--

		if (name == position.getName())
		{
			playerState = PlayerState_PLAY_IN;
			fadeVal = 0;
			playerAudio.setVolume(0);
			playerAudio.setPosition(position);

			bool b = bPlaying;
			//bool b = playerAudio.isPlaying();

			if (!bPlay && !b)
			{
				bPlay.setWithoutEventNotifications(true);
				bPlaying = true;
			}

			//if (bStopped) bPlay = true;

			return;
		}

		//--

		// Play

		if (name == bPlay.getName())
		{
			if (bPlay.get()) // true
			{
				bool b = bPlaying;
				//bool b = playerAudio.isPlaying();//BUG: don't notifies when paused
				//bool b = playerAudio.getPlayer()->setPaused;
				//bool b = playerAudio.pause();

				if (b)
				{
					playerAudio.setPaused(false);
					bPlaying = false;

					//if (playerAudio.isPlaying()) playerAudio.setPaused(false);
					playerState = PlayerState_PLAY_OUT;
				}
				else
				{
					playerAudio.play();
					bPlaying = true;

					playerState = PlayerState_PLAY_IN;
					fadeVal = 0;
					playerAudio.setVolume(0);
				}

				bStopped = false;
			}
			//TODO: some small bug
			else // false
			{
				bool b = bPlaying;
				//bool b = playerAudio.isPlaying();

				if (b)
				{
					playerAudio.setPaused(true);
					bPlaying = false;

					playerState = PlayerState_PLAY_OUT;
				}
				else
				{
					//playerAudio.stop();
					//bPlaying = false;
				}

				bStopped = false;
			}

			return;
		}

		if (name == bStop.getName())
		{
			//bPlay.set(false);
			bPlay.setWithoutEventNotifications(false);
			playerAudio.stop();
			bPlaying = false;

			bStopped = true;
			playerState = PlayerState_STOP_IN;

			return;
		}

		if (name == path.getName())
		{
			this->load(path);

			return;
		}
		if (name == volume.getName())
		{
			volume.setWithoutEventNotifications(ofClamp(volume, 0, 1));
			playerAudio.setVolume(volume.get());
			return;
		}
		if (name == bLoop.getName())
		{
			playerAudio.setLoop(bLoop);

			return;
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
				ui->AddLabel(name_Audio);

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

			float t = getPositionAudioMS() / 1000.f;//seconds
			//min::sec
			string s = ofxSurfingHelpers::calculateTime(t);
			ui->AddLabelHugeXXL(s);
			//ms
			if (!ui->bMinimize)
			{
				int tms = getPositionAudioMS() % 1000;
				s = ofToString(tms);
				if (tms < 10) s = "00" + s;
				else if (tms < 100) s = "0" + s;
				s += " ms";

				// push up
				AddSpacingOffset(ImVec2{ 0,-13 });

				// align right
				AddSpacingRightAlign(35);//ms
				//AddSpacingRightAlign(21);
				ui->AddLabel(s);
			}

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

			ui->bAutoResize = true;
			ui->AddSpacingSeparated();
			ui->AddDebugToggle(false);
			ui->AddSpacing();
			if (ui->bDebug)
			{
				ui->Indent();

				ui->Add(fadeStep, OFX_IM_STEPPER);
				ofxImGuiSurfing::AddProgressBar(fadeVal);
				s = "FADE      " + ofToString(fadeVal, 3);
				ui->AddLabel(s);

				bool b = bPlaying;
				//bool b = playerAudio.isPlaying();

				s = "PLAYING   " + ofToString(b ? "+" : " ");
				ui->AddLabel(s);

				s = "PAUSED    " + ofToString(playerState == PlayerState_PAUSED ? "+" : " ");
				ui->AddLabel(s);

				s = "STOPPED   " + ofToString(bStopped ? "+" : " ");
				ui->AddLabel(s);

				ui->AddLabel(namePlayerState);

				ui->Unindent();
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
