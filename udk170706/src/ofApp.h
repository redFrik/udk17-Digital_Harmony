#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSyphon.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void keyReleased(int key);
    bool info;          //display info flag
    
    ofxSyphonServerDirectory dir;
    ofxSyphonClient client1, client2, client3;
    ofxIntSlider c1, c2, c3;
    ofxFloatSlider i1, i2, i3;
    ofxPanel gui;
    void c1Changed(int &val);
    void c2Changed(int &val);
    void c3Changed(int &val);
};
