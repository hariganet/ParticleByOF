#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"

#include "Particle.h"
#include "vectorField.h"

class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    //cv::VideoCapture cap;
    
    cv::Mat sakura;
    
	cv::Mat src;
	cv::Mat gray;
	cv::Mat prevGray;
	cv::Mat subGray;
	
	cv::Mat bin;
	cv::Mat canny;
	cv::Mat cannys, bins;
	cv::Mat canvasBW;
    cv::Mat canvasColor;
	cv::Mat* show;
    
    cv::Mat img;
	
	
	int key;
	
	int thr;
	int sigma;
	
    bool isEndProcess = false;
    
	vectorField VF;
    std::vector <Particle> particles;
    
    float gravity;
    float sideWind;
    
    
    
};
