#include "ofApp.h"
#include "opencv2/opencv.hpp"
//Bingxin Feng - Creative Coding 2 Final Project
//This is a program designed for experimenting how computer visions interaction could be used, and affect in a fashion presentation. I collaborated with my textile designer friend, we used the interactions to present 6 garments, in the form of film (potentially to be developped into performance).
//So in order to convey each garment's feature, there are kinect depth image, frame differencing, optical flow, etc. different approaches to be interacted with the model in this project. So I have to admit that the code is a bit messy here. Sorry about that but I've tried my best to manage it...
//Also, I used blackMagic Design Ultra Studio Mini Recorder (adapter), Black Syphon (software) and ofxSyphon to run live feed from a camcorder. (That's one of my main goal in this project, making a progress into filmmaking and live performance.) So if you couldn't get the full set of equipment, it won't be able to run the camcorder modes codes. But You can see how it works from my documentation and demo videos. Will updated in the blog post later. Or you can watch our livestream history here: https://youtu.be/h1nZsLMQ4vg

/*Resource Credits:
 1. ofxLiquidFun addon from Github-tado https://github.com/tado/ofxLiquidFun
 2. Solution of using fbo to convert syphon to image: https://forum.openframeworks.cc/t/ofxsyphon-to-image/7791/4
 3. Solution of live camcorder in openFrameworks:  https://forum.openframeworks.cc/t/what-is-the-best-device-to-capture-an-hdmi-video-stream/24333
 4. ofxSyphon addon fron Github-Astellato https://github.com/astellato/ofxSyphon
 5. Solution of locating CV blob: https://forum.openframeworks.cc/t/finding-center-of-opencv-blobs/11836
 */

