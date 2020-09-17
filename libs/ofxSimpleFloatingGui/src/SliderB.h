#ifndef SliderB_H
#define SliderB_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ofMain.h"

class SliderB
{
public:
	// methods
	// setup
	void setup();
	void setup(float _value);
	void setup(float _value, int _sliderW, int _sliderH, int _sliderR);
	void setup(float _value, ofColor _rectColor, ofColor _ellColor);
	void setup(float _value, int _sliderW, ofColor _rectColor, ofColor _ellColor);
	void setup(float _value, int _sliderW, int _sliderH, int _sliderR, ofColor _rectColor, ofColor _ellColor);
	// draw
	float draw(float posX, float poxY, ofMatrix4x4 transMatrix);

	SliderB();

	//TODO:
	// getter
	float getValue();
	void setValue(float f);
	float getHeight();

	//void setVisibleBall(bool b)
	//{
	//	bBallHidden = !b;
	//}

private:
	// variables
	// value
	float       value, valueStart;
	// design
	int         sliderW, sliderH, sliderR;
	float       posSlider;
	ofColor     rectColor, ellColor, ellColorHovered;
	// states
	bool        rectHovered, ellHovered, dragged, draggedPrev, mousePressedPrev;
	// interactivity
	ofRectangle boundingBoxRect, boundingBoxEll;
	float       diff;
	ofVec2f     mousePosStart;

	//bool bBallHidden = false;
};

#endif // SliderB_H
