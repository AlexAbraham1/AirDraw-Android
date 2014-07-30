#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxAccelerometer.h"

#include "ofxOpenCv.h"
#include <vector>

#include "Circle.h";

class ofApp: public ofxAndroidApp {

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void windowResized(int w, int h);

	void touchDown(int x, int y, int id);
	void touchMoved(int x, int y, int id);
	void touchUp(int x, int y, int id);
	void touchDoubleTap(int x, int y, int id);
	void touchCancelled(int x, int y, int id);
	void swipe(ofxAndroidSwipeDir swipeDir, int id);

	void pause();
	void stop();
	void resume();
	void reloadTextures();

	bool backPressed();
	void okPressed();
	void cancelPressed();

	void cleanupCircles();
	bool isBlankSpace(int x, int y);
	void switchCamera();
	void switchInvert();
	void resetCoordinates();
	bool colorFound();
	void saveImage();
	void setColor(int x, int y);
	void changeBackground(bool selectedColor);

	shared_ptr<ofVideoGrabber> grabber;

	ofImage image;

	bool setupMode;
	bool drawMode;



	std::vector<Circle*> circles;

	int circleRadius;
	int xMargin, yMargin;
	int maxTimesDrawn;
	float minRed, minGreen, minBlue, maxRed, maxGreen, maxBlue, avgRed, avgGreen, avgBlue;
	int xLow, xHigh, yLow, yHigh;

	int DEVICE_ID;

	int screenWidth;
	int screenHeight;

	ofColor background;
	ofColor circleColor;

	ofTrueTypeFont verdana;

};
