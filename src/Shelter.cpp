#include "Shelter.h"

//--------------------------------------------------------------
void Shelter::setup(){
    
    //base initialize
    //ofSetFrameRate(60);
    ofSetCircleResolution(50);
    ofSetLogLevel(OF_LOG_NOTICE);
    
    //initialize box2d WORLD
    box2dE.init();
    box2dE.setGravity(0, 10);
    box2dE.setFPS(30.0);
    box2dE.registerGrabbing();
    box2dE.createBounds();
    
    //move statement to setup bullets and shelter
    move=false;
    hide = false;
    
    //bullet color vector setup
    for(int i=0; i<1000; i++) {
        hsbS.push_back(ofRandom(200,255));
        hsbH.push_back(ofRandom(0,25));
    }
}

//--------------------------------------------------------------
void Shelter::update(){
    
    box2dE.update();

}

//--------------------------------------------------------------
void Shelter::draw(float _ctX, float _ctY, float _ratioX, float _ratioY, float _brW, float _brH){
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
    
        box2dE.setGravity(0, 1);
    if(move==true){ //'c' to draw the shelter, then 'm' to attach it to the target centrio
        for(int i=0; i<shelter.size(); i++){
            shelterRad = _brH/2;
            shelter[i]->justForce(_ctX * ratioX, _ctY * ratioY - 50);//force it to attach
        }
        
        //test sprout circle bullets +++++++++
        bulletRad = ofRandom(6,20);
        if(ofGetFrameNum()%8==0){ //make it shoot slower
            bulletCir.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
            bulletCir.back().get()->setPhysics(4.0, 0.23, 0.1);//density, bounce, friction
            bulletCir.back().get()->setup(box2dE.getWorld(), 10, 10, bulletRad);
            bulletCir.back().get()->setVelocity(10,5);
            
            bulletCir2.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
            bulletCir2.back().get()->setPhysics(4.0, 0.23, 0.1);//density, bounce, friction
            bulletCir2.back().get()->setup(box2dE.getWorld(), ofGetWidth()-10, 10, bulletRad);
            bulletCir2.back().get()->setVelocity(-10,5);
        }
    }
//    }//blob
}
//--------------------------------------------------------------
void Shelter::drawOutsideBlob(){

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
        ofColor bulletColor = ofColor::fromHsb(hsbH[i],hsbS[i],150);
        ofSetColor(bulletColor);
        ofFill();
        bulletCir[i].get()->draw();
        ofPopStyle();
    }
    for(int i=0; i<bulletCir2.size(); i++) {
        ofPushStyle();
        ofColor bulletColor = ofColor::fromHsb(hsbH[i],hsbS[i],150);
        ofSetColor(bulletColor);
        ofFill();
        bulletCir2[i].get()->draw();
        ofPopStyle();
    }
}
//--------------------------------------------------------------
void Shelter::keyCsetup(int _blobSize, float _brH){
    if(_blobSize > 0) {
        shelterRad = _brH+20;
        shelter.push_back(ofPtr<ofxBox2dCircle>(new ofxBox2dCircle));
        shelter.back().get()->setPhysics(4.0, 0.53, 0.1);//heavy weight //light weight (0.9, 0.9, 0.1)
        shelter.back().get()->setup(box2dE.getWorld(), mouseX, mouseY, shelterRad);
    }
}
//--------------------------------------------------------------
void Shelter::key(int key, int _blobSize, float _brH) {
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
//--------------------------------------------------------------
void Shelter::drawOpf(float _avgDirX, float _avgDirY){
    phase += ofClamp(_avgDirX, -4, 4);
    int numOfDoors = 8;
    
    for (int i=0; i<numOfDoors; i++){
        sphere(2*phase + i * 180/numOfDoors, _avgDirY, _avgDirX);
    }
}
//--------------------------------------------------------------
void Shelter::sphere(float p, float y, float x){
    ofPushMatrix();
    ofPushStyle();
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofTranslate(-ofGetWidth()/2, 0);
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofRotateDeg(90);
    ofRotateX(90);//or90
    //ofRotateZ(45);
    ofRotateY(p);
    float s = abs(sin(ofDegToRad(p))) + 0.3;
    //ofScale(s,s,s);
    ofNoFill();
    ofSetColor(50+p/2,100+p/2,10+p/2);
    ofSetLineWidth(10);
    //ofDrawRectangle(0, 0, 200,200);
    ofDrawCircle(-ofGetWidth()/6, 0, 200);
    ofPopMatrix();
    ofPopStyle();
    
    ofPushMatrix();
    ofPushStyle();
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofTranslate(ofGetWidth()/2, 0);
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofRotateDeg(90);
    ofRotateX(90);//or90
    //ofRotateZ(45);
    ofRotateY(-p);
    //ofScale(s,s,s);
    ofNoFill();
    ofSetColor(50+p/2,100+p/2,10+p/2);
    ofSetLineWidth(10);
    //ofDrawRectangle(0, 0, 200,200);
    ofDrawCircle(-ofGetWidth()/6, 0, 200);
    
    ofPopMatrix();
    ofPopStyle();
}
