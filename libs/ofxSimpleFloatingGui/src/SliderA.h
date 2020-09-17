#ifndef SliderA_H
#define SliderA_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ofMain.h"
#include "ofTrueTypeFont.h"

class SliderA
{
public:
    // methods
        // setup
    void setup(float _minValue, float _maxValue, float _value, ofTrueTypeFont _font);
    void setup(float _minValue, float _maxValue, float _value, ofTrueTypeFont _font, ofColor _textColor, ofColor _triangleColor);
        // draw
    float draw(float posX, float posY, ofMatrix4x4 transMatrix);
        // getters
    float getValue();
    float getMinValue();
    float getMaxValue();

    SliderA();

private:
    // variables
        // value
    float       value, minValue, maxValue;
    string      valueAsString;
        // design
    float           triangleW, triangleH, fontH, stringW, padX;
    ofColor         textColor, triangleColor, dragColor, maxColor;
    ofTrueTypeFont  textFont;
    int             stringLengthPrev;
        // interactivity
    ofRectangle boundingBox;
    ofVec2f     mousePosStart;
    float       valueStart, diff;
        // states
    bool        hovered, dragged, draggedPrev, mousePressedPrev;

};

#endif // SliderA_H
