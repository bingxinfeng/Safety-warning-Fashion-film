#include "RainbowGravity.h"

//--------------------------------------------------------------
void RainbowGravity::setup(){
    
    ofSetCircleResolution(50);
    
    ///////////
    // the world bounds
    boundsD.set(0, 0, ofGetWidth(), ofGetHeight());
    
    // setup world A
    box2dD.init();
    box2dD.setFPS(60);
    box2dD.setGravity(0, -10);
    box2dD.createBounds(boundsD);
    box2dD.registerGrabbing();
    
    //////add some color to A
    for(int i=0; i<500; i++) {
//        red.push_back(ofRandom(255));
//        green.push_back(ofRandom(255));
//        blue.push_back(ofRandom(255));
        H.push_back(ofRandom(255));
    }
    
    // add some cirlces to world A
    for(int i=0; i<500; i++) {
        auto c = std::make_shared<ofxBox2dCircle>();
        c.get()->setPhysics(1, 0.5, 1);
        c.get()->setup(box2dD.getWorld(), 250+ofRandom(-50, 50), 10, ofRandom(3,10));//The last two is circles amount and radius
        circlesA.push_back(c);
    }
    
    // we can also have a vector of any shape
    for(int i=0; i<40; i++) {
        
        auto r = std::make_shared<ofxBox2dRect>();
        r.get()->setPhysics(1, 0.7, 0.9);
        
        r.get()->setup(box2dD.getWorld(), 250+ofRandom(-50, 50), 10, ofRandom(60,200), ofRandom(3,10));//the last two is rect w and h
        
        // add to one vector
        sharedRects.push_back(r);
    }
}

//--------------------------------------------------------------
void RainbowGravity::update(){
    box2dD.update();
}

//--------------------------------------------------------------
void RainbowGravity::draw(float _avgDirX, float _avgDirY){
    
    ////////////////////MAP avgDir to Gravity
    float maxGravity = 300;
    
    //if(boundsA.inside(ofGetMouseX(), ofGetMouseY())) {
    if(boundsD.inside(ofGetWidth()/2, ofGetHeight()/2)) {
        float gx = ofMap(_avgDirX, -4, 4, -maxGravity, maxGravity);
        float gy = ofMap(_avgDirY, 4, -4, -maxGravity, maxGravity);//+++ maybe not doing the -time thing, just let it falls everytime
        box2dD.setGravity(gx, gy);
    }
    
    // A World Circles
    for (int i=0; i<circlesA.size(); i++) {
        //ofSetHexColor(0xBFE364);
        //ofSetColor(red[i],green[i],blue[i]);
        ofColor hsbC = ofColor::fromHsb(H[i],200,255);
        ofSetColor(hsbC);
        circlesA[i].get()->draw();
    }
    
    // Shared Rects
    for (int i=0; i<sharedRects.size(); i++) {
        //ofSetHexColor(0x2F9BA1);
        ofColor hsbC2 = ofColor::fromHsb(H[i],200,255);
        ofSetColor(hsbC2);
        ofFill();
        sharedRects[i].get()->draw();
    }

}
