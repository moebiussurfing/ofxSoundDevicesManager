#pragma once

#include "ofMain.h"

#define AMP_GAIN_MAX_POWER 10 /// for plots drawing

class RoundedPlot
{
public:

	RoundedPlot::RoundedPlot()
	{
		setup();
	}

	RoundedPlot::~RoundedPlot()
	{
	}

	ofParameterGroup params_Circled{ "Circled " };
	ofParameter<bool> bEnable{ "Circled", false };
	ofRectangle r;//box

private:

	ofParameter<float> gain{ "Gain", 0, -1, 1 };
	ofParameter<float> radius{ "Radius", 0, 0, 1 };
	ofParameter<float> spread{ "Spread", 0, 0, 1 };
	ofParameter<float> ratioWidth{ "Width", 0.5f, 0, 1 };
	ofParameter<float> rounded{ "Rounded", 0, 0, 1 };
	ofParameter<float> minWidth{ "Min", 0.1f, 0, 1 };
	ofParameter<float> alpha{ "Alpha", 0.5, 0, 1 };
	//ofParameter<bool> bAbs{ "Abs", true };
	ofParameter<bool> bMirror{ "Mirror", true };
	ofParameter<ofColor> cPlot{ "c Plot", ofColor(0, 225), ofColor(0), ofColor(0) };

public:

	void setup()
	{
		params_Circled.add(bEnable);
		params_Circled.add(gain);
		params_Circled.add(radius);
		params_Circled.add(spread);
		params_Circled.add(ratioWidth);
		params_Circled.add(rounded);
		params_Circled.add(minWidth);
		//params_Circled.add(bAbs);
		params_Circled.add(bMirror);
		params_Circled.add(cPlot);
		params_Circled.add(alpha);
	};

	// NOTE: I think that arrays are passed as reference. 
	// don't need to use pointers. 
	void draw(float _plotIn[], int size)
	{
		float _gain = ofMap(gain, gain.getMin(), gain.getMax(), 0, AMP_GAIN_MAX_POWER, true);

		if (bEnable)
		{
			float wb = r.getWidth();
			float hb = r.getHeight();

			ofFill();
			ofPushMatrix();
			ofTranslate(wb / 2.f, 0);

			// Color Plot with alpha
			int _a = ofMap(alpha, 0, 1.f, 0, cPlot.get().a);
			ofColor _c = ofColor(cPlot, _a);
			ofSetColor(_c);

			// Amount of desired elements 
			// (lines or rectangles)
			int _amount = (int)ofMap(spread, 0, 1.f, 364, 8, true);

			//TODO: remake
			int iStep = size / _amount;//it step

			float _radius = radius * MIN(wb / 2.f, hb / 2.f); // constraint into box
			float _radiusGain = _radius * _gain; // apply gain

			int ii = -1;

			for (int i = 0; i < size / 2; i++)
			{
				//TODO:
				if (i % iStep == 0)
				{
					ii++;
				}
				else continue; // skip it from here to end (SIZE_BUFFER)!

				// final elements size!
				float _value = _plotIn[i] * _radiusGain;

				// min
				if (minWidth != 0.f) {
					int _szmin = 5;
					_value = MAX(_value, _szmin +  abs(minWidth * _szmin * _plotIn[i]));
				}

				//// abs
				//if (bAbs) _value = abs(_value);

				float t = -HALF_PI + ofMap(i, 0, (size / 2.f), 0, TWO_PI);
				float x = cos(t) * _radius;
				float y = sin(t) * _radius;
				float a = ofRadToDeg(atan2(y, x));

				ofPushMatrix();
				{
					ofTranslate(x, y);
					ofRotateZDeg(a);

					/*
					// scale
					float scl = 1;
					glScalef(scl, scl, scl);
					*/

					// 1. line
					//ofDrawLine(0, 0, _value, 0);

					// 2. bar
					//int gap = 1;
					float _width = ofMap(ratioWidth, 0, 1.f, 1, _radius / 4.f, true);//width

					float _oy = - _width / 2.f;
					float _ox;
					if (bMirror) _ox = - _value / 2.f;
					else _ox = 0;

					ofRectangle r(_ox, _oy, _value, _width);

					ofFill();
					ofDrawRectRounded(r, rounded * (_width / 2));
				}
				ofPopMatrix();
			}

			/*
			// center label
			ofSetColor(100);
			font.drawString(sortTypeInfo, -(font.stringWidth(sortTypeInfo) / 2), 0);
			*/

			ofPopMatrix();
		}
	};
};