//--------------------------------------------------------------
void ofApp::setup() {
    ofBackground(0);
    ofSetFrameRate(60);
    myKinectSetup(255,162);//near,far
   
    info = true;
    debug = false;
    
    kScan = false;
    hide1 = false;
    camScan = false;
    //frameDiff
    frameDiffSetup();
    frameDiff = false;
    //opticalFlow
    calculatedFlow = false;
    opf=false;
    wind = false;
    swimming = false;
    forcing = false;
    sheltering = false;
    hideinfo = true;
    
    img4scan.allocate(ofGetWidth(), ofGetHeight(),OF_IMAGE_COLOR);
    phase = 0;
    mri.load("mriFruit2.mp4");
    instruction.load("instruction.jpg");
    
    //syphon setup (FOR THE CAMCORDER LIVE FEED)+++++++++++++++++++++++
    dir.setup();//setup our directory
    client.setup();//setup our client
    //register for our directory's callbacks
    ofAddListener(dir.events.serverAnnounced, this, &ofApp::serverAnnounced);
    // not yet implemented
    //ofAddListener(dir.events.serverUpdated, this, &ofApp::serverUpdated);
    ofAddListener(dir.events.serverRetired, this, &ofApp::serverRetired);
    dirIdx = -1;
    syphonData = false;
    
    //Don't forget to allocate the camPixels, fbo, and CVimg:
    camW = 1920/2;
    camH = 1080/2;
    camPix.allocate(camW,camH,1);
    fbo.allocate(camW,camH,GL_RGB);//GL_RBG fix the unsynchronised issue!!!

    //For scanlit: Set the maxBufferSize to the width of the entire window.
    maxBufferSize = camW;
    
    
    //++++++++++++++++++++++++++++++++++++++++++++++++
    //classes of the box2d world interactions
    pullgravity.setup(ofGetWidth()/2, ofGetHeight());
    rainbowGravity.setup();
    windshelter.setup();
    swim.setup();
    force.setup();
    shelter.setup();
}
//--------------------------------------------------------------
void ofApp::update() {
    frameDiffUpdate(7.0, 0.85);
    
    myKinectUpdate();
    mri.update();
    
    
    if(opf==true) {
        opfUpdate();
        pullgravity.update();
        rainbowGravity.update();
    }
    
    if(wind) windshelter.update();
    if(swimming) swim.update();
    if(forcing) force.update();
    if(sheltering) shelter.update();
    
    if (syphonData==true){
        //camcorder syphon updates
        camImg.setFromPixels(camPix);//ofImage
        imgSc.setFromPixels(camPix);
        //remove from back if buffer has reached maximum size
        imgBuffer.push_front(imgSc);
        //remove from back if buffer has reached maximum size
        if (imgBuffer.size()>maxBufferSize) imgBuffer.pop_back();
    }

}
//--------------------------------------------------------------
void ofApp::draw() {
    
    if(info){
        ofSetColor(255);
        instruction.draw(0,0,ofGetWidth(), ofGetHeight());
    }
    
    //Kinect modes ++++++++++++++++++++++++++++++++++++++++++++++
        if(debug){
            ofClear(20);
            //draw the videos
            ofFill();
            ofSetColor(255, 255, 255);
            // draw from the live kinect
            kinect.drawDepth(10, 10, 400, 300);//top left image
            kinect.draw(420, 10, 400, 300);//top right image
            blurredImg.draw(10, 320, 400, 300); //down left image
            contourFinder.draw(0,0, ofGetWidth(), ofGetHeight());// CONTOUR HERE!
            
            myKinectDrawInstruct();
        
            //Calculate the ratio between the window and the kinext display range
            float ratioX = ofGetWidth()  / grayImage.getWidth();
            float ratioY = ofGetHeight()  / grayImage.getHeight();
            
            //Kinect modes 1 Figure Scanning++++++++++++++++++++++++++
            if(kScan){
                //directly draw kinect video (store in img4scan)
                img4scan.setFromPixels(kinect.getPixels());
                ofSetColor(255);
                img4scan.draw(0,0, ofGetHeight()/480*640, ofGetHeight());
                
                //scan location
                float lineY = abs(sin(ofGetFrameNum()*0.005))*img4scan.getHeight();
                float lineX = img4scan.getWidth()/3 + 70;
                ofNoFill();
                ofSetLineWidth(2);
                ofSetColor(255,0,0);
                ofDrawLine(0,lineY*ratioY,ofGetWidth()/3,lineY*ratioY); //the display scan range
                
                //get color data
                float rSum = 0;
                float gSum = 0;
                float bSum = 0;
                for(int x=0; x<lineX; x++){
                    scanImgColor = img4scan.getPixels().getColor(x,lineY);
                    rSum += scanImgColor.r;
                    gSum += scanImgColor.g;
                    bSum += scanImgColor.b;
                }
                int samples = img4scan.getWidth(); // The number of pixels we are averaging
                
                // Update the average color
                averageColor.r = rSum / samples;
                averageColor.b = bSum / samples;
                averageColor.g = gSum / samples;
                
                //visulizing graphic:::::::::::::::::::::::::::::::::
                //blackdrop
                ofPushStyle();
                ofSetColor(0);
                ofFill();
                ofDrawRectangle(ofGetWidth()/3, 0, ofGetWidth(), ofGetHeight());
                ofPopStyle();
                
                //MRI video playback
                ofEnableAlphaBlending();
                mri.play();
                ofPushStyle();
                float mriR = ofMap(averageColor.r, 0, 100, 50,255, true);
                float mriG = ofMap(averageColor.g, 0, 100, 50,255, true);
                float mriB = ofMap(averageColor.b, 0, 100, 50,255, true);
                
                float min = 50;
                float max = 60;
                
                if((averageColor.r-averageColor.g)>40 && (averageColor.r-averageColor.b)>40){
                    mriG = ofClamp(mriG, min, max);
                    mriB = ofClamp(mriB, min, max);
                }
                if((averageColor.g-averageColor.r)>40 && (averageColor.g-averageColor.b)>40){
                    mriR = ofClamp(mriR, min, max);
                    mriB = ofClamp(mriB, min, max);
                }
                if((averageColor.b-averageColor.r)>40 && (averageColor.b-averageColor.g)>40){
                    mriR = ofClamp(mriR, min, max);
                    mriG = ofClamp(mriG, min, max);
                }
                ofSetColor(mriR,mriG,mriB);
                mri.draw(ofGetWidth()/3, 0, ofGetWidth()/1.5, ofGetHeight());
                ofPopStyle();
                
                //sphere on top
                ofPushStyle();
                ofPushMatrix();
                ofTranslate(ofGetWidth()/8, 0);
                ofFill();
                ofSetColor(255, 0, 0,100);
                int numOfDoors = 4;
                float gap = 180/numOfDoors/3*1.0f;
                float zR = ofMap(averageColor.r, 0, 100, 70,140, true);
                float rR = zR;
                float rtXR = ofMap(averageColor.r, 0, 100, 1,10, true);
                for (int i=0; i<numOfDoors; i++){
                    sphere(phase + i * 180/numOfDoors * ofGetFrameNum()*0.01, zR, rtXR, rR);
                }
                ofNoFill();
                for (int i=0; i<numOfDoors; i++){
                    sphere(phase + i * 180/numOfDoors * ofGetFrameNum()*0.01, zR, rtXR, rR*1.3);
                }
                
                ofFill();
                ofSetColor(0, 255, 0,100);
                float zG = ofMap(averageColor.g, 0, 100, 70,140, true);
                float rG = zG;
                float rtXG = ofMap(averageColor.r, 0, 100, 1,10, true);
                for (int i=0; i<numOfDoors; i++){
                    sphere(phase + i * 180/numOfDoors * ofGetFrameNum()*0.01+90, zG, rtXG, rG);
                }
                ofNoFill();
                for (int i=0; i<numOfDoors; i++){
                    sphere(phase + i * 180/numOfDoors * ofGetFrameNum()*0.01+90, zG, rtXG, rG*1.5);
                }
                
                ofFill();
                ofSetColor(0, 50, 150, 100);//color 1
                float zB = ofMap(averageColor.b, 0, 100, 70,140, true);
                float rB = zB;
                float rtXB = ofMap(averageColor.r, 0, 100, 1,-10, true);
                for (int i=0; i<numOfDoors; i++){
                    sphere(phase + i * 180/numOfDoors * ofGetFrameNum()*0.01 + 60, zB, rtXB, rB);
                }
                ofPopMatrix();
                ofPopStyle();
            }//'0' for scan bool
            
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //Here is the optical Flow interacitons
            if(opf==true && sheltering ==false) opfDraw(); //And it's half-half kinect&camcorder feed
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //Here are the kinect blob tracking interactions
            if (wind || swimming || forcing ) {
                ofSetColor(255);
                kinect.draw(0,0,ofGetWidth(),ofGetHeight());
            }
            
            if(sheltering && opf){ //this one used both blob tracking and opticalFlow, so it's a bit tricky when execute it
                opfDraw();
            }

            if(contourFinder.blobs.size() > 0) {
                // store the (main/biggest blob's)centroid(ofVec3f)
                ofVec3f centroid = contourFinder.blobs[0].centroid;
                if (wind){//draw the wind one
                    windshelter.draw(centroid.x, centroid.y, ratioX, ratioY, contourFinder.blobs[0].boundingRect.width, contourFinder.blobs[0].boundingRect.height);
                }
                if (swimming){//draw the wind one
                    swim.draw(centroid.x, centroid.y, ratioX, ratioY, contourFinder.blobs[0].boundingRect.width, contourFinder.blobs[0].boundingRect.height);
                }
                if (forcing){//draw the wind one
                    force.draw(centroid.x, centroid.y, ratioX, ratioY, contourFinder.blobs[0].boundingRect.width, contourFinder.blobs[0].boundingRect.height);
                }
                if (sheltering){//draw the wind one
                    shelter.draw(centroid.x, centroid.y, ratioX, ratioY, contourFinder.blobs[0].boundingRect.width, contourFinder.blobs[0].boundingRect.height);
                }
            }
            if (wind) windshelter.drawOutsideBlob();
            if (swimming) swim.drawOutsideBlob();
            if (forcing) force.drawOutsideBlob();
            if (sheltering) shelter.drawOutsideBlob();
            
            if(wind || swimming || forcing || sheltering){
                if(hideinfo){
                    ofSetColor(0, 150);
                    ofDrawRectangle(0, 0, ofGetWidth()/2, 90);
                    ofSetColor(255);
                    ofDrawBitmapString("press 'c' to draw the shelter circle", 20, 40);
                    ofDrawBitmapString("then press 'm' to locate the shelter circle on the main blob", 20, 55);
                    ofDrawBitmapString("then press 'h' to hide/show the shelter circle and bounding rects", 20, 70);
                    ofDrawBitmapString("press 'H' to hide/show this info", 20, 85);
                }
            }
            
        }//debug
    
    
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (syphonData==true){
        //notice you the camcorder mode is on
        ofPushStyle();
        ofSetColor(255, 0, 0, 150);
        ofDrawRectangle(0, 0, 500, 30);
        ofSetColor(255);
        ofDrawBitmapString("Camcorder mode ON - getting through the Camcorder data now", 20, 20);
        ofPopStyle();
        
        if(dir.isValidIndex(dirIdx)){
            //this is the underUeath no effect live feed layer for camcorder mode1 (because cam cvImages cnt be too big, slowww)
            if(camScan == true){
                ofPushMatrix();
                float transY = (1+sin(ofGetFrameNum()*0.005))/2*ofGetHeight()*1.75;
                ofTranslate(ofGetWidth(), -transY);
                ofRotateDeg(90);
                ofFill();
                ofSetColor(255);
                camImg.draw(0,0,ofGetWidth()*1.75,ofGetHeight()*1.75);
                ofPopMatrix();
            }
            
            //start drawing top layers:
            ofPushMatrix();
            ofTranslate(ofGetWidth()/2-camW/2, ofGetHeight()/2-camH/2);
            ///////////////////////////////////////
            //use fbo to convert syphon to image: https://forum.openframeworks.cc/t/ofxsyphon-to-image/7791/4
            ///////////////////////////////////////
            fbo.begin();
            client.draw(0, 0, camW,camH);//not appeared
            fbo.end();
            fbo.readToPixels(camPix);
            
            if(camScan == true){
            //imgBuffer
            //When you run the program now, you should see the latest (most recent) frame drawn on the top-left part of the screen.
            if (imgBuffer.size()>0){
                imgBuffer[0].draw(0,0);
            }
            //where you're going to store the exact location of the slit. We're going to use that to draw a virtual red slit so that we know what we're scanning.
            int sclitLocX;
            //sclitLocX = camW/4;
            sclitLocX = camW/2;
            //float a = sin(ofGetFrameNum()*0.01)*50;
            float a = sin(ofGetFrameNum()*0.005)*camW/2;
            
            //set the img back to white filter (normal color)
            ofSetColor(255);
            //create a for loop that will loop over all images of the imgBuffer. Use the size() function to know when to stop.
            for(int i=0; i<imgBuffer.size(); i++){
                //imgBuffer[i].drawSubsection(i, 0, 1, imgSc.getHeight(), sclitLocX+a, 0);
                imgBuffer[i].drawSubsection(i, 0, 1, camH, sclitLocX+a, 0);
            }
            
            ofSetColor(255,0,0);
            ofNoFill();
            ofDrawLine(sclitLocX+a,0,sclitLocX+a,camH);
            }//'mode 1'
            ofPopMatrix();
            
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++
            if(frameDiff==true){
                drawFrameDiff4();
            }

        }//'dir'
    }//'s'
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
    switch (key) {
        case 'i':
            info = !info;
            break;
            
        case 'd':
            debug = !debug;
            break;
            
        case '0':
            kScan = !kScan;
            break;
            
        case 's':
            syphonData = !syphonData;
            //press any key to move through all available Syphon servers
            if (dir.size() > 0)
            {
                dirIdx++;
                if(dirIdx > dir.size() - 1)
                    dirIdx = 0;
                
                client.set(dir.getDescription(dirIdx));
                string serverName = client.getServerName();
                string appName = client.getApplicationName();
                
                if(serverName == ""){
                    serverName = "null";
                }
                if(appName == ""){
                    appName = "null";
                }
                ofSetWindowTitle(serverName + ":" + appName);
            }
            else
            {
                ofSetWindowTitle("No Server");
            }
            break;
            
        case '1':
            camScan = !camScan;
            break;
            
        case '2':
            frameDiff = !frameDiff;
            break;
            
        case '3':
            textile = 1;
            break;
            
        case '4':
            textile = 2;
            break;
            
        case '5':
            textile = 3;
            break;
            
        case '6':
            textile = 4;
            break;
            
        case '7':
            textile = 5;
            break;
            
        case '8':
            physicalMode = 2;
            textile = 6;
            break;
            
        case '9':
            physicalMode = 1;
            break;
 
        case 'o':
            opf = !opf;
            break;
            
        case '(':
            wind = !wind;
            break;
            
        case ')':
            swimming = !swimming;
            break;
            
        case '*':
            forcing = !forcing;
            break;
            
        case '&':
            sheltering = !sheltering;
            break;
            
        case 'H':
            hideinfo = !hideinfo;
            break;
            
    }
    
    if (wind){
        windshelter.key(key, contourFinder.blobs.size(), contourFinder.blobs[0].boundingRect.height);
    }
    
    if (swimming){
        swim.key(key, contourFinder.blobs.size(), contourFinder.blobs[0].boundingRect.height);
    }
    
    if (forcing){
        force.key(key);
    }
    
    if (sheltering){
        shelter.key(key, contourFinder.blobs.size(), contourFinder.blobs[0].boundingRect.height);
    }
    
    myKinectKeys(key);
        
}
//===============================================================
void ofApp::sphere(float p, float z, float rtX, float rad){
    ofPushMatrix();
    ofPushStyle();
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2,z);
    //ofRotateDeg(90);
    ofRotateXDeg(ofGetFrameNum()*0.01*rtX);//or90/30
    //ofRotateZ(90);
    ofRotateYDeg(p);
    float s = abs(sin(ofDegToRad(p))) + 0.3;
    
    ofDrawCircle(0, 0, 0, rad);
    ofPopMatrix();
    ofPopStyle();
}
//===============================================================
void ofApp::myKinectSetup(int _near, int _far){
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // enable depth->video image calibration
    kinect.setRegistration(true);
    
    kinect.init();//shows RGB video image
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)
    
    kinect.open();        // opens first available kinect
    //kinect.open(1);    // open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00362A08602047A");    // open a kinect using it's unique serial #
    
    // print the intrinsic IR sensor values
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    
    // Adjust the distance threshold here:
    nearThreshold = _near;
    farThreshold = _far;
    bThreshWithOpenCV = true;
    
    // zero the tilt on startup
    angle = 0;
    kinect.setCameraTiltAngle(angle);
    
}
//--------------------------------------------------------------
void ofApp::myKinectUpdate(){
    ofBackground(100, 100, 100);
    
    kinect.update();
    
    // there is a new frame and we are connected
    if(kinect.isFrameNew()) {
        
        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(kinect.getDepthPixels());
        
        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        if(bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        }
        
        // update the cv images
        grayImage.flagImageChanged();
        
        //smooth the image
        blurredImg = grayImage;
        blurredImg.blurGaussian( 2 );
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
        
    }
}
//--------------------------------------------------------------
void ofApp::myKinectDrawInstruct(){
    // draw instructions
    ofSetColor(255, 255, 255);
    stringstream reportStream;
    
    if(kinect.hasAccelControl()) {
        reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
        << ofToString(kinect.getMksAccel().y, 2) << " / "
        << ofToString(kinect.getMksAccel().z, 2) << endl;
    } else {
        reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
        << "motor / led / accel controls are not currently supported" << endl << endl;
    }
    
    reportStream    << "set near threshold " << nearThreshold << " (press: + -)" << endl << "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
    << ", fps: " << ofGetFrameRate() << endl;
    
    if(kinect.hasCamTiltControl()) {
        reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl;
    }
    
    ofDrawBitmapString(reportStream.str(), 20, 662);
    
    ofDrawBitmapString("KINECT MODE", 20, 642);
    ofDrawBitmapString("press '0' zero to go to/exit the first Kinect interaction - Figure scan data visualization", 20, 732);
    ofDrawBitmapString("press 'o' to go to/exit the second Kinect interaction - Physical body world - opticalFlow", 20, 752);
    ofDrawBitmapString("- 'o' to '9' go to the 1st garment setup", 20, 770);
    ofDrawBitmapString("- 'o' to '8' go to the 2nd garment setup", 20, 785);
    
    ofDrawBitmapString("press '('  go to/exit the third Kinect interaction - Physical body world - blobTracking (the 3rd garment setup", 20, 815);
    ofDrawBitmapString("press ')'  go to/exit the third Kinect interaction - Physical body world - blobTracking (the 4th garment setup", 20, 830);
    ofDrawBitmapString("press '*'  go to/exit the third Kinect interaction - Physical body world - blobTracking (the 5th garment setup", 20, 845);
    ofDrawBitmapString("press '&' and 'o' go to/exit the third Kinect interaction - Physical body world - blobTracking & opticalFlow (the 6th garment setup", 20, 860);

}
//--------------------------------------------------------------
void ofApp::myKinectKeys(int key){
    switch (key) {
         
        case '>':
        case '.':
            farThreshold ++;
            if (farThreshold > 255) farThreshold = 255;
            break;
            
        case '<':
        case ',':
            farThreshold --;
            if (farThreshold < 0) farThreshold = 0;
            break;
            
        case '+':
        case '=':
            nearThreshold ++;
            if (nearThreshold > 255) nearThreshold = 255;
            break;
            
        case '-':
            nearThreshold --;
            if (nearThreshold < 0) nearThreshold = 0;
            break;
            
        case 'w':
            kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
            break;
            
            
        case OF_KEY_UP:
            angle++;
            if(angle>30) angle=30;
            kinect.setCameraTiltAngle(angle);
            break;
            
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30) angle=-30;
            kinect.setCameraTiltAngle(angle);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::exit() {
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
}
//===============================================================
//these are our directory's callbacks
void ofApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

void ofApp::serverUpdated(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Updated")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

void ofApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}
//===============================================================
void ofApp::frameDiffSetup(){
    grayImageCurr.allocate(ofGetWidth(), ofGetHeight());
    grayImagePrev.allocate(ofGetWidth(), ofGetHeight());
    bufferFloat.allocate(ofGetWidth(), ofGetHeight());
    diffFloat.allocate(ofGetWidth(), ofGetHeight());
    
    camPix2.allocate(ofGetWidth(),ofGetHeight(),1);
    fbo2.allocate(ofGetWidth(),ofGetHeight(),GL_RGB);
    colorImage.allocate(ofGetWidth(),ofGetHeight());
    
    textile1.load("textile_fishing.jpg");
    textile2.load("textile_sunShelter.jpg");
    textile3.load("textile_develop.jpg");
    textile4.load("textile_laundryBag.jpg");
    textile5.load("textile_motor.jpg");
    textile6.load("textile_swim.jpg");
    textile = 0;
}
//--------------------------------------------------------------
void ofApp::frameDiffUpdate(float _diffFloat, float _buffFloat){
    //Store the previous frame, if it exists till now
    //(This must at top of update)
    if ( grayImageCurr.bAllocated ) {
        grayImagePrev = grayImageCurr;
    }
    
    fbo.begin();
    client.draw(0, 0, ofGetWidth(), ofGetHeight());//not appeared
    fbo.end();
    fbo.readToPixels(camPix2);//(pix was set for cvColor image in update)
    colorImage.setFromPixels(camPix2);//CVcolorImage get the pixel
    grayImageCurr = colorImage;
    
    //Do processing if grayImagePrev is inited
    if ( grayImagePrev.bAllocated ) {
        //Get absolute difference
        diff.absDiff( grayImageCurr, grayImagePrev );
        
        //We want to amplify the difference to obtain better visibility of motion
        //We do it by multiplication
        //But to do it, we need to convert diff to float image first
        diffFloat = diff;    //Convert to float image
        diffFloat *= _diffFloat;    //Amplify the pixel values (original = 5.0)
        
        //Update the accumulation buffer
        if ( !bufferFloat.bAllocated ) {
            //If the buffer is not initialized, then
            //just set it equal to diffFloat
            bufferFloat = diffFloat;
        }else {
            //Slow damping the buffer to zero
            bufferFloat *= _buffFloat; //original = 0.85
            
            //Add current difference image to the buffer
            bufferFloat += diffFloat;
            bufferFloat.erode(); //helps get rid of noise (remove if not needed)
        }
    }
}
//--------------------------------------------------------------
void ofApp::drawFrameDiff4(){
    if ( diffFloat.bAllocated ) {
        //Get image dimensions
        int w = grayImageCurr.width;
        int h = grayImageCurr.height;

            //**-------------------------------
        ofSetColor(255);
        if(textile == 1) textile1.draw(0,0,w,h);
        if(textile == 2) textile2.draw(0,0,w,h);
        if(textile == 3) textile3.draw(0,0,w,h);
        if(textile == 4) textile4.draw(0,0,w,h);
        if(textile == 5) textile5.draw(0,0,w,h);
        if(textile == 6) textile6.draw(0,0,w,h);

        //Draw the motion areas----------------------------------
        //Get bufferFloat pixels
        float *pixels = bufferFloat.getPixelsAsFloats();
        //Scan all pixels
        for (int y=0; y<h; y+=1){
            for (int x=0; x<w; x+=1){
                //Get the pixel value
                float value = pixels[ x + w * y ];
                //If value exceed threshold, then draw pixel---------------
                if ( value >= 0.9 ) {//original was 0.9
                    //[ACTUALLY CONTENTS]-----------------------------------
                    if(ofRandom(1)<1){//if the pix go wrong check here
                        //**-----------
                        ofPushStyle();
                        //ofSetColor(camPix.getColor(x,y));
                        ofSetColor(255);
                        ofDrawRectangle( x, y, 1, 1);
                        ofPopStyle();
                    }
                }//if value>
            }//for
        }//for
    }//diffFloat allocated
}
//===============================================================
void ofApp::opfUpdate(){
    if ( gray1.bAllocated ) {
        gray2 = gray1;
        calculatedFlow = true;
    }
    
    //Convert to ofxCv images
    ofPixels & pixels = kinect.getPixels();
    //ofPixels & pixels = camPix;
    currentColor.setFromPixels( pixels );
    
    float decimate = 0.25;              //Decimate images to 25% (makes calculations faster + works like a blurr too)
    ofxCvColorImage imageDecimated1;
    imageDecimated1.allocate( currentColor.width * decimate, currentColor.height * decimate );
    imageDecimated1.scaleIntoMe( currentColor, CV_INTER_AREA );             //High-quality resize
    gray1 = imageDecimated1;
    
    if ( gray2.bAllocated ) {
        cv::Mat img1 = cv::cvarrToMat(gray1.getCvImage());  //Create OpenCV images
        cv::Mat img2 = cv::cvarrToMat(gray2.getCvImage());
        cv::Mat flow;                        //Image for flow
        calcOpticalFlowFarneback( img1, img2, flow, 0.7, 3, 11, 5, 5, 1.1, 0 );
        //0.7, 3, 11, 5, 5, 1.1, 0
        //Split flow into separate images
        vector<cv::Mat> flowPlanes;
        split( flow, flowPlanes );
        //Copy float planes to ofxCv images flowX and flowY
        IplImage iplX( flowPlanes[0] );
        flowX = &iplX;
        IplImage iplY( flowPlanes[1] );
        flowY = &iplY;
    }
}
//--------------------------------------------------------------
void ofApp::opfDraw(){
    //reset statistics about avg direction
    sumX = sumY = avgX = avgY = numOfEntries = 0;
    
    if (calculatedFlow){
        ofSetColor( 255);
        kinect.draw( 0, 0, ofGetHeight()/480*640, ofGetHeight());
        if(sheltering==false){
            ofSetColor( 255 );
            client.draw(ofGetWidth()/2,0, ofGetWidth(), ofGetHeight());
        }
        ofPushStyle();
        ofSetColor(255,0,0);
        ofDrawBitmapString("press '9' or '8' or '&' to see different mode", 20, 20);
        ofDrawBitmapString("it takes around 30 sec for the shapes to birth, during the time it would run very slow, but not crashing, don't worry just wait for a bit Xx", 20, 40);
        ofPopStyle();
        
        int w = gray1.width;
        int h = gray1.height;
        
        //1. Input images + optical flow
//        ofPushMatrix();
//        ofScale( 4, 4 );
        
        //Optical flow
        float *flowXPixels = flowX.getPixelsAsFloats();
        float *flowYPixels = flowY.getPixelsAsFloats();
        ofSetColor( 0, 0, 255 );
        for (int y=0; y<h; y+=10) {
            for (int x=0; x<w; x+=30) {
                float fx = flowXPixels[ x + w * y ];
                float fy = flowYPixels[ x + w * y ];
                //Draw only long vectors
                if ( fabs( fx ) + fabs( fy ) > .5 ) {
                    //add to total direction
                    sumX+=fx;
                    sumY+=fy;
                    numOfEntries++;
                }
            }
        }
//        ofPopMatrix();
    }
    
    //calculate average direction vector
    if (numOfEntries>0){
        avgX = sumX / numOfEntries;
        avgY = sumY / numOfEntries;
    }
    
    ofPoint avgDir = ofPoint(avgX, avgY);
    //Clamp optical flow direction amount range
    avgDir.x = ofClamp(avgDir.x, -4, 4);
    avgDir.y = ofClamp(avgDir.y, -4, 4);
    
    ofPushStyle();
    if(sheltering ==false){
        if (physicalMode==1) {
            pullgravity.draw(avgDir.x, avgDir.y);
        }
        if (physicalMode==2) {
            rainbowGravity.draw(avgDir.x, avgDir.y);
        }
    }
    if (sheltering) shelter.drawOpf(avgDir.x, avgDir.y);
    ofPopStyle();
    
}
