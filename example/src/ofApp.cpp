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
    //cam.setup(0,1,1344,376);
    //...or use a device template
    ofxStereoCameraTools::ELPCamera elpTemplate;
    cam.setup((ofxStereoCameraTools::DeviceTemplate *)&elpTemplate, 320, 240);
    
    cam.setCalibration("calibration/elp_left.yml", "calibration/elp_right.yaml");
    
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
    
    
    ////  added in zed setup
    vidGrabber.setDeviceID(1);
    vidGrabber.setUseTexture(true);
    vidGrabber.setPixelFormat(OF_PIXELS_RGB);
    vidGrabber.setDesiredFrameRate(15);
    //vidGrabber.setup(2160, 720);
    vidGrabber.initGrabber(1344, 376);

    b_zed = true;
    b_paintMesh = true;
    b_drawSource = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    vidGrabber.update();
    
    if (b_zed) {
        if(vidGrabber.isFrameNew()){
            grabbedImage = vidGrabber.getPixels();
            
            imgLx.cropFrom(grabbedImage, 0, 0, grabbedImage.getWidth()/2, grabbedImage.getHeight());
            imgRx.cropFrom(grabbedImage, grabbedImage.getWidth()/2 , 0, grabbedImage.getWidth()/2 , grabbedImage.getHeight());
            colorImage.cropFrom(grabbedImage, 0, 0, grabbedImage.getWidth()/2, grabbedImage.getHeight());
        }
    }

    //cam.update();

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
    
    
    ///create mesh
    
    ofColor col;
    ofColor zGrey = 0;
    
    int step =1;
    int width = imgLx.getWidth();
    int height = imgLx.getHeight();
    int z = 0;
    ofVec3f v3;
    for(int y = 0; y < height; y += step) {
        for(int x = 0; x < width; x +=  step) {
            zGrey = imgDptSGBM.getPixels()[x+y*width];
            z = zGrey.r;
            //cout << z << endl;
            v3.set(0,0,0);
            if (b_paintMesh) {
                col = (colorImage.getColor(x,y)); // getting RGB from ofShortImage
            } else {
                float h = ofMap(z, 0, 255, 0, 255, true);
                col.setHsb(h, 255, 255);
            }
            
            // v3.set((x - (width)*0.002)  ,(y -(height)*0.002) , 255-z*1.0 );
            v3.set(x   , y  , z     );
            mesh.addVertex(v3);
            mesh.addColor(col);
        }
    } // end create mesh
    
}

//--------------------------------------------------------------
void ofApp::draw(){
   // vidGrabber.draw(0, 0, vidGrabber.getWidth()/2, vidGrabber.getHeight()/2);

    //cam.drawRGB(0, 0, cam.getWidth()/2, cam.getHeight()/2);
    //cam.drawDepth(0, cam.getHeight());
//    
//    ofPushMatrix();
//    //ofTranslate(cam.getWidth()*2, 0);
//    imgLx.draw(0, 0);
//    imgRx.draw(imgLx.getWidth(), 0);
//    imgDptBM.draw(0,imgLx.getHeight());
//    imgDptSGBM.draw(imgDptBM.getWidth(), imgLx.getHeight());
//    ofPopMatrix();
//    
    
    
    
    ///
    
    
    
    if (b_drawSource){
        ofPushMatrix();
        //ofTranslate(cam.getWidth()*2, 0);
        imgLx.draw(0, 0);
        imgRx.draw(imgLx.getWidth(), 0);
        imgDptBM.draw(0,imgLx.getHeight());
        imgDptSGBM.draw(imgDptBM.getWidth(), imgLx.getHeight());
        ofPopMatrix();
        
    } else {
        
        
                mesh.setMode(OF_PRIMITIVE_POINTS);
//                break;
//                
//            case 2:
//                mesh.setMode(OF_PRIMITIVE_TRIANGLES);
//                break;
//                
//            case 3:
//                mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
//                break;
//        }
        
        easyCam.begin();
        glPointSize(4);
        ofPushMatrix();
        ofScale(1, -1, 1);  // the projected points are 'upside down' and 'backwards'
        ofTranslate(-200, -200, -200); // center the points a bit
        glEnable(GL_DEPTH_TEST);
        
        //mesh.drawVertices();
        //mesh.drawFaces();
        ofSetColor( 255, 255, 255);  //set render colour for unpainted points, faces and lines
        mesh.draw();
        glDisable(GL_DEPTH_TEST);
        mesh.clear();
        ofPopMatrix();
        easyCam.end();
        
    }
    
    
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
            
        case 'z':
            b_zed=!b_zed;
            break;
            
        case 'p':
            b_drawSource=!b_drawSource;
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
