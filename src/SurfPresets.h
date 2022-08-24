#pragma once

#include "ofMain.h"
#include "ofxSurfingHelpers.h"

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

	ofParameter<void> vPrevious{ "<" };
	ofParameter<void> vNext{ ">" };
	ofParameter<void> vSave{ "Save" };
	ofParameter<void> vLoad{ "Load" };
	ofParameter<void> vNew{ "New" };
	ofParameter<void> vReset{ "Reset" };
	ofParameter<int> index{ "Index", 0, 0, 0 };

	SurfPresets() {
		ofAddListener(params.parameterChangedE(), this, &SurfPresets::Changed);

		params.add(vSave);
		params.add(vLoad);
		params.add(vNew);
		params.add(vReset);
		params.add(index);
	};

	~SurfPresets() {
		ofRemoveListener(params.parameterChangedE(), this, &SurfPresets::Changed);

		Save();
	};

	void setup()
	{
		ofLogNotice("SurfPresets") << (__FUNCTION__);
	}

	void startup()
	{
		ofLogNotice("SurfPresets") << (__FUNCTION__);
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

	void Save()
	{
		ofLogNotice("SurfPresets") << (__FUNCTION__);

		ofxSurfingHelpers::CheckFolder(pathGlobal);
		ofxSurfingHelpers::saveGroup(paramsPreset, pathGlobal + "/" + pathSettings);
	}

	void Load()
	{
		ofLogNotice("SurfPresets") << (__FUNCTION__);

		// Load Settings
		ofxSurfingHelpers::loadGroup(paramsPreset, pathGlobal + "/" + pathSettings);
	}

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
		pathGlobal = p + "/Presets";
	}

private:

	void Changed(ofAbstractParameter& e) {
		string name = e.getName();
		ofLogNotice("SurfPresets") << (__FUNCTION__);
		ofLogNotice("SurfPresets") << name << " " << e;

		if (0) {}

		else if (name == vLoad.getName())
		{
			Load();
		}

		else if (name == vSave.getName())
		{
			Save();
		}
	};

	ofParameterGroup params{ "SurfPresets" };
	ofParameterGroup paramsPreset{ "Preset" };

	string pathGlobal = "SurfPresets";
	string pathSettings = "Settings.json";
};

