#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("syphonmixer");
    dir.setup();
    client1.setup();
    client2.setup();
    client3.setup();
    gui.setup();
    gui.add(c1.setup("client1server", 0, 0, 5));
    gui.add(i1.setup("client1fade", 0.333, 0, 1));
    gui.add(c2.setup("client2server", 1, 0, 5));
    gui.add(i2.setup("client2fade", 0.333, 0, 1));
    gui.add(c3.setup("client3server", 2, 0, 5));
    gui.add(i3.setup("client3fade", 0.333, 0, 1));
    c1.addListener(this, &ofApp::c1Changed);
    c2.addListener(this, &ofApp::c2Changed);
    c3.addListener(this, &ofApp::c3Changed);
    c1= 0;
    c2= 1;
    c3= 2;
    info= false;
    for(int i= 0; i<dir.size(); i++) {
        ofxSyphonServerDescription d= dir.getDescription(i);
        cout<<ofToString(i)<<": "<<d.serverName<<" - "<<d.appName<<"\n";
    }
}
void ofApp::update() {
}
void ofApp::draw() {
    ofClear(0);
    ofEnableBlendMode(OF_BLENDMODE_ADD);  //could be something else
    ofSetColor(255, 255, 255, i1*255);
    client1.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    ofSetColor(255, 255, 255, i2*255);
    client2.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    ofSetColor(255, 255, 255, i3*255);
    client3.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    if(info) {
        ofSetHexColor(0xFFFFFF);
        int x= ofGetWindowWidth()*0.667;
        ofDrawBitmapString("screen resolution: "+ofToString(ofGetScreenWidth())+"x"+ofToString(ofGetScreenHeight()), x, 20);
        ofDrawBitmapString("window dimensions: "+ofToString(ofGetWindowWidth())+"x"+ofToString(ofGetWindowHeight()), x, 40);
        ofDrawBitmapString("framerate: "+ofToString(ofGetFrameRate()), x, 60);
        if(dir.isValidIndex(c1)) {
            ofxSyphonServerDescription d= dir.getDescription(c1);
            ofDrawBitmapString("client1: "+d.serverName+" - "+d.appName, x, 80);
        }
        if(dir.isValidIndex(c2)) {
            ofxSyphonServerDescription d= dir.getDescription(c2);
            ofDrawBitmapString("client2: "+d.serverName+" - "+d.appName, x, 100);
        }
        if(dir.isValidIndex(c3)) {
            ofxSyphonServerDescription d= dir.getDescription(c3);
            ofDrawBitmapString("client3: "+d.serverName+" - "+d.appName, x, 120);
        }
        gui.draw();
    }
}
void ofApp::keyReleased(int key) {
    if(key=='i') {
        info= !info;
    }
}
void ofApp::c1Changed(int &val) {
    if(dir.isValidIndex(val)) {
        client1.set(dir.getDescription(val));
    }
}
void ofApp::c2Changed(int &val) {
    if(dir.isValidIndex(val)) {
        client2.set(dir.getDescription(val));
    }
}
void ofApp::c3Changed(int &val) {
    if(dir.isValidIndex(val)) {
        client3.set(dir.getDescription(val));
    }
}
