#pragma once

#include "ofMain.h"

#define SIZE_BUFFER 4096
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

	//TODO:
	/*
	float* plotIn; // make this bigger, just in case

	void setPlotPtr(float *_plotPtr) {
		//plotIn = _plotPtr;
		plotIn = &_plotPtr[0];
	};
	*/

	//float plotIn[SIZE_BUFFER];

	ofParameterGroup params_Circled{ "Circled" };
	ofParameter<float> gain{ "Gain", 0, -1, 1 };
	ofParameter<bool> W_bCircled{ "Circled", false };
	ofParameter<float> W_CircledRadius{ "Radius", 0, 0, 1 };
	ofParameter<float> W_CircledSize{ "Size", 0, 0, 1 };
	ofParameter<float> W_Spread2{ "Spread2", 0, 0, 1 };
	ofParameter<float> W_RatioWidth2{ "RatioWidth2", 0.5f, 0, 1 };
	ofParameter<float> W_RatioRad2{ "RatioRadius2", 0.5f, 0, 1 };
	ofParameter<float> W_Rounded2{ "Rounded2", 0, 0, 1 };
	ofParameter<float> W_WidthMin2{ "Min2", 0.1f, 0, 1 };
	ofParameter<float> W_Alpha{ "Alpha", 0.5, 0, 1 };
	ofParameter<bool> W_bAbs{ "Abs", true };
	ofParameter<ofColor> cPlot{ "c Plot", ofColor(0, 225), ofColor(0), ofColor(0) };

	ofRectangle r;

	void setup()
	{
		params_Circled.add(W_bCircled);
		params_Circled.add(gain);
		params_Circled.add(cPlot);
		params_Circled.add(W_Alpha);
		params_Circled.add(W_bAbs);
		params_Circled.add(W_CircledRadius);
		params_Circled.add(W_CircledSize);
		params_Circled.add(W_Spread2);
		params_Circled.add(W_RatioWidth2);
		params_Circled.add(W_RatioRad2);
		params_Circled.add(W_Rounded2);
		params_Circled.add(W_WidthMin2);
	};

	void draw() 
	{
		float _gainPower = ofMap(gain, gain.getMin(), gain.getMax(), 0, AMP_GAIN_MAX_POWER, true);

		if (W_bCircled)
		{
			//// color
			//int _a = ofMap(W_Alpha, 0, 1, 100, 255);
			//ofColor _c = ofColor(cPlot, _a);
			//ofSetColor(_c);
			//float a = hb * _gainPower;//amp gain
			//for (int i = 0; i < SIZE_BUFFER - 2; ++i)//?
			//{
			//	float y1 = * a;
			//	ofDrawLine(x1, y1, x1, y2);
			//}

			float wb = r.getWidth();
			float hb = r.getHeight();

			ofFill();
			ofPushMatrix();
			ofTranslate(wb / 2, 0);

			// Amount of desired elements 
			// (lines or rectangles)
			int amount = (int)ofMap(W_Spread2, 0, 1, 365, 8, true);

			//TODO: remake
			int iStep = SIZE_BUFFER / amount;//it step
			float xStep = wb / (float)amount;//width of each

			float hb_Half = hb / 2;

			int ii = -1;

			float rd = W_CircledRadius * MIN(wb / 2, hb / 2);
			float sz = ofMap(W_CircledSize, 0, 1, 100, rd, true);
			float _amplify = sz * _gainPower;//amp gain

			for (int i = 0; i < SIZE_BUFFER / 2; i++)
			{
				//TODO:
				if (i % iStep == 0)
				{
					ii++;
				}
				else continue;//skip it from here to end (SIZE_BUFFER)!

				// Color Plot with alpha
				int _a = ofMap(W_Alpha, 0, 1, 0, cPlot.get().a);
				ofColor _c = ofColor(cPlot, _a);
				ofSetColor(_c);

				float val;
				val = plotIn[i] * _amplify;
				/*
				if (plotIn != nullptr) val = plotIn[i] * _amplify;
				else val = 0.5f;
				*/

				if (W_bAbs) val = abs(val);

				float t = -HALF_PI + ofMap(i, 0, (SIZE_BUFFER / 2), 0, TWO_PI);
				float x = cos(t) * rd;
				float y = sin(t) * rd;
				float a = ofRadToDeg(atan2(y, x));

				ofPushMatrix();
				{
					ofTranslate(x, y);
					ofRotateZDeg(a);

					float scl = 1;
					glScalef(scl, scl, scl);

					//line
					//ofDrawLine(0, 0, val, 0);

					//bar
					ofFill();
					int gap = 1;
					float wr = ofMap(W_RatioWidth2, 0, 1.f, 1, rd / 4, true);//width
					float offset = 0;
					//float offset = wr / 2;

					ofDrawRectRounded({ offset, 0, val, wr }, W_Rounded2 * (wr / 2));
					//ofDrawRectangle({ wr / 2, 0, val, wr });
				}
				ofPopMatrix();
			}

			//ofSetColor(100);
			//font.drawString(sortTypeInfo, -(font.stringWidth(sortTypeInfo) / 2), 0);

			ofPopMatrix();
		}

	};

};
