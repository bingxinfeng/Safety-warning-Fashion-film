#pragma once
#include "ofMain.h"
#include "ofxLiquidFun.h"

class pullGravity : public ofBaseApp{

public:
	void setup(float _w, float _h);
	void update();
	void draw(float _avgDirX, float _avgDirY);
    
    void drawGravity(ofPoint p, ofPoint gravity);
    
    ofxBox2d box2dA;
    
    vector<shared_ptr<ofxBox2dCircle>> circlesRed;
    vector<shared_ptr<ofxBox2dCircle>> circlesLightGreen;
    vector<shared_ptr<ofxBox2dCircle>> circlesDarkGreen;
    vector<shared_ptr<ofxBox2dRect>> sharedRects;
    ofxBox2dCircle ShelterCircle;
    
    ofRectangle boundsA;
    
    
    //////
    vector<float> lengthVar;
};
