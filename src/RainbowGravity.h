#include "ofMain.h"
#include "ofxLiquidFun.h"

class RainbowGravity : public ofBaseApp{

public:
	void setup();
	void update();
	void draw(float _avgDirX, float _avgDirY);
    
    
    ofxBox2d box2dD;
    
    vector<shared_ptr<ofxBox2dCircle>> circlesA;
    vector<shared_ptr<ofxBox2dRect>> sharedRects;
    
    ofRectangle boundsD;
    
    //////
    vector<float> red,green,blue,H;
};
