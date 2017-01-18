//
//  ofxStereoCameraTools.h
//
//  Created by enrico<nausea>viola on 1/17/17.
//
//

#pragma once
#include "ofMain.h"

namespace ofxStereoCameraTools {
    class DeviceTemplate{
    public:
        string leftName;
        string rightName;
    };
    
    class ELPCamera : DeviceTemplate{
    public:
        ELPCamera(){
            leftName = "Stereo Vision 1";
            rightName = "Stereo Vision 2";
        }
    };
}