//
//  ofxStereoDepthCam.h
//
//  Created by enrico<nausea>viola on 1/17/17.
//
//

#pragma once
#include "ofMain.h"
#include "ofxStereoCameraTools.h"
#include "ofxCv.h"

class ofxStereoDepthCam{
public:
    
    enum StereoMode{
        STEREO_BM,
        STEREO_SGBM
    };
    
    ofxStereoDepthCam();
    ~ofxStereoDepthCam();
    void setupCamera(ofVideoGrabber & _cam, vector<ofVideoDevice> & _devs, string devName, int w, int h);
    void setupCamera(ofVideoGrabber & _cam, int devId, int w, int h);
    inline void setupLeftCamera(int devId, int w, int h){setupCamera(camLx, devId, w, h);}
    inline void setupRightCamera(int devId, int w, int h){setupCamera(camRx, devId, w, h);}
    inline void setupLeftCamera(vector<ofVideoDevice> & _devs, string devName, int w, int h){setupCamera(camLx, _devs, devName, w, h);}
    inline void setupRightCamera(vector<ofVideoDevice> & _devs, string devName, int w, int h){setupCamera(camRx, _devs, devName, w, h);}
    void setup(int devLx, int devRx, int w, int h, StereoMode sm=STEREO_BM);
    void setup(ofxStereoCameraTools::DeviceTemplate * tmp, int w, int h, StereoMode sm=STEREO_BM);
    void setCalibration(string pthLx, string pthRx);
    void setStereoMode(StereoMode sm);
    
    void update();
    
    void drawRGB(float x, float y, float w, float h);
    void drawDepth(float x, float y, float w, float h);
    inline void drawRGB(float x, float y){drawRGB(x,y,width,height);}
    inline void drawDepth(float x, float y){drawDepth(x,y,width,height);}
    
    inline ofVideoGrabber * getLeftCamera(){return & camLx;}
    inline ofVideoGrabber * getRightCamera(){return & camRx;}
    inline float getWidth(){return width;}
    inline float getHeight(){return height;}
    inline bool isLeftCalibrated(){return bLxCal;}
    inline bool isRightCalibrated(){return bRxCal;}
    inline bool areBothCalibrated(){return (bLxCal && bRxCal);}
    inline StereoMode getStereoMode(){return curStereoMode;}
    inline cv::StereoBM * getStereoBM(){return sbm;}
    inline cv::StereoSGBM * getStereoSGBM(){return sgbm;}
    
    static vector<ofVideoDevice> listAvailableDevices();
    
    template<typename S>
    static cv::Mat getDisparityMap(cv::Mat & lx, cv::Mat & rx, S * stereo=NULL, bool bNormalize=true);
    static cv::Mat getDisparityMapBM(cv::Mat & lx, cv::Mat & rx, cv::StereoBM * sbm=NULL, bool bNormalize=true);
    static cv::Mat getDisparityMapSGBM(cv::Mat & lx, cv::Mat & rx, cv::StereoSGBM * sgbm=NULL, bool bNormalize=true);
protected:
    void cleanStereoPointers();
    
    cv::StereoBM * sbm;
    cv::StereoSGBM * sgbm;
    ofxCv::Calibration calibLx;
    ofxCv::Calibration calibRx;
    ofVideoGrabber camLx;
    ofVideoGrabber camRx;
    ofImage imgUndLx;
    ofImage imgUndRx;
    cv::Mat frameLx;
    cv::Mat frameRx;
    ofImage imgDpt;
    float width;
    float height;
    StereoMode curStereoMode;
    bool bLxCal;
    bool bRxCal;
};