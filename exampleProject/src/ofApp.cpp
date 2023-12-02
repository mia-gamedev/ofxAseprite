#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup(){

	spinner = new ofxAseprite("spinner.json", "spinner.png");
	spinner->setTag("Spin");
	spinner->play();
	spinner->setAnchorPercent(0.5,0.5);

	ofVec2f man_pos(ofGetWidth()/2, ofGetHeight()/2);
	ofVec2f man_size(160, 240);
	man = new man_object(man_pos,man_size,"man.json","man.png");
}

//--------------------------------------------------------------
void ofApp::update(){
	spinner->update(ofGetElapsedTimef());

	if (ofGetKeyPressed('w'))
	{
		man->move(UP);
	}
	if (ofGetKeyPressed('s'))
	{
		man->move(DOWN);
	}
	if (ofGetKeyPressed('a'))
	{
		man->move(LEFT);
	}
	if (ofGetKeyPressed('d'))
	{
		man->move(RIGHT);
	}

	man->update(ofGetElapsedTimef());
	ofResetElapsedTimeCounter();
}

//--------------------------------------------------------------
void ofApp::draw() {
	spinner->draw(ofGetWidth()/2,ofGetHeight()/2,2);
	man->draw();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == 'w' || key == 'a' || key == 's' || key == 'd')
	{
		man->stopMoving();
	}
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
