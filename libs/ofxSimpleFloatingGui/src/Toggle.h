#ifndef TOGGLE_H
#define TOGGLE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ofMain.h"

class Toggle
{
public:
    // methods
        // setup
    void setup();
    void setup(bool _value);
    void setup(bool _value, ofColor _trueColor);
        // draw
    bool draw(float posX, float poxY, ofMatrix4x4 transMatrix);

	//TODO:
    bool getValue();
	void setValue(bool b);
    
	Toggle();
private:
    // variables
        // value
    bool        value;
        // design
    ofColor     trueColor, falseColor;
    int         circlePad, rectW, rectH;
        // states
    bool        hovered, clicked, mousePressedPrev;
        // toggle animation
    int         animTime;
    float       animCounter, deltaT;
    bool        animRunningUP ,animRunningDOWN;
};

#endif // TOGGLE_H
