
/*

	TODO

	add fade in to avoid clicks when dragging position
	add start / end loop
	files browser for fast preview

*/


#define USE_EXTERNAL_DEVICE_OUT_MANAGER

//#define USE_DEBUG_FADING // to debug start/stop fading engine

//--

#pragma once

#include "ofMain.h"

#include "ofxSurfingHelpers.h"
#include "ofxSurfingImGui.h"

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
		PlayerState_UNKNOWN = 0,
		PlayerState_PLAY_TO_STOP,
		PlayerState_STOPPED,
		PlayerState_PLAY_IN,
		PlayerState_PLAYING,
		PlayerState_PLAY_TO_PAUSED,
		PlayerState_PAUSED,
		PlayerState_AMOUNT,
	};

	PlayerState_ playerState = PlayerState_UNKNOWN;
	PlayerState_ playerState_PRE = PlayerState_AMOUNT;
	string namePlayerState = "";

	float fadeVal = 0;
	float fadeVal_PRE = -1;
	ofParameter<float> fadeStep{ "Step", 0.05, 0, 0.5 };

	//TODO: It seems that ofPlayer do not shows if it's really playing..
	// it looks like when paused, it returns that is playing. so it's an oF API decision..

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
	ofParameter<bool> bLoop{ "Loop", true };
	ofParameter<bool> bScrub{ "Scrub", false };
	ofParameter<float> position{ "Position", 0, 0, 1 };

	string name_Audio = "NO FILE";

public:

	void setup()
	{
		ofLogNotice("ofxSurfingSoundPlayer") << ":" << (__FUNCTION__);

		setup_Audio();

		ofAddListener(params.parameterChangedE(), this, &ofxSurfingSoundPlayer::Changed_Params);

		startup();
	};

