#include "Swim.h"

//--------------------------------------------------------------
void Swim::setup(){
    ofSetFrameRate(60);
    ofSetCircleResolution(50);
    ofSetLogLevel(OF_LOG_NOTICE);
    
    //initialize box2dC WORLD
    box2dC.init();
    box2dC.setGravity(0, 10);
    box2dC.setFPS(30.0);
    
    box2dC.registerGrabbing();
    box2dC.createBounds();
    
    ofColor color = ofColor(62,173,227);
    particles.setup(box2dC.getWorld(), 8000, 3000, 8.5, 7, color);
    
    for (int i = 0; i < 8000; i++) {
        ofVec2f position = ofVec2f(ofRandom(ofGetWidth()),ofRandom(ofGetHeight()));
        ofVec2f velocity = ofVec2f(0, 0);
        particles.createParticle(position, velocity);
    }
    hide2 = false;
    move2=false;
}

//--------------------------------------------------------------
void Swim::update(){
    
    box2dC.update();
    
}

//--------------------------------------------------------------
void Swim::draw(float _ctX, float _ctY, float _ratioX, float _ratioY, float _brW, float _brH){
    //ofSetColor(255);
    //kinect.draw(0,0,ofGetWidth(),ofGetHeight());
    
    float ratioX = _ratioX;
    float ratioY = _ratioY;
    
//    if(contourFinder.blobs.size() > 0) {
//        // store the (main/biggest blob's)centroid(ofVec3f)
//        ofVec3f centroid = contourFinder.blobs[0].centroid;
    
    if(hide2==false){
        ofPushStyle();
        //bounding rect:
        ofSetColor(255, 0, 0);
        ofNoFill();
        ofSetRectMode(OF_RECTMODE_CENTER);
        ofDrawRectangle( _ctX * ratioX, _ctY * ratioY, _brW* ratioX, _brH* ratioY);
        //center circle:
        ofSetColor(0, 0, 255);
        ofDrawCircle( _ctX * ratioX, _ctY * ratioY, 20 );
        ofPopStyle();
    }
    
    if(move2==true){
        //r = contourFinder.blobs[0].boundingRect.height* ratioY/2;
        for(int i=0; i<circles.size(); i++){
            r = _brH;
            circles[i]->justForce(_ctX * ratioX, _ctY * ratioY-_brH* ratioY/9);//YES!!
        }
    }
//    }//blob
}
//--------------------------------------------------------------
void Swim::drawOutsideBlob(){

    if(hide2 == false){
        //invisible the shelter
        for(int i=0; i<circles.size(); i++) {
            ofFill();
            ofSetHexColor(0xf6c738);
            circles[i].get()->draw();
        }
    }
    
    particles.draw();
}
//--------------------------------------------------------------
void Swim::keyCsetup(int _blobSize, float _brH){
    if(_blobSize > 0) {
        r = _brH;
        circles.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
        circles.back().get()->setPhysics(4.0, 0.53, 0.1);//heavy weight
        circles.back().get()->setup(box2dC.getWorld(), mouseX, mouseY, r);
    }
}
//--------------------------------------------------------------
void Swim::key(int key, int _blobSize, float _brH) {
    if(key == 'c') {
        keyCsetup(_blobSize, _brH);
    }
    
    if(key == 'm') {
        move2=true;
    }
    
    if(key == 'h') {
        hide2 = !hide2;
        }
}
