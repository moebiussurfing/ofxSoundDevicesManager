#include "ofxTextFlow.h"

ofxTextFlow *ofxTextFlow::singleton;

ofxTextFlow::ofxTextFlow() {
	singleton = this;
	showing = true;
	BBoxShowing = false;
	textColor.set(255);
	bgColor.set(0, 200);
	maxLineNum = 30;

	position = glm::vec2(10, 10);

	ofAddListener(ofEvents().update, this, &ofxTextFlow::update);
	ofAddListener(ofEvents().draw, this, &ofxTextFlow::draw);

	//TODO:
	//to not draw upfront of everething..
	//ofRemoveListener(ofEvents().draw, this, &ofxTextFlow::draw, OF_EVENT_ORDER_AFTER_APP);
}

ofxTextFlow::~ofxTextFlow() {
	singleton = nullptr;

	ofRemoveListener(ofEvents().update, this, &ofxTextFlow::update);
	ofRemoveListener(ofEvents().draw, this, &ofxTextFlow::draw);
}

void ofxTextFlow::update(ofEventArgs & e) {
}

void ofxTextFlow::draw(ofEventArgs & e) {
	if (showing)
	{
		mutex.lock();

		ofPushStyle();
		ofPushMatrix();

		int yPadTitle = fontSize + lineHeight;//y pad below title
		int heightMax = fontSize + lineHeight * maxLineNum + yPadTitle;
		//int heightMax = lineHeight * maxLineNum;

		//1. bbox
		if (BBoxShowing)
		{
			ofSetColor(bgColor);
			ofFill();

			//bbox
			rectBB = ofRectangle(position.x, position.y, BBoxWidth, heightMax);
			rectBB.x -= margin;
			rectBB.y -= margin;
			rectBB.height += 2 * margin;
			rectBB.width += 2 * margin;

			//to compensate pos after margin
			ofPushMatrix();
			ofTranslate(margin, margin);

			//rounded
			if (bRounded)
			{
				ofDrawRectRounded(rectBB, roundedSize);
			}
			else
			{
				ofDrawRectangle(rectBB);
			}
		}

		//go to setted position
		ofTranslate(position.x, position.y + fontSize);//bc draw from baseline

		//2. print title
		ofSetColor(textColor);

		string str = title;
		if (showFPS)
		{
			str += "\t\t";
			str += ofToString(ofGetFrameRate(), 0);
			str += " FPS";
		}

		float _x, _y;

		//at the top
		if (!bTitlePositionBottom)
		{
			_x = 0;
			_y = 0;
		}
		//at the bottom
		else
		{
			_x = 0;
			_y = heightMax - 3*fontSize;
		}

		if (bTitlePositionBottom)
		{

		}

		if (font.isLoaded()) {
			font.drawString(str, _x, _y);
		}
		else
		{
			ofDrawBitmapString(str, _x, _y);
		}

		//draw log lines

		//direction from top to bottom
		ofTranslate(0, 0);

		//print lines
		for (auto &l : lines) {
			if (font.isLoaded()) {
				font.drawString(l, 0, 0);
			}
			else
			{
				ofDrawBitmapString(l, 0, 0);
			}
			ofTranslate(0, lineHeight);
		}

		if (BBoxShowing)
		{
			ofPopMatrix();
		}

		ofPopMatrix();
		ofPopStyle();

		//-

		mutex.unlock();
	}
}

void ofxTextFlow::addText(string txt) {
	singletonGenerate();

	singleton->mutex.lock();
	auto lines = ofSplitString(txt, "\n");
	for (int i = 0; i < lines.size(); ++i) {
		auto &line = lines[i];
		if (line == "" && i == lines.size() - 1) break;

		singleton->lines.push_back(line);
		if (singleton->lines.size() > singleton->maxLineNum) {
			singleton->lines.erase(singleton->lines.begin());
		}
	}
	singleton->mutex.unlock();

}

void ofxTextFlow::clear() {
	singletonGenerate();
	singleton->lines.clear();
}

void ofxTextFlow::setMaxLineNum(int _maxLineNum) {
	singletonGenerate();
	singleton->maxLineNum = _maxLineNum;
}
void ofxTextFlow::setMaxHeight(float _maxH) {
	singletonGenerate();
	singleton->maxLineNum = (int)singleton->fontSize* (_maxH / singleton->lineHeight);

}

int ofxTextFlow::getMaxLineNum() {
	singletonGenerate();
	return singleton->maxLineNum;
}

void ofxTextFlow::setShowing(bool _showing) {
	singletonGenerate();
	singleton->showing = _showing;
}

void ofxTextFlow::setShowingBBox(bool _showing) {
	singletonGenerate();
	singleton->BBoxShowing = _showing;
}

bool ofxTextFlow::getShowing() {
	singletonGenerate();
	return singleton->showing;
}

void ofxTextFlow::toggleShowing() {
	singletonGenerate();
	singleton->showing = !singleton->showing;
}

void ofxTextFlow::singletonGenerate() {
	if (singleton == nullptr) {
		singleton = new ofxTextFlow();
	}
}

//colors
void ofxTextFlow::setTextColor(ofColor _color) {
	singletonGenerate();
	singleton->textColor.set(_color);
}

ofColor ofxTextFlow::getTextColor() {
	singletonGenerate();
	return singleton->textColor;
}

void ofxTextFlow::setBackgroundColor(ofColor _color) {
	singletonGenerate();
	singleton->bgColor = _color;
}

ofColor ofxTextFlow::getBackgroundColor() {
	singletonGenerate();
	return singleton->bgColor;
}

void ofxTextFlow::setTitlePositionBottom(bool b) {
	singletonGenerate();
	singleton->bTitlePositionBottom = b;
}

void ofxTextFlow::setTitle(string _title) {
	singletonGenerate();
	singleton->title = _title;
}

void ofxTextFlow::loadFont(string _path, float _size) {
	singletonGenerate();

	singleton->fontSize = _size;
	bool b = singleton->font.load(_path, _size);
	if (!b) singleton->font.load(OF_TTF_MONO, _size);//avoid crash when non existing font
	//singleton->font.load(_path, _size);
}

//layout
void ofxTextFlow::setPosition(float _x, float _y) {
	singletonGenerate();
	singleton->position = glm::vec2(_x, _y);
}

void ofxTextFlow::setPosition(glm::vec2 _position) {
	singletonGenerate();
	singleton->position = _position;
}

void ofxTextFlow::setBBoxWidth(int w) {
	singletonGenerate();
	singleton->BBoxWidth = w;
}

void ofxTextFlow::setLineHeight(float h) {
	singletonGenerate();
	singleton->lineHeight = h;
}

void ofxTextFlow::setMarginBorders(int _margin) {
	singletonGenerate();
	singleton->margin = _margin;
}

void ofxTextFlow::setRounded(bool b, float size) {
	singletonGenerate();
	singleton->bRounded = b;
	singleton->roundedSize = size;
}

void ofxTextFlow::setTabbed(bool b, int num) {
	singletonGenerate();
	singleton->bTabbed = b;
	singleton->tabsNum = num;
}

void ofxTextFlow::setFloatResolution(int res) {
	singletonGenerate();
	singleton->fRes = res;
}

void ofxTextFlow::setShowFPS(bool _showing) {
	singletonGenerate();
	singleton->showFPS = _showing;
}