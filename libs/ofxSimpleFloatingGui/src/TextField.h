#ifndef TextField_H
#define TextField_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ofMain.h"
#include "ofTrueTypeFont.h"

class TextField
{
public:
    // methods
        // setup methods
    void setup(ofTrueTypeFont _stringFont);
    void setup(int _maxChar, ofTrueTypeFont _stringFont);
    void setup(string _displayedString, int _maxChar, ofTrueTypeFont _stringFont);
    void setup(string _displayedString, int _maxChar, ofTrueTypeFont _stringFont, ofColor _stringColor, ofColor _backgroundColor);
        // draw
    string draw(float posX, float posY, ofMatrix4x4 transMatrix);
        // getters
    string getValue();

    TextField();

private:
    // methods
    void keyPressed(ofKeyEventArgs& eventArgs);
    float getMaxSize(ofTrueTypeFont _font, int _sz);

    // variables
        // value
    string          displayedString;
    string          editString;
    int             maxChar;
    int             nChar;
        // design
    ofTrueTypeFont  stringFont;
    ofColor         stringColor, backgroundColor;
    ofRectangle     backgroundRect;
    int             padX, padY;
    float           maxSize, targetWidth, deltaWidth, lineHeight;
        // animation
    float           animTime, animCounter, deltaT;
    bool            animRunning;
        // blinking
    int             blinkTime, blinkThresh;
    float           cursorX;
    bool            blink;
        // states
    bool            hovered, editMode, editModePrev, mousePressedPrev;
};

#endif // TextField_H
