#pragma once
#include "ofMain.h"
#include "ofxLiquidFun.h"
#include "ofxOpenCv.h"


class Swim : public ofBaseApp{

public:
    void setup();
    void update();
    void draw(float _ctX, float _ctY, float _ratioX, float _ratioY, float _brW, float _brH);
    void drawOutsideBlob();
    void keyCsetup(int _blobSize, float _brH);
    void key(int key, int _blobSize, float _brH);

    ofxBox2d box2dC;
    vector <ofPtr<ofxBox2dCircle> >    circles;          //    default box2d circles
    ofxBox2dParticleSystem particles;                  //    LiquidFun particle system
    
    bool move2, hide2;
    float r;
    
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvGrayscaleImage blurredImg;
    ofxCvContourFinder contourFinder;
    
};
