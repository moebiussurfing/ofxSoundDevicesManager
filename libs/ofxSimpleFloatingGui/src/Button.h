#ifndef BUTTON_H
#define BUTTON_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ofMain.h"
#include "ofTrueTypeFont.h"

class Button
{
public:
    // methods
        // setup
    void setup();
    void setup(string _textString, ofTrueTypeFont _textFont);
    void setup(string _textString, ofTrueTypeFont _textFont, ofColor _textColor, ofColor _backgroundColor);
        // draw
    bool draw(float posX, float posY, ofMatrix4x4 transMatrix);
        // getter
    bool getValue();
	float getHeight();
        // setter
    void setTextColor(ofColor col);

    Button();

private:
    // methods
    string toUpperCase(string str);

    // variables
        // design
    bool            noFont = false;
    string          textString;
    float           buttonW, buttonH, padX, padY, lineHeight;
    ofColor         textColor, backgroundColor, hoveredColor;
    ofTrueTypeFont  textFont;
        // states
    bool            clicked = false;
    bool            hovered, mousePressedPrev;
        // interaction
    ofPoint         circlePos;
    float           animTime, animCounter, deltaT;
    bool            animRunning;

};

#endif // BUTTON_H
