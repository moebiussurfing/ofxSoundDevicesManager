#include "SliderB.h"

SliderB::SliderB()
{

}

/// setup methods
void SliderB::setup()
{
    setup(0.0f);
}

void SliderB::setup(float _value)
{
    setup(_value, 100, 5, 7);
}

void SliderB::setup(float _value, int _sliderW, int _sliderH, int _sliderR)
{
    setup(_value, _sliderW, _sliderH, _sliderR, ofColor::gray, ofColor::black);
}

void SliderB::setup(float _value, ofColor _rectColor, ofColor _ellColor)
{
    setup(_value, 100, 5, 7, _rectColor, _ellColor);
}

void SliderB::setup(float _value, int _sliderW, ofColor _rectColor, ofColor _ellColor)
{
    setup(_value, _sliderW, 5, 7, _rectColor, _ellColor);
}

void SliderB::setup(float _value, int _sliderW, int _sliderH, int _sliderR, ofColor _rectColor, ofColor _ellColor)
{
    // copy values
    value = ofClamp(_value, 0, 1);
    rectColor = _rectColor;
    ellColor = _ellColor;
    sliderW = _sliderW;
    sliderH = _sliderH;
    sliderR = _sliderR;

    // init variables
    posSlider = _value * sliderW;
}

/// draw method
float SliderB::draw(float posX, float posY, ofMatrix4x4 transMatrix){
    /*
    // debug
    ofFill();
    ofDrawCircle(posX, posY, 2);
    */

    // compute position of slider
    ofPoint posRectAbs = ofPoint(posX, posY) * transMatrix;
    ofPoint posSlider = ofPoint(posX + value * sliderW, posY + sliderH / 2);
    ofPoint posSliderAbs = posSlider * transMatrix;

    // check if hovered
    ellHovered = (ofGetMouseX() < posSliderAbs.x + sliderR) &&
                 (ofGetMouseX() > posSliderAbs.x - sliderR) &&
                 (ofGetMouseY() < posSliderAbs.y + sliderR) &&
                 (ofGetMouseY() > posSliderAbs.y - sliderR);

    rectHovered = (ofGetMouseX() < posRectAbs.x + sliderW)     &&
                  (ofGetMouseX() > posRectAbs.x)                &&
                  (ofGetMouseY() < posRectAbs.y + sliderH + 5)     &&
                  (ofGetMouseY() > posRectAbs.y - 5);

    // check if ellipse is clicked on
    if(ellHovered && ofGetMousePressed() && !mousePressedPrev)
    {
        dragged = true;
        rectHovered = false;
    }

    // check if slider rectangle is clicked on
    if(rectHovered && ofGetMousePressed() && !mousePressedPrev)
    {
        // get where the slider is clicked on
        diff = ofGetMouseX() - posSliderAbs.x;

        // update value
        float valueUpdate = value + diff / sliderW;
        value = ofClamp(valueUpdate, 0, 1);
        dragged = true;
    }

    // this registers when we unclick
    dragged &= ofGetMousePressed();

    // store mouse posString and value when dragging starts
    if(dragged && !draggedPrev)
    {
        mousePosStart = ofVec2f(ofGetMouseX(), ofGetMouseY());
        valueStart = value;
    }

    // compare mouse posStringition to stored posStringition and update value accordingly
    if(dragged)
    {
        diff = ofGetMouseX() - mousePosStart.x;
        float valueUpdate = valueStart + diff / sliderW;
        value = ofClamp(valueUpdate, 0, 1);
    }

    // set style
    ofPushStyle();

        ofFill();

        // draw slider
        ofSetColor(ellColor);
        ofDrawRectRounded(posX, posY, posSlider.x - posX, sliderH, 10);
        ofSetColor(rectColor);
        ofDrawRectRounded(posSlider.x, posY, sliderW - (posSlider.x - posX), sliderH, 10);

        // draw ellipse
		//if (!bBallHidden)
		{
			ofSetColor(ellColor);
			ofDrawCircle(posSlider, sliderR);

			//trying rectangle form
			//float _gap = 20;
			//ofDrawRectangle(posSlider.x- _gap, posSlider.y- sliderH*0.5, 2*_gap, sliderH);
		}

    // reset style
    ofPopStyle();

    // update stuff
    draggedPrev = dragged;
    mousePressedPrev = ofGetMousePressed();

    // return value
    return value;
}


//TODO:
float SliderB::getValue(){
    return value;
}
void SliderB::setValue(float f)
{
	value = f;
}
float SliderB::getHeight()
{
	return 2 * sliderR;
}
