#pragma once

#include "ofMain.h"

#include "ofxSurfingHelpers.h"
#include "ofxSurfingImGui.h"
#include "imgui_stdlib.h"

/*

	Simple Presets Manager

*/

//----

/*

	TODO:

	add save files
	search file listers from surfingPresets

	could pass an unique pointer of a ui object to avoid create a new one..
	#include "ofxSurfingImGui.h"

*/

//----

class SurfPresets
{

public:

	SurfPresets() {
		ofAddListener(params.parameterChangedE(), this, &SurfPresets::Changed);

		params.add(vScan);
		params.add(vDelete);
		params.add(vSave);
		params.add(vLoad);
		params.add(vNew);
		params.add(vReset);
		params.add(index);
	};

	~SurfPresets() {
		ofRemoveListener(params.parameterChangedE(), this, &SurfPresets::Changed);

		doSave();
	};

public:

	ofxSurfingGui* ui;
	void setUiPtr(ofxSurfingGui* _ui) {
		ui = _ui;
	}



	//--

	// 2. Presets
	void drawImGui(bool bWindowed)
	{
		//TODO:
		// make windowed

		if (bWindowed) {}
		{
			//TODO:
			static string _namePreset = "";
			//static bool bTyping = false;
			static bool bInputText = false;
			//string s = "presetName";
			string s = filename;


			ui->AddLabelBig("Presets", true, true);
			if (!ui->bMinimize) {
				ui->Add(vLoad, OFX_IM_BUTTON_SMALL, 2, true);

				if (ui->Add(vSave, OFX_IM_BUTTON_SMALL, 2))
				{
					bInputText = false;
					_namePreset = s;
				};

				if (ui->Add(vNew, OFX_IM_BUTTON_SMALL, 2, true))
				{
					if (!bInputText) bInputText = true;
					_namePreset = "";
					setFilename(_namePreset);
				};
				ui->Add(vReset, OFX_IM_BUTTON_SMALL, 2);

				ui->Add(vScan, OFX_IM_BUTTON_SMALL, 2, true);
				ui->Add(vDelete, OFX_IM_BUTTON_SMALL, 2);

				//--

				ui->AddSpacing();

				if (bInputText)
				{
					int _w = ui->getWidgetsWidth() * 0.9f;
					ImGui::PushItemWidth(_w);
					{
						bool b = ImGui::InputText("##NAME", &s);
						if (b) {
							ofLogNotice("WaveformPlot") << "InputText:" << s.c_str();
							setFilename(s);
						}
					}

					ImGui::PopItemWidth();
				}
			}

			//--

			// Combo
			ui->AddComboButtonDual(index, filenames);

			/*
			if (!ui->bMinimize)
			{
				// preset name
				if (_namePreset != "") ui->AddLabel(_namePreset.c_str());
			}
			*/
		}
		if (bWindowed) {}
	}

	//--

public:

	ofParameter<void> vPrevious{ "<" };
	ofParameter<void> vNext{ ">" };
	ofParameter<void> vScan{ "Scan" };
	ofParameter<void> vDelete{ "Delete" };
	ofParameter<void> vSave{ "Save" };
	ofParameter<void> vLoad{ "Load" };
	ofParameter<void> vNew{ "New" };
	ofParameter<void> vReset{ "Reset" };
	ofParameter<int> index{ "Index", 0, 0, 0 };

public:

	void setup()
	{
		ofLogNotice("SurfPresets") << (__FUNCTION__);
	}

	void startup()
	{
		ofLogNotice("SurfPresets") << (__FUNCTION__);

		doRefreshFiles();
	}

	void update()
	{
		ofLogNotice("SurfPresets") << (__FUNCTION__);
	}

	void draw()
	{
		//ImVec2 sz(100,30);
		//if (ofxSurfingGui::AddButton("Save", sz)) {
		//}
	}

	//--

	void doSave()
	{
		ofLogNotice("SurfPresets") << (__FUNCTION__);

		ofxSurfingHelpers::CheckFolder(pathPresets);
		ofxSurfingHelpers::saveGroup(paramsPreset, pathPresets + "/" + filename + ".json");

		doRefreshFiles();
	}

	/*
	void Save(string filename)
	{
		ofLogNotice("SurfPresets") << (__FUNCTION__);

		//ofxSurfingHelpers::CheckFolder(pathPresets);
		ofxSurfingHelpers::saveGroup(paramsPreset, pathPresets + "/" + filename + ".json");
	}
	*/

	void doLoad()
	{
		ofLogNotice("SurfPresets") << (__FUNCTION__);

		// Load Settings
		ofxSurfingHelpers::loadGroup(paramsPreset, pathPresets + "/" + filename + ".json");
	}

	//-

	void AddGroup(ofParameterGroup& group)
	{
		// make a pointer, bc maybe that = no works well..

		auto ptype = group.type();
		bool isGroup = ptype == typeid(ofParameterGroup).name();
		if (isGroup)
		{
			paramsPreset = group;
		}
	}

	void setPath(string p) {
		pathPresets = p + "/Presets";
	}

	void setFilename(string p) {
		filename = p;
	}

private:

	void Changed(ofAbstractParameter& e)
	{
		string name = e.getName();
		ofLogNotice("SurfPresets") << (__FUNCTION__);
		ofLogNotice("SurfPresets") << name << " " << e;

		if (0) {}

		else if (name == index.getName())
		{
			filename = filenames[index];
			doLoad();
		}

		else if (name == vLoad.getName())
		{
			doLoad();
		}

		else if (name == vSave.getName())
		{
			doSave();
		}

		else if (name == vScan.getName())
		{
			doRefreshFiles();
		}

		else if (name == vDelete.getName())
		{
			filename = filenames[index];
			ofFile::removeFile(pathPresets + "/" + filename + ".json");
			doRefreshFiles();
		}
	};

	//--

private:

	ofParameterGroup params{ "SurfPresets" };
	ofParameterGroup paramsPreset{ "Preset" };

	string pathPresets = "SurfPresets";

	// Files Browser
	ofDirectory dir;
	std::string fileName;
	std::string filePath;

public:
	string filename = "presetName";

	vector<std::string> filenames;

	//--

private:

	bool doRefreshFiles()
	{
		// Load dragged images folder
		ofLogNotice("SurfPresets") << (__FUNCTION__) << "list files " << pathPresets;

		bool b = false;

		dir.listDir(pathPresets);
		dir.allowExt("JSON");
		dir.allowExt("json");
		dir.sort();

		// Log files on folder
		filenames.clear();
		for (int i = 0; i < dir.size(); i++)
		{
			ofLogNotice("SurfPresets") << (__FUNCTION__) << "file " << "[" << ofToString(i) << "] " << dir.getName(i);

			std::string _name = "NoName"; // without ext
			auto _names = ofSplitString(dir.getName(i), ".");
			if (_names.size() > 0) {
				_name = _names[0];
			}
			filenames.push_back(_name);
		}

		index.setMin(0);

		if (dir.size() == 0) index.setMax(0);
		else
		{
			index.setMax(dir.size() - 1);
		}

		//index = index;

		//-

		b = (dir.size() > 0);
		// true if there's some file

		return b;
	}
};

