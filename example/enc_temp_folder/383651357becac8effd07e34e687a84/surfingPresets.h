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

	add settings state for gui
	add auto save
	add rename preset
	add copy/swap?
	add save files
	search file listers from surfingPresets
*/

//----

class SurfingPresets
{

public:

	SurfingPresets() {
		ofAddListener(params.parameterChangedE(), this, &SurfingPresets::Changed);

		params.add(vScan);
		params.add(vDelete);
		params.add(vSave);
		params.add(vLoad);
		params.add(vNew);
		params.add(vRename);
		params.add(vReset);
		params.add(index);
		params.add(bAutoSave);
		params.add(bClicker);
	};

	~SurfingPresets() {
		ofRemoveListener(params.parameterChangedE(), this, &SurfingPresets::Changed);

		doSave();
	};

public:

	ofxSurfingGui* ui;
	void setUiPtr(ofxSurfingGui* _ui) {
		ui = _ui;
	}



	//--

	// Presets

	void drawImGui(bool bWindowed = false)
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

			static bool bFolder = true;
			static bool bExpand = false;

			bool b;
			if (!bFolder) ui->AddLabelBig("PRESETS", true, true);
			else b = ui->BeginTree("PRESETS");
			if (b) {
				ui->AddToggle("Expand", bExpand, OFX_IM_TOGGLE_ROUNDED_MINI);
				//if (!ui->bMinimize)
				if (bExpand)
				{
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

					if(ui->Add(vRename, OFX_IM_BUTTON_SMALL, 2, true)){
						//delete
						vDelete.trigger();
						//create new
						if (!bInputText) bInputText = true;
						_namePreset = "";
						setFilename(_namePreset);
					};

					ui->Add(vDelete, OFX_IM_BUTTON_SMALL, 2);
					ui->Add(vScan, OFX_IM_BUTTON_SMALL, 2, true);
					ui->Add(bAutoSave, OFX_IM_TOGGLE_SMALL, 2);

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
				else {
					ui->Add(vSave, OFX_IM_BUTTON_SMALL);
				}

				//--
				
				ui->AddSpacingSeparated();

				// Combo
				ui->AddComboButtonDual(index, filenames, true);

				if (ui->AddButton("NEXT", OFX_IM_BUTTON_SMALL))
				{
					doLoadNext();
				};

				//ui->Add(bClicker, OFX_IM_TOGGLE_BUTTON_ROUNDED);

				//--

				ui->AddSpacingSeparated();

				drawImGuiClicker();

				/*
				if (!ui->bMinimize)
				{
					// preset name
					if (_namePreset != "") ui->AddLabel(_namePreset.c_str());
				}
				*/

				//--

				if (b) ui->EndTree();
			}
		}
		if (bWindowed) {}
	}


	////--

	//TODO:
	// Files

	// 2. Presets
	void drawImGuiClicker(bool bWindowed = false)
	{
		ui->Add(bClicker, OFX_IM_TOGGLE_BUTTON_ROUNDED_SMALL);
		if (bClicker)
		{
			float h = ui->getWidgetsHeightUnit();
			ofxImGuiSurfing::AddMatrixClickerLabelsStrings(index, filenames, true, 1, true, h);

			/*
			inline bool AddMatrixClickerLabelsStrings(ofParameter<int>&_index,
			const std::vector<string> labels, bool bResponsive = true, int amountBtRow = 3, const bool bDrawBorder = true, float __h = -1, bool bSpaced = true, string toolTip = "")
			*/

			/*
			ui->Indent();
			{
				// Paths
				{
					bool bOpen = false;
					ImGuiTreeNodeFlags _flagt = (bOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None);
					//_flagt |= ImGuiTreeNodeFlags_Framed;

					if (ImGui::TreeNodeEx("Path", _flagt))
					{
						ImGui::TextWrapped(pathPresets.data()); // -> show path
						ImGui::TreePop();
					}
				}

				// Files
				// Buttons Selector for each file
				if (ofxImGuiSurfing::filesPicker(pathPresets, filename, index, { "json" }))
				{
					// Buttons Matrix

					//TODO:
					// Index back not working
					// this is a workaround
					// could fail on macOS/Linux -> requires fix paths slashes

					for (int i = 0; i < dir.size(); i++)
					{
						std::string si = ofToString(i);
						if (i < 10) si = "0" + si;
						std::string ss = name_Root + "_" + si;
						fileName = ss;

						auto s0 = ofSplitString(nameSelected, "\\", true);
						std::string s1 = s0[s0.size() - 1]; // filename
						auto s = ofSplitString(s1, ".json");

						std::string _nameSelected = s[0];

						if (_nameSelected == fileName)
						{
							index = i;
						}
					}

					ofLogNotice(__FUNCTION__) << "Picked file " << nameSelected << " > " << index;
				}
			}
			ui->Unindent();
			*/
		}
	}

	//--

