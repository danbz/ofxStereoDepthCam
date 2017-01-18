#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    vector<ofVideoDevice> devs = ofxStereoDepthCam::listAvailableDevices();
    for(int i=0;i<devs.size();i++){
        cout<<devs[i].id<<" "<<devs[i].deviceName<<endl;
    }
    
    //do it manually...
    //cam.setup(0,2,320,240);
    //...or use a device template
    ofxStereoCameraTools::ELPCamera elpTemplate;
    cam.setup((ofxStereoCameraTools::DeviceTemplate *)&elpTemplate, 320, 240);
    
    //cam.setCalibration("calibration/elp_left.yml", "calibration/elp_right.yaml");
    
    imgLx.load("img/scene_l.jpg");
    imgRx.load("img/scene_r.jpg");
    cv::Mat cvLx = ofxCv::toCv(imgLx);
    cv::Mat cvRx = ofxCv::toCv(imgRx);
    ofPixels pix;
    cv::Mat disp = ofxStereoDepthCam::getDisparityMapBM(cvLx, cvRx);
    ofxCv::toOf(disp, pix);
    imgDptBM.setFromPixels(pix);
    
    cv::StereoSGBM sgbm;
    /*sgbm.SADWindowSize = 5;
    sgbm.numberOfDisparities = 192;
    sgbm.preFilterCap = 4;
    sgbm.minDisparity = -64;
    sgbm.uniquenessRatio = 1;
    sgbm.speckleWindowSize = 150;
    sgbm.speckleRange = 2;
    sgbm.disp12MaxDiff = 10;
    sgbm.fullDP = false;
    sgbm.P1 = 600;
    sgbm.P2 = 2400;*/
    sgbm.minDisparity = 0;
    sgbm.numberOfDisparities=32;
    sgbm.SADWindowSize=3;
    sgbm.P1=128;
    sgbm.P2=256;
    sgbm.disp12MaxDiff=20;
    sgbm.preFilterCap=16;
    sgbm.uniquenessRatio=1;
    sgbm.speckleWindowSize=100;
    sgbm.speckleRange=20;
    sgbm.fullDP=true;
    
    disp = ofxStereoDepthCam::getDisparityMapSGBM(cvLx, cvRx, &sgbm);
    ofxCv::toOf(disp, pix);
    imgDptSGBM.setFromPixels(pix);
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.drawRGB(0, 0);
    cam.drawDepth(0, cam.getHeight());
    
    ofPushMatrix();
    ofTranslate(cam.getWidth()*2, 0);
    imgLx.draw(0, 0);
    imgRx.draw(imgLx.getWidth(), 0);
    imgDptBM.draw(0,imgLx.getHeight());
    imgDptSGBM.draw(imgDptBM.getWidth(), imgLx.getHeight());
    ofPopMatrix();
    
    string s;
    s+="speckle win: "+ofToString(cam.getStereoBM()->state->speckleWindowSize)+"\n";
    s+="specke range: "+ofToString(cam.getStereoBM()->state->speckleRange)+"\n";
    //s+="SAD win: "+ofToString(cam.getStereoBM()->state->SADWindowSize)+"\n";
    s+="preFilterSize: "+ofToString(cam.getStereoBM()->state->preFilterSize)+"\n";
    s+="preFilterCap: "+ofToString(cam.getStereoBM()->state->preFilterCap)+"\n";
    s+="minDisparity: "+ofToString(cam.getStereoBM()->state->minDisparity)+"\n";
    s+="textureThreshold: "+ofToString(cam.getStereoBM()->state->textureThreshold)+"\n";
    s+="uniquenessRatio: "+ofToString(cam.getStereoBM()->state->uniquenessRatio)+"\n";
    s+="disp12MaxDiff: "+ofToString(cam.getStereoBM()->state->disp12MaxDiff)+"\n";
    ofDrawBitmapString(s, 10, 500);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case OF_KEY_UP:
            cam.getStereoBM()->state->speckleWindowSize++;
            break;
        case OF_KEY_DOWN:
            cam.getStereoBM()->state->speckleWindowSize--;
            break;
        case OF_KEY_LEFT:
            cam.getStereoBM()->state->speckleRange--;
            break;
        case OF_KEY_RIGHT:
            cam.getStereoBM()->state->speckleRange++;
            break;
        case 'q':
        //    cam.getStereoBM()->state->preFilterSize++;
            break;
        case 'a':
        //    cam.getStereoBM()->state->preFilterSize--;
            break;
        case 'w':
        //    cam.getStereoBM()->state->preFilterCap++;
            break;
        case 's':
        //    cam.getStereoBM()->state->preFilterCap--;
            break;
        case 'e':
            cam.getStereoBM()->state->minDisparity++;
            break;
        case 'd':
            cam.getStereoBM()->state->minDisparity--;
            break;
        case 'r':
            cam.getStereoBM()->state->textureThreshold++;
            break;
        case 'f':
            cam.getStereoBM()->state->textureThreshold--;
            break;
        case 't':
            cam.getStereoBM()->state->uniquenessRatio++;
            break;
        case 'g':
            cam.getStereoBM()->state->uniquenessRatio--;
            break;
        case 'y':
            cam.getStereoBM()->state->disp12MaxDiff++;
            break;
        case 'h':
            cam.getStereoBM()->state->disp12MaxDiff--;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
