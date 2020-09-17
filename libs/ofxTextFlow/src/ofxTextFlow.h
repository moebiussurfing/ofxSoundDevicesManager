#pragma once
#include "ofMain.h"

//TODO:
//+	add variable real line height
//+	add set max height
//+ add multiple instances/panels?

class ofxTextFlow {

public:
	
	ofxTextFlow();
	~ofxTextFlow();

	void update(ofEventArgs &e);
	void draw(ofEventArgs &e);

	static void setTitle(string title);

	bool bTitlePositionBottom = false;
	static void setTitlePositionBottom(bool b);

	static void addText(string txt);
	static void clear();
	
	static void toggleShowing();
	static void setShowing(bool _showing = true);
	static void setShowingBBox(bool _showing = true);

	static void loadFont(string _path, float _size);
	static ofColor getTextColor();
	static void setTextColor(ofColor _color);
	static void setBackgroundColor(ofColor _color);
	static ofColor getBackgroundColor();
	static bool getShowing();
	static int getMaxLineNum();

	static void setPosition(glm::vec2 position);
	static void setPosition(float _x, float _y);
	static void setMaxLineNum(int _maxLineNum);
	static void setMaxHeight(float _maxH);


	static void setTabbed(bool b, int num = 1);
	static void setFloatResolution(int res = 2);
	static void setMarginBorders(int _margin);
	static void setRounded(bool b, float size = 10.0f);
	static void setBBoxWidth(int _w);
	static void setLineHeight(float h = 14);
	static void setShowFPS(bool _showing);

private:

	static ofxTextFlow *singleton;
	static void singletonGenerate();
	ofMutex mutex;
	bool initialized;

	glm::vec2 position;
	string title;

	vector<string> lines;
	int maxLineNum;
	bool showing;
	bool BBoxShowing;

	bool bTabbed = false;
	int tabsNum = 1;
	int fRes = 2;
	int margin = 5;
	bool bRounded = false;
	float roundedSize = 10.0f;
	bool showFPS = false;
	float lineHeight = 14;
	float fontSize = 9;
	int BBoxWidth = 400;

	ofColor textColor, bgColor;
	ofTrueTypeFont font;
	ofRectangle rectBB;
};