public:

	ofParameter<void> vPrevious{ "<" };
	ofParameter<void> vNext{ ">" };
	ofParameter<void> vRename{ "Rename" };
	ofParameter<void> vScan{ "Scan" };
	ofParameter<void> vDelete{ "Delete" };
	ofParameter<void> vSave{ "Save" };
	ofParameter<void> vLoad{ "Load" };
	ofParameter<void> vNew{ "New" };
	ofParameter<void> vReset{ "Reset" };

	ofParameter<int> index{ "Index", 0, 0, 0 };
	ofParameter<bool> bClicker{ "CLICKER", false };
	ofParameter<bool> bAutoSave{ "AutoSave", true };

public:

	void setup()
	{
		ofLogNotice("SurfingPresets") << (__FUNCTION__);
		doRefreshFiles();//TODO:
	}

	void startup()
	{
		ofLogNotice("SurfingPresets") << (__FUNCTION__);

		doRefreshFiles();

		//index = index;
	}

	void update()
	{
		if (ofGetFrameNum() == 0) {
			startup();
		}
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
		ofLogNotice("SurfingPresets") << (__FUNCTION__);

		ofxSurfingHelpers::CheckFolder(pathPresets);
		ofxSurfingHelpers::saveGroup(paramsPreset, pathPresets + "/" + filename + ".json");
	}

	/*
	void Save(string filename)
	{
		ofLogNotice("SurfingPresets") << (__FUNCTION__);

		//ofxSurfingHelpers::CheckFolder(pathPresets);
		ofxSurfingHelpers::saveGroup(paramsPreset, pathPresets + "/" + filename + ".json");
	}
	*/

	void doLoad()
	{
		ofLogNotice("SurfingPresets") << (__FUNCTION__);

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

	void doLoadNext() {
		if (index < index.getMax()) index++;
		else if (index == index.getMax()) index = index.getMin();
	}

private:

	void Changed(ofAbstractParameter& e)
	{
		string name = e.getName();
		ofLogNotice("SurfingPresets") << (__FUNCTION__);
		ofLogNotice("SurfingPresets") << name << " " << e;

		if (0) {}

		else if (name == index.getName())
		{
			if (filenames.size() == 0) return;

			static int _index;
			if (_index != index) {//changed
				if (bAutoSave) {
					filename = filenames[_index];
					doSave();
				}
				_index = index;
			}

			filename = filenames[index];

			doLoad();
		}

		else if (name == vLoad.getName())
		{
			doLoad();
		}

		else if (name == vSave.getName())
		{
			int num = getNumFiles();

			doSave();

			// scan
			string filename_ = filename;
			doRefreshFiles();

			//if (filename_ != filename)
			if (num != getNumFiles())
			{
				ofLogNotice("SmoothChannel") << (__FUNCTION__) << "Reorganize " << dir.size()<<" files.";
				for (int i = 0; i < dir.size(); i++)
				{
					if (dir.getName(i) == filename_)
					{
						index = i;
						break;
					}
				}
			}
		}

		else if (name == vScan.getName())
		{
			doRefreshFiles();
		}

		else if (name == vDelete.getName())
		{
			if (filenames.size() == 0) return;

			//filename = filenames[index];
			ofFile::removeFile(pathPresets + "/" + filename + ".json");
			doRefreshFiles();

			index = index;
			//doLoad();
		}

		else if (name == vRename.getName())
		{
			if (filenames.size() == 0) return;

			/*
			// remove
			//filename = filenames[index];
			ofFile::removeFile(pathPresets + "/" + filename + ".json");
			doRefreshFiles();

			// make new
			*/
		}
	};

	//--

private:

	ofParameterGroup params{ "SurfingPresets" };
	ofParameterGroup paramsPreset{ "Preset" };

	string pathPresets = "SurfingPresets";

	// Files Browser
	ofDirectory dir;
	std::string fileName;
	std::string filePath;

public:
	string filename = "presetName";

	vector<std::string> filenames;

	//--

private:
	
	int getNumFiles() {
		return filenames.size();
	}

	bool doRefreshFiles()
	{
		// Load dragged images folder
		ofLogNotice("SurfingPresets") << (__FUNCTION__) << "list files " << pathPresets;

		bool b = false;

		dir.listDir(pathPresets);
		dir.allowExt("JSON");
		dir.allowExt("json");
		dir.sort();

		// Log files on folder
		filenames.clear();
		for (int i = 0; i < dir.size(); i++)
		{
			ofLogNotice("SurfingPresets") << (__FUNCTION__) << "file " << "[" << ofToString(i) << "] " << dir.getName(i);

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

