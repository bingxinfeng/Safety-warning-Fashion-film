#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    // <-------- setup the GL context
	ofSetupOpenGL(640*2,480*2,OF_WINDOW);   //for syphon (cant run too big)
    //ofSetupOpenGL(640*1.2,480*1.2,OF_FULLSCREEN); //for kinect

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
