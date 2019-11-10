#pragma once
#include "ofMain.h"
#include "ofxLiquidFun.h"
#include "ofxOpenCv.h"


class Force : public ofBaseApp{

public:
    void setup();
    void update();
    void draw(float _ctX, float _ctY, float _ratioX, float _ratioY, float _brW, float _brH);
    void drawOutsideBlob();
    void keyCsetup();
    void key(int key);

    
    //------------------box2D--------------------
    float                                   px, py;
    bool                                    bForce, hide;
    
    ofxBox2d                                box2d;           // the box2d world
    ofPolyline                              drawing;         // we draw with this first
    ofxBox2dEdge                            edgeLine;        // the box2d edge/line shape (min 2 points)
    vector    <shared_ptr<ofxBox2dCircle> > circles;         // default box2d circles
    vector    <shared_ptr<ofxBox2dCircle> > circles2;         // default box2d circles
    vector <int> hsbS;
    vector <int> hsbB;
    vector <int> hsbH;
    
    float blobX, blobY, blobX2, blobY2;
    
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvGrayscaleImage blurredImg;
    ofxCvContourFinder contourFinder;
    
};
