#include "DropDown.h"

DropDown::DropDown()
{

}

/// setup
void DropDown::setup(vector<string> _stringList) {
	ofTrueTypeFont font;
	setup(_stringList, 0, font);

}

void DropDown::setup(vector<string> _stringList, int _currInd) {
	ofTrueTypeFont font;
	setup(_stringList, _currInd, font);

}

void DropDown::setup(vector<string> _stringList, int _currInd, ofTrueTypeFont _textFont) {
	setup(_stringList, _currInd,  _textFont, ofColor(230), ofColor::gray);
}

void DropDown::setup(vector<string> _stringList, int _currInd, ofTrueTypeFont _textFont, ofColor _textColor, ofColor _backgroundColor) {
	// copy variables
	//textString = toUpperCase(_textString);
	stringList = _stringList;
	currInd = _currInd;
	textFont = _textFont;
	textColor = _textColor;
	backgroundColor = _backgroundColor;

	// set up background rectangle size
	padX = ofMap(_textFont.getSize(), 5, 11, 5, 15);
	padY = 8;
	lineHeight = _textFont.getLineHeight();
	float maxW = 0;
	for (int i = 0; i < stringList.size(); i++) {
		float strW = textFont.stringWidth(stringList[i]);
		if (strW > maxW) maxW = strW;
	}
	buttonW = maxW + 4 * padX;
	buttonHSmall = lineHeight + 2 * padY + ofMap(_textFont.getSize(), 5, 11, 1, 5);
	buttonHExtended = (lineHeight + 2 * padY) * stringList.size() + ofMap(_textFont.getSize(), 5, 11, 1, 5);
	buttonH = buttonHSmall;
	// set color
	hoveredColor = _backgroundColor;
	hoveredColor.setBrightness(hoveredColor.getBrightness() + 30);
	// animation
	motionRatio = 0.8;
	motionRatioGizmo = 0.3;
	animTime = (int)(ofGetFrameRate() * 0.3);
	deltaT = 1 / (float)animTime;
}

/// draw
void DropDown::draw(float posX, float posY, ofMatrix4x4 transMatrix) {
	/*
	// debug
	ofFill();
	ofDrawCircle(posX, posY, 2);
	*/

	// get absolute position of hover rectangle
	ofPoint     buttonPosAbs = ofPoint(posX, posY) * transMatrix;

	// set target height of button
	float buttonHTarget = hovered ? buttonHExtended : buttonHSmall;
	buttonH *= motionRatio;
	buttonH += (1 - motionRatio) * buttonHTarget;

	// check if string is hovered
	hovered = (ofGetMouseX() < buttonPosAbs.x + buttonW) &&
		(ofGetMouseX() > buttonPosAbs.x) &&
		(ofGetMouseY() < buttonPosAbs.y + buttonH) &&
		(ofGetMouseY() > buttonPosAbs.y);

	// check if button is clicked on
	clicked = false;
	if (hovered && ofGetMousePressed() && !mousePressedPrev)
	{
		clicked = true;
	}

	// update stuff
	mousePressedPrev = ofGetMousePressed();

	// draw button
	ofPushStyle();
	ofFill();

	ofPushMatrix();
	ofTranslate(posX, posY);

	// draw background rectangle
	int numIter = 5;
	for (int i = 0; i < numIter; i++) {
		float alpha = ofMap(i, 0, numIter - 1, 20, 0);
		float w = ofMap(i, 0, numIter - 1, buttonW*1.0, buttonW*0.9);
		float hOffset = ofMap(i, 0, numIter - 1, 0, 10);
		float c = ofMap(i, 0, numIter - 1, 0, 200);
		float xOffset = (buttonW - w) * 0.5;
		ofSetColor(c, alpha);
		ofDrawRectRounded(xOffset, 0, w, buttonH + hOffset, buttonHSmall * 0.1);
	}
	ofSetColor(backgroundColor);
	ofDrawRectRounded(0, 0, buttonW, buttonH, buttonHSmall * 0.1);

	// draw string
	float offsetY = padY + lineHeight;
	ofSetColor(textColor);
	textFont.drawString(stringList[currInd], padX, offsetY);
	offsetY += padY;

	if (!hovered) {
		ofPopMatrix();
		ofPopStyle();
		return;
	}

	// draw line
	ofSetColor(textColor);
	ofSetLineWidth(1);
	ofDrawLine(padX, offsetY, buttonW - padX, offsetY);

	// go over each element in list
	for (int i = 0; i < stringList.size(); i++) {
		// skip the one currently selected
		if (i == currInd ||	buttonH < offsetY + lineHeight) continue;

		// draw text
		offsetY += padY + lineHeight;
		ofSetColor(textColor);
		textFont.drawString(stringList[i], padX, offsetY);
		offsetY += padY;

		// debug
		/*
		ofNoFill();
		ofSetColor(textColor);
		ofSetLineWidth(1);
		ofDrawLine(0, offsetY, buttonW, offsetY);
		ofDrawLine(0, offsetY - lineHeight - 2*padY, buttonW, offsetY - lineHeight - 2 * padY);
		ofDrawLine(0, offsetY, 0, offsetY - lineHeight - 2*padY);
		ofDrawLine(buttonW, offsetY, buttonW, offsetY - lineHeight - 2*padY);
		*/

		// check if hovering over element
		bool hoveredInList = (ofGetMouseX() < buttonPosAbs.x + buttonW) &&
			(ofGetMouseX() > buttonPosAbs.x) &&
			(ofGetMouseY() < buttonPosAbs.y + offsetY) &&
			(ofGetMouseY() > buttonPosAbs.y + offsetY - 2 * padY - lineHeight);

		// ignore if we don't hover
		if (!hoveredInList) continue;
			
		// display gizmo
		float strW = textFont.stringWidth(stringList[i]);
		float strH = textFont.stringHeight(stringList[i]);
		float ptX = 2 * padX + strW;
		float ptY = (2 * offsetY - lineHeight - 2 * padY)*0.5 + strH * 0.5;
		gizmoX *= motionRatioGizmo;
		gizmoX += (1 - motionRatioGizmo)*ptX;
		gizmoY *= motionRatioGizmo;
		gizmoY += (1 - motionRatioGizmo)*ptY;
		ofSetColor(textColor);
		ofDrawTriangle(gizmoX, gizmoY, gizmoX + 0.8 * padX, gizmoY + padX * 0.5, gizmoX + 0.8 * padX, gizmoY - padX * 0.5);

		// if clicked, change current index
		if (clicked) {
			currInd = i;
		}
	}
	

	ofPopMatrix();
	ofPopStyle();
}

/// getter for value
string DropDown::getValueString() {
	return stringList[currInd];
}

/// getter for value
int DropDown::getValueInt() {
	return currInd;
}
void DropDown::setValueInt(int i) {
	currInd = i;
}

/// getter for height
float DropDown::getHeight() {
	return buttonH;
}

/// setter for color
void DropDown::setTextColor(ofColor col) {
	textColor = col;
}

string DropDown::toUpperCase(string str) {
	string strUpper = "";
	for (int i = 0; i < str.length(); i++)
	{
		strUpper += toupper(str[i]);
	}

	return strUpper;
}
