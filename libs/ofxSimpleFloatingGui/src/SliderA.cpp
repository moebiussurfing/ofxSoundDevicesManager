#include "SliderA.h"

SliderA::SliderA()
{

}

/// setup
void SliderA::setup(float _minValue, float _maxValue, float _value, ofTrueTypeFont _font) {
    setup(_minValue, _maxValue, _value, _font, ofColor::black, ofColor::black);
}

void SliderA::setup(float _minValue, float _maxValue, float _value, ofTrueTypeFont _font, ofColor _textColor, ofColor _triangleColor)
{
    // copy arguments
    minValue = _minValue;
    maxValue = _maxValue;
    value = ofClamp(_value, _minValue, _maxValue);
    textFont = _font;
    textColor = _textColor;
    triangleColor = _triangleColor;

    // init variables
    valueAsString = ofToString(_value, 0);
    fontH = _font.stringHeight(valueAsString);
    stringW = _font.stringWidth(valueAsString);
        // triangle design
    triangleH = fontH * 0.8;
    triangleW = triangleH * sqrt(3) / 2;
    dragColor = _triangleColor;
    dragColor.setBrightness(100);
    maxColor = _triangleColor;
    maxColor.setBrightness(50);
    padX = _font.getSize();
}

/// draw
float SliderA::draw(float posX, float posY, ofMatrix4x4 transMatrix){
    /*
    // debug
    ofFill();
    ofDrawCircle(posX, posY, 2);
    */

    // set bounding box
    boundingBox = ofRectangle(posX, posY, stringW, fontH);

    // get absolute position of bounding box
    ofPoint bbAbs = ofPoint(boundingBox.getTopLeft()) * transMatrix;

    // check if hovered
    hovered = (ofGetMouseX() < bbAbs.x + stringW)   &&
              (ofGetMouseX() > bbAbs.x)             &&
              (ofGetMouseY() < bbAbs.y + fontH)     &&
              (ofGetMouseY() > bbAbs.y);

    // check if dragged
    if(hovered && ofGetMousePressed() && !mousePressedPrev)
    {
        dragged = true;
    }
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
        float valueUpdate = valueStart + diff / ofGetWidth() * (maxValue - minValue);
        value = ofClamp(valueUpdate, minValue, maxValue);

        // update string state of value and updates stringW everytime the length changes
        valueAsString = ofToString(value - 0.5f, 0);
        if(valueAsString.length() != stringLengthPrev) {
            stringW = textFont.stringWidth(valueAsString);
        }
        stringLengthPrev = valueAsString.length();
    } else {
        diff = 0;
    }

    // update stuff
    draggedPrev = dragged;
    mousePressedPrev = ofGetMousePressed();

    ofPushStyle();
    ofFill();
        ofPushMatrix();
        ofTranslate(posX, posY);

            // draw triangles
            if(hovered || dragged) {
                // set color
                ofSetColor((value == maxValue || value == minValue) ? maxColor : dragColor);

                if(diff <= 0) {
                    // draw 1st triangle
                    ofPushMatrix();
                    ofTranslate(- triangleW - padX, fontH / 2);
                        ofDrawTriangle(0, 0, triangleW, - triangleH / 2, triangleW, triangleH / 2);
                    ofPopMatrix();
                }

                if(diff >= 0) {
                    // draw 2nd triangle
                    ofPushMatrix();
                    ofTranslate(padX + stringW, fontH / 2);
                        ofDrawTriangle(0, - triangleH / 2, 0, triangleH / 2, triangleW, 0);
                    ofPopMatrix();
                }
            }

            // draw text
            ofPushMatrix();
            ofTranslate(0, fontH);
                ofSetColor(textColor);
                textFont.drawString(valueAsString, 0, 0);
            ofPopMatrix();

            /*
            // debug stuff
            ofNoFill();
            ofSetColor(ofColor::black);
            ofDrawRectangle(0, 0, stringW, fontH);
            */

        ofPopMatrix();
    ofPopStyle();

    // return value
    return value;
}

/// getter for value
float SliderA::getValue(){
    return value;
}

/// getter for max value
float SliderA::getMaxValue(){
    return maxValue;
}

/// getter for min value
float SliderA::getMinValue(){
    return minValue;
}
