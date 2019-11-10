#include "pullGravity.h"

//--------------------------------------------------------------
void pullGravity::setup(float _w, float _h)
{
    
    ofSetCircleResolution(50);
    
    // the world bounds
    boundsA.set(0, 0, _w, _h);
//    boundsB.set(0, 0, _w, _h);
    
    // setup world A
    box2dA.init();
    box2dA.setFPS(60);
    box2dA.setGravity(0, ofRandom(-10,10));
    box2dA.createBounds(boundsA);
    box2dA.registerGrabbing();
    
    //lengthVar vector
    for(int i=0; i<ofGetWidth(); i++) {
        lengthVar.push_back(ofRandom(50));
    }
    
    // add some red cirlces
    for(int i=0; i<150; i++) {
        auto c = std::make_shared<ofxBox2dCircle>();
        c.get()->setPhysics(1, 0.5, 1);
        c.get()->setup(box2dA.getWorld(), 250+ofRandom(-50, 50), 50, ofRandom(3,20));//The last two is circles amount and radius
        circlesRed.push_back(c);
    }
    
    // add some light green cirlces
    for(int i=0; i<100; i++) {
        auto c = std::make_shared<ofxBox2dCircle>();
        c.get()->setPhysics(1, 0.5, 1);
        c.get()->setup(box2dA.getWorld(), 250+ofRandom(-50, 50), 100, ofRandom(3,20));
        circlesLightGreen.push_back(c);
    }
    
    // add some dark green cirlces
    for(int i=0; i<100; i++) {
        auto c = std::make_shared<ofxBox2dCircle>();
        c.get()->setPhysics(1, 0.5, 1);
        c.get()->setup(box2dA.getWorld(), 250+ofRandom(-50, 50), 200, ofRandom(3,20));
        circlesDarkGreen.push_back(c);
    }
    
    //Shelter circle
    for(int i =0; i<2; i++){
        ShelterCircle.setup(box2dA.getWorld(), ofGetWidth()/4, ofGetHeight()/2-i*100, ofGetHeight()/5);
    }
    
    //boxes
    for(int i=0; i<40; i++) {
        auto r = std::make_shared<ofxBox2dRect>();
        r.get()->setPhysics(1, 0.7, 0.9);
        r.get()->setup(box2dA.getWorld(), 250+ofRandom(-50, 50), 10, 10, 10);//the last two is rect w and h
        sharedRects.push_back(r);
    }
    
    
}

//--------------------------------------------------------------
void pullGravity::update(){
	
    box2dA.update();
}
//--------------------------------------------------------------
void pullGravity::drawGravity(ofPoint p, ofPoint gravity) {
    
    float angle = (atan2(gravity.y, gravity.x) * 180 / PI) - 90;
    float len   = MIN(200, gravity.length()*10); // scale it up a bit
    
    ofPushMatrix();
    ofTranslate(p.x, p.y);
    ofRotate(angle);
    ofDrawLine(0, 0, 0, len);
    ofDrawTriangle(0, len,-5, len-10,5, len-10);
    ofPopMatrix();
}

//--------------------------------------------------------------
void pullGravity::draw(float _avgDirX, float _avgDirY){
    
    //吊线
    for(int x=0; x<ofGetWidth()/2; x+=20){
        ofSetColor(200,225,0);
        ofNoFill();
        ofSetLineWidth(2);
        float length = (sin(x/20+ofGetFrameNum()*0.05)*30+100)+ _avgDirY/1.3*(-lengthVar[x]);
        ofDrawLine(x, 0, x, length );
    }
    
    ////////////////////MAP avgDir to Gravity
    float maxGravity = 100;
    
    if(boundsA.inside(ofGetWidth()/4, ofGetHeight()/2)) {
        float gx = ofMap(_avgDirX, -4, 4, -maxGravity, maxGravity);
        float gy = ofMap(_avgDirY, 0, -4, maxGravity, -maxGravity*15);//+++ maybe not doing the -time thing, just let it falls everytime
        float gyB = ofMap(_avgDirY, 0, -4, -maxGravity, maxGravity);
        box2dA.setGravity(gx, gy);
        //box2dB.setGravity(gx, gyB);
    }
    ////////////////////Draw Gravity objects
    // world A
    boundsA.inside(ofGetWidth()/4, ofGetHeight()/2) ? ofSetColor(80) :
    
    ofFill();
    
    // A World Circles
    for (int i=0; i<circlesRed.size(); i++) {
        ofSetColor(155,25,25);
        ofFill();
        circlesRed[i].get()->draw();
    }
    
    for (int i=0; i<circlesLightGreen.size(); i++) {
        ofSetColor(0,255,0);
        ofFill();
        circlesLightGreen[i].get()->draw();
    }
    
    for (int i=0; i<circlesDarkGreen.size(); i++) {
        ofSetColor(34,76,7);
        ofFill();
        circlesDarkGreen[i].get()->draw();
    }
    //ofSetColor(34,76,7);
    //ShelterCircle.draw();

    // Shared Rects
    for (int i=0; i<sharedRects.size(); i++) {
        ofSetColor(255,255,0);
        ofFill();
        sharedRects[i].get()->draw();
    }
    
}
