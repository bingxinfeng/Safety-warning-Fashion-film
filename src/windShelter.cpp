#include "windShelter.h"

//--------------------------------------------------------------
void windShelter::setup(){
    
    ofSetCircleResolution(50);
    ofSetLogLevel(OF_LOG_NOTICE);
    
    //initialize box2dB WORLD
    box2dB.init();
    box2dB.setGravity(0, 0);
    box2dB.setFPS(60.0);
    box2dB.registerGrabbing();
    //box2dB.createBounds();
    
    //move statement to setup bullets and shelter
    move=false;
    hide = false;
    
    //bullet color vector setup
    for(int i=0; i<10000; i++) {
        hsbS.push_back(ofRandom(200,255));
        hsbH.push_back(ofRandom(180,200));
    }
    
    //myKinectSetup(255,122);
}

//--------------------------------------------------------------
void windShelter::update(){
    
    box2dB.update();
    ofRemove(bulletCir, ofxBox2dBaseShape::shouldRemoveOffScreen);
    
    //myKinectUpdate();
}

//--------------------------------------------------------------
void windShelter::draw(float _ctX, float _ctY, float _ratioX, float _ratioY, float _brW, float _brH){
    //ofSetColor(255);
    //kinect.draw(0,0,ofGetWidth(),ofGetHeight());
    
    float ratioX = _ratioX;
    float ratioY = _ratioY;
    
//    if(contourFinder.blobs.size() > 0) {
//        // store the (main/biggest blob's)centroid(ofVec3f)
//        ofVec3f centroid = contourFinder.blobs[0].centroid;
    
        if(hide == false){ //draw the bouding rect and centrio circle
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
        
        if(move==true){ //'c' to draw the shelter, then 'm' to attach it to the target centrio
            for(int i=0; i<shelter.size(); i++){
                shelterRad = _brH/1.3;
                shelter[i]->justForce(_ctX * ratioX, _ctY * ratioY - _brH/2);//force it to attach
            }
            
            //test sprout circle bullets +++++++++
            bulletRad = ofRandom(3,6);
            
            if(ofGetFrameNum()%4==0){ //make it shoot slower
                for(float y = 0; y < ofGetHeight(); y+=50){
                    bulletCir.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
                    bulletCir.back().get()->setPhysics(0.1, 0.5, 0.0);//heavy weight//light weight (0.9, 0.9, 0.1) density, bounce, friction
                    bulletCir.back().get()->setup(box2dB.getWorld(), 10, y+ ofRandom(20), bulletRad);
                    bulletCir.back().get()->setVelocity(ofRandom(10,50),0);
                }
            }
            
        } //move
//    }//blob
}
//--------------------------------------------------------------
void windShelter::drawOutsideBlob(){

    if(hide == false){
        for(int i=0; i<shelter.size(); i++) {
            ofPushStyle();
            ofSetHexColor(0xf6c738);
            shelter[i].get()->draw();
            ofPopStyle();
        }
    }
    
    for(int i=0; i<bulletCir.size(); i++) {
        ofPushStyle();
        ofColor bulletColor = ofColor::fromHsb(hsbH[i],hsbS[i],200);
        ofSetColor(bulletColor);
        ofFill();
        bulletCir[i].get()->draw();
        ofPopStyle();
    }
}
//--------------------------------------------------------------
void windShelter::keyCsetup(int _blobSize, float _brH){
    if(_blobSize > 0) {
        shelterRad = _brH/1.3;
        shelter.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
        shelter.back().get()->setPhysics(0.2, 0.3, 0.0);//heavy weight //light weight (0.9, 0.9, 0.1)
        shelter.back().get()->setup(box2dB.getWorld(), mouseX, mouseY, shelterRad);
    }
}
//--------------------------------------------------------------
void windShelter::key(int key, int _blobSize, float _brH) {
    if(key == 'c') {
        keyCsetup(_blobSize, _brH);
    }
    
    if(key == 'm') {
        move=true;
    }
    
    if(key == 'h') {
        hide = !hide;
        }
}
