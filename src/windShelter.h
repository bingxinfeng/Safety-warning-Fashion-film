#pragma once
#include "ofMain.h"
#include "ofxLiquidFun.h"
#include "ofxOpenCv.h"


class windShelter : public ofBaseApp{

public:
    void setup();
    void update();
    void draw(float _ctX, float _ctY, float _ratioX, float _ratioY, float _brW, float _brH);
    void drawOutsideBlob();
    void keyCsetup(int _blobSize, float _brH);
    void key(int key, int _blobSize, float _brH);

    
    ofxBox2d box2dB;
    vector <ofPtr<ofxBox2dCircle> >    shelter, bulletCir;
    vector <ofPtr<ofxBox2dRect> > boxes;              //    defalut box2d rects
   
    bool move, hide;
    float shelterRad, bulletRad;
    vector <int> hsbH;
    vector <int> hsbS;
    
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvGrayscaleImage blurredImg;
    ofxCvContourFinder contourFinder;
    
};
