#include "Force.h"

static int pts[] = {257,219,257,258,259,274,263,325,266,345,266,352,269,369,276,387,286,415,291,425,302,451,308,462,316,472,321,480,328,488,333,495,339,501,345,505,350,507,365,515,370,519,377,522,382,525,388,527,405,534,426,538,439,539,452,539,468,540,485,540,496,541,607,541,618,539,625,537,641,530,666,513,682,500,710,476,723,463,727,457,729,453,732,450,734,447,738,440,746,423,756,404,772,363,779,343,781,339,784,327,789,301,792,278,794,267,794,257,795,250,795,232,796,222,796,197,797,195,797,188,796,188};
static int nPts  = 61*2;
//--------------------------------------------------------------
void Force::setup(){
    ofSetVerticalSync(true);
    ofSetCircleResolution(50);
    ofSetLogLevel(OF_LOG_NOTICE);
    
    //initialize box2dB WORLD
    box2d.init();
    box2d.setGravity(0, 0);
    box2d.setFPS(60.0);
    box2d.registerGrabbing();
    box2d.createBounds();
    
    for(int i=0; i<50; i++){
        hsbS.push_back(ofRandom(180,255));
        hsbB.push_back(ofRandom(180,255));
        hsbH.push_back(ofRandom(0,10));
    }
    
}

//--------------------------------------------------------------
void Force::update(){
    
    box2d.update();
    if(bForce) {
        float strength = 80.0f;//original 8.0 , but with 600? framerate
        float damping  = 0.7f;//7.0 too much, just fly away
        float minDis   = 500;
        for(int i=0; i<circles.size(); i++) {
            circles[i].get()->addAttractionPoint(blobX, blobY, strength);
            circles[i].get()->setDamping(damping, damping);
        }
        for(int i=0; i<circles2.size(); i++) {
            circles2[i].get()->addAttractionPoint(blobX2, blobY2, strength);
            circles2[i].get()->setDamping(damping, damping);
        }
    }
    ofRemove(circles, ofxBox2dBaseShape::shouldRemoveOffScreen);
    ofRemove(circles2, ofxBox2dBaseShape::shouldRemoveOffScreen);
}

//--------------------------------------------------------------
void Force::draw(float _ctX, float _ctY, float _ratioX, float _ratioY, float _brW, float _brH){
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
        
    //////////
    float rectW = _brW * ratioX;
    if(rectW>270){ //need to modified the threshold with model
        bForce = true;
    }else bForce = false;
    
    ofSetColor(255);
    ofDrawBitmapString("rectSz "+ofToString(rectW), 20, 20);
    
    //center circle:
    //ofSetColor(0, 0, 255);
    //ofDrawCircle( centroid.x * ratioX, centroid.y * ratioY, 20 );
    //ofPopStyle();
    
    //////// need to modified the position with model
    blobX = _ctX * ratioX+160;
    blobY = _ctY * ratioY+80;
    blobX2 = _ctX * ratioX-160;
    blobY2 = _ctY * ratioY+80;
//    }//blob
}
//--------------------------------------------------------------
void Force::drawOutsideBlob(){

    float r = ofRandom(2, 10);        // a random radius 4px - 20px
    circles.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
    circles.back().get()->setPhysics(3.0, 0.53, 0.1);
    circles.back().get()->setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()-20, r);
    
    circles2.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
    circles2.back().get()->setPhysics(3.0, 0.53, 0.1);
    circles2.back().get()->setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()-20, r);
    
    //this is how to constrain the amount of object in an object vector!
    if(circles.size()>50){
        circles.pop_back();
    }
    if(circles2.size()>50){
        circles2.pop_back();
    }
    
    ofPushStyle();
    for(int i=0; i<circles.size(); i++) {
        ofFill();
        ofColor hsbC = ofColor::fromHsb(hsbH[i],hsbS[i],hsbB[i]);
        ofSetColor(hsbC);
        circles[i].get()->draw();
    }
    for(int i=0; i<circles2.size(); i++) {
        ofFill();
        ofColor hsbC = ofColor::fromHsb(hsbH[i],hsbS[i],hsbB[i]);
        ofSetColor(hsbC);
        circles2[i].get()->draw();
    }
    ofPopStyle();

}
//--------------------------------------------------------------
void Force::keyCsetup(){
    float r = ofRandom(4, 20);        // a random radius 4px - 20px
    circles.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
    circles.back().get()->setPhysics(3.0, 0.53, 0.1);
    circles.back().get()->setup(box2d.getWorld(), mouseX, mouseY, r);
    
    circles2.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
    circles2.back().get()->setPhysics(3.0, 0.53, 0.1);
    circles2.back().get()->setup(box2d.getWorld(), mouseX, mouseY, r);
}
//--------------------------------------------------------------
void Force::key(int key) {
    if(key == 'c') {
        keyCsetup();
    }
    
    if(key == 'f') {
        bForce = !bForce;
    }
    
    if(key == 'h') {
        hide = !hide;
        }
}
