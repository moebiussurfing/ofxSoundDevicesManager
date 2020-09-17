#ifndef DROPDOWN_H
#define DROPDOWN_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ofMain.h"
#include "ofTrueTypeFont.h"

class DropDown
{
public:
	// methods
		// setup
	void setup(vector<string> _stringList);
	void setup(vector<string> _stringList, int _currInd);
	void setup(vector<string> _stringList, int _currInd, ofTrueTypeFont _textFont);
	void setup(vector<string> _stringList, int _currInd, ofTrueTypeFont _textFont, ofColor _textColor, ofColor _backgroundColor);
	// draw
	void draw(float posX, float posY, ofMatrix4x4 transMatrix);
	// getter
	string getValueString();
	int getValueInt();
	void setValueInt(int i);

	float getHeight();
	// setter
	void setTextColor(ofColor col);

	DropDown();

private:
	// methods
	string toUpperCase(string str);

	// variables
		// design
	vector<string>  stringList;
	float           buttonW, buttonH, buttonHSmall, buttonHExtended, padX, padY, lineHeight;
	ofColor         textColor, backgroundColor, hoveredColor;
	ofTrueTypeFont  textFont;
	// states
	int				currInd;
	bool            clicked = false;
	bool            hovered, mousePressedPrev;
	// interaction
	ofPoint         circlePos;
	float			motionRatio;
	float           animTime, animCounter, deltaT;
	bool            animRunning;
	float			gizmoX, gizmoY, motionRatioGizmo;

};

#endif // BUTTON_H
