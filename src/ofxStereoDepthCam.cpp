//
//  ofxStereoDepthCam.cpp
//
//  Created by enrico<nausea>viola on 1/17/17.
//
//

#include "ofxStereoDepthCam.h"

ofxStereoDepthCam::ofxStereoDepthCam(){
    width=0;
    height=0;
    bLxCal=false;
    bRxCal=false;
    sbm = NULL;
    sgbm = NULL;
}

ofxStereoDepthCam::~ofxStereoDepthCam(){
    cleanStereoPointers();
}

void ofxStereoDepthCam::cleanStereoPointers(){
    if(sbm!=NULL){
        delete sbm;
    }
    if(sgbm!=NULL){
        delete sgbm;
    }
}

vector<ofVideoDevice> ofxStereoDepthCam::listAvailableDevices(){
    vector<ofVideoDevice> devs;
    ofVideoGrabber cam;
    devs = cam.listDevices();
    return devs;
}

void ofxStereoDepthCam::setStereoMode(ofxStereoDepthCam::StereoMode sm){
    cleanStereoPointers();
    switch(sm){
        case STEREO_BM:
            /*sbm = new cv::StereoBM();
            sbm->state->SADWindowSize = 9;
            sbm->state->numberOfDisparities = 112;
            sbm->state->preFilterSize = 5;
            sbm->state->preFilterCap = 61;
            sbm->state->minDisparity = -39;
            sbm->state->textureThreshold = 507;
            sbm->state->uniquenessRatio = 0;
            sbm->state->speckleWindowSize = 0;
            sbm->state->speckleRange = 8;
            sbm->state->disp12MaxDiff = 1;*/
            sbm = new cv::StereoBM();
            sbm->state->SADWindowSize = 9;
            sbm->state->numberOfDisparities = 112;
            sbm->state->preFilterSize = 5;
            sbm->state->preFilterCap = 61;
            sbm->state->minDisparity = -39;
            sbm->state->textureThreshold = 507;
            sbm->state->uniquenessRatio = 0;
            sbm->state->speckleWindowSize = 0;
            sbm->state->speckleRange = 8;
            sbm->state->disp12MaxDiff = 1;
            break;
        case STEREO_SGBM:
            sgbm = new cv::StereoSGBM();
            sgbm->SADWindowSize = 5;
            sgbm->numberOfDisparities = 192;
            sgbm->preFilterCap = 4;
            sgbm->minDisparity = -64;
            sgbm->uniquenessRatio = 1;
            sgbm->speckleWindowSize = 150;
            sgbm->speckleRange = 2;
            sgbm->disp12MaxDiff = 10;
            sgbm->fullDP = false;
            sgbm->P1 = 600;
            sgbm->P2 = 2400;
            break;
        default:
            break;
    }
}

void ofxStereoDepthCam::setupCamera(ofVideoGrabber & _cam, int devId, int w, int h){
    _cam.setDeviceID(devId);
    _cam.initGrabber(w, h);
    width=w;
    height=h;
}

void ofxStereoDepthCam::setupCamera(ofVideoGrabber & _cam, vector<ofVideoDevice> & _devs, string devName, int w, int h){
    for(int i=0;i<_devs.size();i++)
        if(ofStringTimesInString(_devs[i].deviceName, devName)>0){
            setupCamera(_cam, _devs[i].id, w, h);
            i=_devs.size()+2;
        }
}

void ofxStereoDepthCam::setup(int devLx, int devRx, int w, int h, ofxStereoDepthCam::StereoMode sm){
    setupLeftCamera(devLx, w, h);
    setupRightCamera(devRx, w, h);
    imgDpt.allocate(w, h, OF_IMAGE_COLOR);
    setStereoMode(sm);
}

void ofxStereoDepthCam::setup(ofxStereoCameraTools::DeviceTemplate * tmp, int w, int h, ofxStereoDepthCam::StereoMode sm){
    vector<ofVideoDevice> devs = listAvailableDevices();
    setupLeftCamera(devs, tmp->leftName, w, h);
    setupRightCamera(devs, tmp->rightName, w, h);
    imgDpt.allocate(w, h, OF_IMAGE_COLOR);
    setStereoMode(sm);
}

void ofxStereoDepthCam::setCalibration(string pthLx, string pthRx){
    calibLx.load(pthLx);
    //calibRx.load(pthRx);
    ofxCv::imitate(imgUndLx, camLx);
    bLxCal=true;
    bRxCal=true;
}

//sample params:
    /*sbm->state->SADWindowSize = 9;
    sbm->state->numberOfDisparities = 112;
    sbm->state->preFilterSize = 5;
    sbm->state->preFilterCap = 61;
    sbm->state->minDisparity = -39;
    sbm->state->textureThreshold = 507;
    sbm->state->uniquenessRatio = 0;
    sbm->state->speckleWindowSize = 0;
    sbm->state->speckleRange = 8;
    sbm->state->disp12MaxDiff = 1;*/

template<typename S>
cv::Mat ofxStereoDepthCam::getDisparityMap(cv::Mat & lx, cv::Mat & rx, S * stereo, bool bNormalize){
    cv::Mat disp;
    cv::Mat glx;
    cv::Mat grx;
    if(lx.channels()==3){
        cv::cvtColor(lx, glx, CV_BGR2GRAY);
        cv::cvtColor(rx, grx, CV_BGR2GRAY);
    }else{
        glx=lx;
        grx=rx;
    }
    bool bClean = false;
    if(stereo==NULL){
        stereo = new S();
    }
    
    stereo->operator()(glx, grx, disp);
    
    if(bClean){
        delete stereo;
    }
    
    if(bNormalize){
        cv::Mat dispN;
        cv::normalize(disp, dispN, 0, 255, CV_MINMAX, CV_8U);
        return dispN;
    }else{
        return disp;
    }
    
}

cv::Mat ofxStereoDepthCam::getDisparityMapBM(cv::Mat &lx, cv::Mat &rx, cv::StereoBM * sbm, bool bNormalize){
    return getDisparityMap<cv::StereoBM>(lx, rx, sbm, bNormalize);
}

cv::Mat ofxStereoDepthCam::getDisparityMapSGBM(cv::Mat &lx, cv::Mat &rx, cv::StereoSGBM * sgbm, bool bNormalize){
    return getDisparityMap<cv::StereoSGBM>(lx, rx, sgbm, bNormalize);
}

void ofxStereoDepthCam::update(){
    camLx.update();
    camRx.update();
    
    if(camLx.isFrameNew() || camRx.isFrameNew()){
        
        frameLx = ofxCv::toCv(camLx.getPixels());
        frameRx = ofxCv::toCv(camRx.getPixels());
        
        cv::Mat disp;
        if(curStereoMode==STEREO_BM){
            disp = getDisparityMapBM(frameLx, frameRx, sbm);
        }else{
            disp = getDisparityMapSGBM(frameLx, frameRx, sgbm);
        }
        
        ofPixels pix;
        ofxCv::toOf(disp, pix);
        imgDpt.setFromPixels(pix);
    }
    
}

void ofxStereoDepthCam::drawRGB(float x, float y, float w, float h){
    ofPushMatrix();
    ofTranslate(x,y);
    camLx.draw(0,0,w,h);
    camRx.draw(w,0,w,h);
    ofPopMatrix();
}

void ofxStereoDepthCam::drawDepth(float x, float y, float w, float h){
    imgDpt.draw(x,y,w,h);
}