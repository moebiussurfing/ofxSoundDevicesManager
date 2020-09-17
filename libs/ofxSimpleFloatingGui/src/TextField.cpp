#include "TextField.h"

TextField::TextField()
{

}

/// setup methods
void TextField::setup(ofTrueTypeFont _stringFont){
    setup(10, _stringFont);
}

void TextField::setup(int _maxChar, ofTrueTypeFont _stringFont){
    setup(" ", _maxChar, _stringFont);
}

void TextField::setup(string _displayedString, int _maxChar, ofTrueTypeFont _stringFont)
{
    setup(_displayedString, _maxChar, _stringFont, ofColor::black, ofColor(230));
}

void TextField::setup(string _displayedString, int _maxChar, ofTrueTypeFont _stringFont, ofColor _stringColor, ofColor _backgroundColor)
{
    ofAddListener(ofEvents().keyPressed, this, &TextField::keyPressed);

    displayedString = _displayedString.substr(0, _maxChar);
    stringFont = _stringFont;
    stringColor = _stringColor;
	backgroundColor = _backgroundColor;
    maxChar = _maxChar;

    // set up background rectangle size
    padX = ofMap(_stringFont.getSize(), 5, 11, 5, 15);
    padY = 15;
    lineHeight = stringFont.stringHeight(displayedString);
    float rectW = stringFont.stringWidth(displayedString) + 2 * padX;
    float rectH = lineHeight + 2 * padY;

    // init other variables
    maxSize = getMaxSize(_stringFont, _maxChar) + 2 * padX;
    backgroundRect = ofRectangle(0, 0, rectW, rectH);
    animTime = (int) (ofGetFrameRate() * 0.2);
    deltaT = 1 / (float) animTime;
    nChar = displayedString.size();
    blinkThresh = (int) (ofGetFrameRate() * 0.6);   // we want the blink to be 0.6 seconds long
}

string TextField::draw(float posX, float posY, ofMatrix4x4 transMatrix){

    // get absolute position of hover rectangle
    ofPoint     backgroundRectPosAbs = ofPoint(posX, posY) * transMatrix;

    // check if string is hovered
    hovered = (ofGetMouseX() < backgroundRectPosAbs.x + backgroundRect.width)  &&
              (ofGetMouseX() > backgroundRectPosAbs.x)                         &&
              (ofGetMouseY() < backgroundRectPosAbs.y + backgroundRect.height) &&
              (ofGetMouseY() > backgroundRectPosAbs.y);

    // if we click while hovered we enter edit mode
    if(ofGetMousePressed() && !mousePressedPrev)
    {
        editMode = hovered;
    }

    // if we just entered edit mode
    if(editMode && !editModePrev)
    {
        // we update editString with displayedString
        editString = displayedString;

        // backgroundRect.width is gonna be animated to its maxSize
        targetWidth = maxSize;
        deltaWidth = (targetWidth - backgroundRect.getWidth()) / animTime;
        animCounter = 0;

        // init blinking of cursor
        blinkTime = 0;
        blink = true;
    }

    // if we just quit edit mode
    if(!editMode && editModePrev)
    {
        // we save change into displayedString
        displayedString = editString;

        // backgroundRect.width is gonna be animated to the width of current string
        targetWidth = cursorX + 2 * padX;
        deltaWidth = (targetWidth - backgroundRect.getWidth()) / animTime;
        animCounter = 0;
    }

    // if we are currently in edit mode
    if(editMode)
    {
        // get string rect
        cursorX = stringFont.stringWidth(editString);

        // handle blinking
        blinkTime++;
        if(blinkTime >= blinkThresh) {
            blink = !blink;
            blinkTime = 0;
        }
    }

    // update animation counter and bool
    animCounter += deltaT;
    animRunning = (animCounter <= 1);

    // animate backgroundRect.width when animation is running
    if(animRunning) {
        backgroundRect.setWidth(backgroundRect.getWidth()+ deltaWidth) ;
    }

    ofPushStyle();
    ofSetLineWidth(1);

        ofPushMatrix();
        ofTranslate(posX, posY);

            // debug
            //ofDrawCircle(0, 0, 2);

            // draw background rectangle
            ofFill();
            ofSetColor(backgroundColor);
            ofDrawRectRounded(backgroundRect, backgroundRect.height / 2);

            // display a boundary rectangle when text field is hovered, being edited or animation is running
            if(hovered || editMode || animRunning) {
                ofNoFill();
                ofSetColor(ofColor::gray);
                ofDrawRectRounded(backgroundRect, backgroundRect.height / 2);
            }

            // draw string
            ofSetColor(stringColor);
            stringFont.drawString(editMode ? editString : displayedString, padX, padY + lineHeight);

            // draw line
            if(editMode && blink){
                ofSetLineWidth(2);
                ofSetColor(ofColor::black);
                ofDrawLine(cursorX + padX + 3, 0, cursorX + padX + 3, backgroundRect.height);
            }

            /*
            // debug stuff
            ofNoFill();
            ofSetLineWidth(1);
            ofSetColor(ofColor::black);
            ofDrawRectangle(padX, padY, stringFont.stringWidth(editMode ? editString : displayedString), stringFont.getLineHeight());

            ofSetColor(ofColor::black);
            ofDrawLine(0, rectH / 2, rectW, rectH / 2);
            ofDrawLine(rectW / 2, 0, rectW / 2, rectH);
            */

        ofPopMatrix();
    ofPopStyle();

    // update states
    mousePressedPrev    = ofGetMousePressed();
    editModePrev        = editMode;

    // we return displayedString, this means that this value changed when we leave edit mode
    return displayedString;
}

// getter
string TextField::getValue(){
    return displayedString;
}


float TextField::getMaxSize(ofTrueTypeFont _font, int _sz) {
    std::stringstream ss;
    for(int i = 0; i < _sz; i++) {
        ss << 'w';
    }
    return _font.stringWidth(ss.str());
}

void TextField::keyPressed(ofKeyEventArgs& eventArgs) {
    // debug
    //ofLog() << eventArgs.key;

    if(editMode) {
        // delete if backspace
        if(eventArgs.key == 8) {
            // remove last char
            if(editString.size() > 0) {
                editString = editString.substr(0, editString.size()-1);
                nChar -= 1;
            }
        }

        // only consider a-z or 0-9 or spacebar
        if((nChar < maxChar) && ((eventArgs.key >= 97 && eventArgs.key <= 122) || (eventArgs.key >= 48 && eventArgs.key <= 57) || eventArgs.key == 32)) {
            editString += ofToString((char) eventArgs.key);
            nChar += 1;
        }

        // if enter, exit edit mode
        if(eventArgs.key == 13){
            editMode = !editMode;
        }
    }
}
