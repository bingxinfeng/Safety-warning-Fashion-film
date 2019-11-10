#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxSyphon.h"

#include "pullGravity.h"
#include "RainbowGravity.h"
#include "windShelter.h"
#include "Swim.h"
#include "Force.h"
#include "Shelter.h"

class ofApp : public ofBaseApp {
public:
    
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    //Kinect ++++++++++++++++++++++++++++++++++++++
    ofxKinect kinect;
    void myKinectSetup(int _near, int _far);
    void myKinectUpdate();
    void myKinectDrawInstruct();
    void myKinectKeys(int key);
    void exit();
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvGrayscaleImage blurredImg;
    ofxCvContourFinder contourFinder;
    
    bool bThreshWithOpenCV;
    bool debug, kScan, hide1, kinectOn;
    
    int nearThreshold;
    int farThreshold;
    int angle;
    // used for viewing the point cloud
    ofEasyCam easyCam;
    // for scan
    ofImage img4scan;
    ofColor averageColor, scanImgColor;
    
    void sphere(float p, float z, float rtX, float rad);
    float phase;
    
    ofVideoPlayer mri;
    ofImage instruction;
    bool info;
    
    //Syphon ++++++++++++++++++++++++++++++++++++++
    void syphonSetup();
    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverUpdated(ofxSyphonServerDirectoryEventArgs &args);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
    
    ofxSyphonServerDirectory dir;
    ofxSyphonClient client;
    int dirIdx;
    bool syphonData, camScan;
    
    //convert from syphon image to ofImage
    ofImage camImg;
    ofPixels camPix, camPix2;
    ofFbo fbo, fbo2;
    float camH, camW;
    //imgBuffer
    deque<ofImage> imgBuffer;
    int maxBufferSize;
    ofImage imgSc;
    
    //Frame diff ++++++++++++++++++++++++++++++++++++++
    void frameDiffSetup();
    void frameDiffUpdate(float _diffFloat, float _buffFloat);
    void drawFrameDiff4();
    ofxCvColorImage colorImage;//store pix CVcolorImage
    //The current and the previous video frames as grayscale images
    ofxCvGrayscaleImage grayImageCurr, grayImagePrev;
    ofxCvGrayscaleImage diff;        //Absolute difference of the frames
    ofxCvFloatImage diffFloat;        //Amplified difference images
    ofxCvFloatImage bufferFloat;    //Buffer image
    bool frameDiff;
    ofImage textile1, textile2, textile3, textile4, textile5, textile6;
    int textile;
    
    //optical flow ++++++++++++++++++++++++++++++++++++++
    void opfSetup();
    void opfUpdate();
    void opfDraw();
    bool calculatedFlow, opf;
    ofxCvColorImage currentColor;        //First and second original images
    ofxCvGrayscaleImage gray1, gray2;    //Decimated grayscaled images
    ofxCvFloatImage flowX, flowY;        //Resulted optical flow in x and y axes
    float sumX, sumY, avgX, avgY;
    int numOfEntries;
    
    //box2D +++++++++++++++++++++++++++++++++++++++++++++
    int physicalMode;
    pullGravity pullgravity;
    RainbowGravity rainbowGravity;
    windShelter windshelter;
    bool wind;
    Swim swim;
    bool swimming;
    Force force;
    bool forcing;
    Shelter shelter;
    bool sheltering;
    bool hideinfo;

};