private:

	bool bDoneStartup = false;

	void startup()
	{
		ofLogNotice("ofxSurfingSoundPlayer") << ":" << (__FUNCTION__);

		playerState = ofxSurfingSoundPlayer::PlayerState_STOPPED;
		updateStates();

		ofxSurfingHelpers::loadGroup(params, path_GLOBAL + "/" + path_AppSettings);

		bDoneStartup = true;
	};

	void setup_Audio()
	{
		ofLogNotice("ofxSurfingSoundPlayer") << ":" << (__FUNCTION__);

		bGui.set("PLAYER", true);

		params_Audio.add(bGui);
		params_Audio.add(path);
		params_Audio.add(bPlay);
		params_Audio.add(bStop);
		params_Audio.add(bLoop);
		params_Audio.add(bScrub);
		params_Audio.add(position);
		params_Audio.add(volume);

		params_Audio.add(fadeStep);

		params.add(params_Audio);

		//// to starts at zero always!
		//position.setSerializable(false);
		//bPlay.setSerializable(false);

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

	//--

public:

	void updateStates() {

		// Fade volume transitions
		// to avoid glitches
		{
			// 1. Sate changed

			if (playerState != playerState_PRE)
			{
				switch (playerState)
				{

				case ofxSurfingSoundPlayer::PlayerState_PLAY_IN: // begins play phase
					namePlayerState = "PLAY_IN";
					if (!bPlay) bPlay.setWithoutEventNotifications(true);
					if (playerState_PRE == PlayerState_PAUSED) playerAudio.setPaused(false);
					else playerAudio.play();
					fadeVal = 0;
					playerAudio.setVolume(0);
					break;

				case ofxSurfingSoundPlayer::PlayerState_PLAYING: // already playing
					namePlayerState = "PLAYING";
					if (!playerAudio.isPlaying()) playerAudio.play();
					else if (playerState_PRE == PlayerState_PAUSED) playerAudio.setPaused(false);
					break;

				case ofxSurfingSoundPlayer::PlayerState_PLAY_TO_PAUSED: // begins pause phase
					namePlayerState = "PLAY_TO_PAUSED";
					playerAudio.setPaused(true);
					break;

				case ofxSurfingSoundPlayer::PlayerState_PAUSED: // just paused
					namePlayerState = "PAUSED";
					fadeVal = 0;
					playerAudio.setVolume(0);
					break;

				case ofxSurfingSoundPlayer::PlayerState_PLAY_TO_STOP: // begins stop phase
					namePlayerState = "PLAY_TO_STOP";
					if (bPlay) bPlay.setWithoutEventNotifications(false);
					break;

				case ofxSurfingSoundPlayer::PlayerState_STOPPED: // just stopped
					namePlayerState = "STOPPED";
					if (bPlay) bPlay.setWithoutEventNotifications(false);
					playerAudio.stop();
					bPlay.setWithoutEventNotifications(false);
					fadeVal = 0;
					playerAudio.setVolume(0);
					break;
				}

				playerState_PRE = playerState;

				ofLogNotice("ofxSurfingSoundPlayer") << "State Changed to " << namePlayerState;
			}

			//--

			// 2. Update for current happening state

			switch (playerState)
			{
			case ofxSurfingSoundPlayer::PlayerState_PLAY_IN:
				fadeVal += fadeStep;
				fadeVal = ofClamp(fadeVal, 0, 1.f);
				if (fadeVal == 1.f) playerState = PlayerState_PLAYING;
				break;

			case ofxSurfingSoundPlayer::PlayerState_PLAYING:
				break;

			case ofxSurfingSoundPlayer::PlayerState_PLAY_TO_PAUSED:
				fadeVal -= fadeStep;
				fadeVal = ofClamp(fadeVal, 0, 1.f);
				if (fadeVal == 0) playerState = PlayerState_PAUSED;
				break;

			case ofxSurfingSoundPlayer::PlayerState_PAUSED:
				break;

			case ofxSurfingSoundPlayer::PlayerState_PLAY_TO_STOP:
				fadeVal -= fadeStep;
				fadeVal = ofClamp(fadeVal, 0, 1.f);
				if (fadeVal == 0) playerState = PlayerState_STOPPED;
				break;

			case ofxSurfingSoundPlayer::PlayerState_STOPPED:
				fadeVal = 0;
				break;
			}

			//--

			// 3. Set volume only if changed

			playerAudio.setVolume(fadeVal * volume.get());
			//if (fadeVal != fadeVal_PRE)
			//{
			//	fadeVal_PRE = fadeVal;
			//	playerAudio.setVolume(fadeVal * volume.get());
			//}
		}
	};

	void update()
	{
		if (!bLoaded) return;

		updateStates();

		//--

		//if (position.get() != playerAudio.getPosition())
		{
			position.setWithoutEventNotifications(playerAudio.getPosition());
		}

		ofSoundUpdate();
	};

	void keyPressed(int key)
	{
		if (key == ' ') setTogglePlay();
		if (key == OF_KEY_BACKSPACE) setStop();
		if (key == OF_KEY_LEFT) setPositionLeft();
		if (key == OF_KEY_RIGHT) setPositionRight();
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

	// Setters

	//--

public:

	// Transport

	void setTogglePlay() {
		//playerState = PlayerState_PLAY_IN;
		bPlay = !bPlay;
	}
	void setStop() {
		//playerState = PlayerState_PLAY_TO_STOP;
		bStop.trigger();
	}
	void setPaused(bool b) {
		if (b) { // do pause
			if (playerState != PlayerState_PAUSED)
				playerState = PlayerState_PLAY_TO_PAUSED;
		}
		else { // release pause
			if (playerState == PlayerState_PAUSED)
				playerState = PlayerState_PLAY_IN;
		}
	};

	//--

public:

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
		volume = MIN(volume + 0.05f, 1);
	};
	void setVolumenDown()
	{
		volume = MAX(volume - 0.05f, 0);
	};

	void setPositionLeft()
	{
		position = MIN(position - 0.025f, 1);
	};
	void setPositionRight()
	{
		position = MIN(position + 0.025f, 1);
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

		if (0) {}

		//--

		// Play

		else if (name == bPlay.getName())
		{
			if (bPlay.get())
			{
				if (playerState == PlayerState_PAUSED)
				{
					playerState = PlayerState_PLAY_IN;
					updateStates();

					return;
				}

				else if (playerState != PlayerState_PLAYING)
				{
					playerState = PlayerState_PLAY_IN;
					updateStates();

					return;
				}
			}
			else
			{
				if (playerState == PlayerState_PLAYING)
				{
					playerState = PlayerState_PLAY_TO_PAUSED;
					updateStates();

					return;
				}
			}

		}

		//--

		// Stop

		else if (name == bStop.getName())
		{
			//if (playerState != PlayerState_STOPPED)
			{
				playerState = PlayerState_PLAY_TO_STOP;
				updateStates();

				return;
			}
		}

		//--

		// Position

		else if (name == position.getName())
		{
			if (playerAudio.getPosition() != position.get())
			{
				//TODO: fix
				//workflow: force play
				if (bScrub)
					if (bDoneStartup)
					{
						playerState = PlayerState_PLAY_IN;
						updateStates();
					}

				playerAudio.setPosition(position.get());

				return;
			}
		}

		//--

		else if (name == path.getName())
		{
			this->load(path);

			return;
		}

		else if (name == volume.getName())
		{
			volume.setWithoutEventNotifications(ofClamp(volume, 0, 1));
			playerAudio.setVolume(volume.get() * fadeVal);

			return;
		}

		else if (name == bLoop.getName())
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

			//workflow
			playerState = PlayerState_PLAY_IN;
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

	ofxSurfingGui* ui = nullptr;

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
			// Time label

			// blink
			bool b1 = playerState == PlayerState_PAUSED;

			// dark
			bool b2 = playerState == PlayerState_STOPPED;

			ui->BeginDarkenText(b2);
			ui->BeginBlinkText(b1);
			{
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

					// align right. 
					// push left from right border
					AddSpacingRightAlign(40);//ms
					//AddSpacingRightAlign(21);
					ui->AddLabel(s);
				}
			}
			ui->EndBlinkText(b1);
			ui->EndDarkenText(b2);

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
				ui->AddSpacingBig();
				ui->Add(bLoop, OFX_IM_TOGGLE_ROUNDED_SMALL);
				ui->Add(bScrub, OFX_IM_TOGGLE_ROUNDED_MINI);
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

#ifdef USE_DEBUG_FADING
			ui->AddSpacingSeparated();
			ui->AddDebugToggle(false);
			ui->AddSpacing();
			if (ui->bDebug)
			{
				ui->Indent();

				ui->Add(fadeStep, OFX_IM_STEPPER);
				ofxImGuiSurfing::AddProgressBar(fadeVal);
				string s = "FADE " + ofToString(fadeVal, 3);
				ui->AddLabel(s);
				ui->AddLabel("> " + namePlayerState);

				bool b = playerAudio.isPlaying();
				if (b) {
					s = "isPlaying()";
					ui->AddLabel(s);
				}

				//if (playerState == PlayerState_PAUSED) {
				//	s = "PAUSED    " + ofToString(playerState == PlayerState_PAUSED ? "+" : " ");
				//	ui->AddLabel(s);
				//}
				//if (bStopped) {
				//s = "STOP      " + ofToString(bStopped ? "+" : " ");
				//ui->AddLabel(s);
				//}

				ui->Unindent();
		}
#endif
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
