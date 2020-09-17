#include "Button.h"

Button::Button()
{

}

/// setup
void Button::setup(){
    ofTrueTypeFont font;
    noFont = true;
    setup("", font);

}

void Button::setup(string _textString, ofTrueTypeFont _textFont){
    setup(_textString, _textFont, ofColor(230), ofColor::gray);
}

void Button::setup(string _textString, ofTrueTypeFont _textFont, ofColor _textColor, ofColor _backgroundColor){
    // copy variables
    textString      = toUpperCase(_textString);
    textFont        = _textFont;
    textColor       = _textColor;
    backgroundColor = _backgroundColor;

    // set up background rectangle size
    padX = ofMap(_textFont.getSize(), 5, 11, 5, 15);
    padY = 8;
    lineHeight = _textFont.getLineHeight();
    buttonW = _textFont.stringWidth(textString) + 2 * padX;
    buttonH = lineHeight + 2 * padY + ofMap(_textFont.getSize(), 5, 11, 1, 5) ;
    if(noFont){
        buttonW = 75;
        buttonH = 25;
    }
    // set color
    hoveredColor = _backgroundColor;
    hoveredColor.setBrightness(hoveredColor.getBrightness() + 30);
    // animation
    animTime = (int) (ofGetFrameRate() * 0.3);
    deltaT = 1 / (float) animTime;
}

/// draw
bool Button::draw(float posX, float posY, ofMatrix4x4 transMatrix){
     /*
     // debug
     ofFill();
     ofDrawCircle(posX, posY, 2);
     */

    // get absolute position of hover rectangle
    ofPoint     buttonPosAbs = ofPoint(posX, posY) * transMatrix;

    // check if string is hovered
    hovered = (ofGetMouseX() < buttonPosAbs.x + buttonW)  &&
              (ofGetMouseX() > buttonPosAbs.x)                         &&
              (ofGetMouseY() < buttonPosAbs.y + buttonH) &&
              (ofGetMouseY() > buttonPosAbs.y);

    // check if button is clicked on
    clicked = false;
    if(hovered && ofGetMousePressed() && !mousePressedPrev)
    {
        clicked = true;
        circlePos = ofPoint(ofGetMouseX() - buttonPosAbs.x, ofGetMouseY() - buttonPosAbs.y);
        animCounter = 0;
    }

    // run animation
    animCounter += deltaT;
    animRunning = animCounter <= 1;

    // update stuff
    mousePressedPrev = ofGetMousePressed();

    // draw button
    ofPushStyle();
    ofFill();

        ofPushMatrix();
        ofTranslate(posX, posY);

            // draw background rectangle
            ofSetColor(hovered ? backgroundColor : hoveredColor);
            ofDrawRectRounded(0, 0, buttonW, buttonH, buttonH * 0.1);

            if(animRunning) {
                ofSetColor(255, 255, 255, ofMap(animCounter, 0, 1, 20, 0));
                ofDrawCircle(circlePos, animCounter * 35);
            }

            if(!noFont) {
                // draw string
                ofSetColor(textColor);
                textFont.drawString(textString, padX, padY + lineHeight);
            }

        ofPopMatrix();
    ofPopStyle();

    return clicked;
}

/// getter for value
bool Button::getValue(){
    return clicked;
}

/// getter for height
float Button::getHeight() {
	return buttonH;
}

/// setter for color
void Button::setTextColor(ofColor col){
    textColor = col;
}

string Button::toUpperCase(string str){
    string strUpper = "";
    for( int i=0; i<str.length(); i++ )
    {
        strUpper += toupper( str[ i ] );
    }

    return strUpper;
}
