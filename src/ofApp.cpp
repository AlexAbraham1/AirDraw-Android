#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofBackground(255, 255, 255);
	ofSetLogLevel (OF_LOG_VERBOSE);
	verdana.loadFont("verdana.ttf",25);

	screenWidth = 640;
	screenHeight = 480;

	ofEnableDepthTest(); //Used to calculate where to draw elements based on Z index instead of order drawn

	grabber = shared_ptr<ofVideoGrabber>(new ofVideoGrabber());
	grabber->setPixelFormat(OF_PIXELS_RGB);
	DEVICE_ID = 1;
	grabber->setDeviceID(DEVICE_ID);
	grabber->initGrabber(screenWidth, screenHeight);

	image.allocate(screenWidth, screenHeight, OF_IMAGE_COLOR);

	setupMode = true;

	circleRadius = 5;
	xMargin = 25;
	yMargin = 15;

	maxTimesDrawn = 1000;

	minRed = minGreen = minBlue = 255;

	maxRed = maxGreen = maxBlue = 0;

	avgRed = avgGreen = avgBlue = 0;

	background = ofColor(0,0,0);
	circleColor = ofColor(avgRed, avgGreen, avgBlue);

	drawMode = false;
	resetCoordinates();
}

//--------------------------------------------------------------
void ofApp::update(){

    grabber->update();
    if (grabber->isFrameNew()) {

        resetCoordinates();

        image.setFromPixels(grabber->getPixelsRef());
        image.mirror(false, true);

        if (!setupMode) {

            for (int x = 0; x < screenWidth; x++) {
                for (int y = 0; y < screenHeight; y++) {
                    ofColor color = image.getColor(x,y);
                    float red = float(color.r);
                    float green = float(color.g);
                    float blue = float(color.b);

                    if (ofInRange(red, minRed, maxRed) &&
                        ofInRange(green, minGreen, maxGreen) &&
                        ofInRange(blue, minBlue, maxBlue)) {


                        if (x < xLow) {
                            xLow = x;
                        }
                        if (x > xHigh) {
                            xHigh = x;
                        }
                        if (y < yLow) {
                            yLow = y;
                        }
                        if (y > yHigh) {
                            yHigh = y;
                        }

                    }


                }
            }

            if (drawMode) {
                if (colorFound()) {
                    int x = (xLow + xHigh)/2;
                    int y = (yLow + yHigh)/2;

                    if (x < circleRadius) {
                        x = circleRadius;
                    }
                    if (y < circleRadius + yMargin) {
                        y = circleRadius + yMargin;
                    }
                    if (x > screenWidth - circleRadius - xMargin) {
                        x = screenWidth - circleRadius - xMargin;
                    }
                    if (y > screenHeight - circleRadius - yMargin) {
                        y = screenHeight - circleRadius - yMargin;
                    }

                    int z = 50;
                    x += 450;
                    Circle * circle = new Circle();

                    circle->x = x;
                    circle->y = y;
                    circle->z = z;
                    circle->radius = circleRadius;
                    circle->color = circleColor;

                    circles.push_back(circle);
                }
            }

        }
    }

}

//--------------------------------------------------------------
void ofApp::draw(){

    if (setupMode) {
        ofSetHexColor(0xFFFFFF);
        image.draw(450,0);

        ofPushStyle();
		ofSetHexColor(0x000000);
		verdana.drawString("Hold up the object you want to use and click on it.", 20, screenHeight + 30);
		verdana.drawString("Make sure the object is a unique color.", 20, screenHeight + 60);
		ofSetHexColor(0xFF0000);
		verdana.drawString("'ESC': Quit AirDraw", 20, screenHeight + 90);
		ofPopStyle();
    } else {
        ofSetColor(background);
        ofRect(450, 0, screenWidth, screenHeight);

        ofSetColor(circleColor);

        for (std::vector<Circle>::size_type i = 0; i != circles.size(); i++) {
            Circle * circle = circles[i];
            circle->drawCircle();
        }

        if (colorFound()) {
            int x = (xLow + xHigh)/2;
            int y = (yLow + yHigh)/2;

            if (x < circleRadius) {
                x = circleRadius;
            }
            if (y < circleRadius) {
                y = circleRadius;
            }
            if (x > screenWidth - circleRadius - xMargin) {
                x = screenWidth - circleRadius - xMargin;
            }
            if (y > screenHeight - circleRadius - yMargin) {
                y = screenHeight - circleRadius - yMargin;
            }

            x += 450;

            int radius = circleRadius;
            int z = 50;
            ofPushStyle();
            ofSetColor(circleColor);
            ofCircle(x, y, z, radius);
            ofPopStyle();
        }

		ofSetHexColor(0xFF0000);
		ofRect(0, 0, 150, 480);
		ofSetHexColor(0x00FF00);
		ofRect(150, 0, 150, 480);
		ofSetHexColor(0x0000FF);
		ofRect(300, 0, 150, 480);

		ofSetHexColor(0x000000);
		verdana.drawString("Draw", 25, screenHeight/2);
		verdana.drawString("Clear", 175, screenHeight/2);
		verdana.drawString("Save", 325, screenHeight/2);
    }

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	ofLog() << "Key Pressed: " << key;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	ofLog() << "Key Released: " << key;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id) {

}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id) {

}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id) {

	if (x > 450 && x < screenWidth+450 && y < screenHeight) { //Touching Video Feed
		if (setupMode) {

			setColor(x, y);
			setupMode = false;


		} else {
			drawMode = false;
			setupMode = true;
		}
	} else  if (!setupMode){
		if (y < screenHeight && x < 150) {
			drawMode = !drawMode;
		} else if (y < screenHeight && x < 300) {
			drawMode = false;
			circles.clear();
		} else if (y < screenHeight && x < 450) {
			saveImage();
		}

	}

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id) {
	saveImage();
}

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id) {

}

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id) {
	//swipeDir: 1 = UP, 2 = DOWN, 3 = LEFT, 4 = RIGHT

	if (swipeDir == 1) {
		ofLogNotice() << "Swipe Up, MaxRadius NOT changed";
	} else if (swipeDir == 2) {
		ofLogNotice() << "Swipe Down, MaxRadius NOT changed";
	}
}

//--------------------------------------------------------------
void ofApp::pause() {

}

//--------------------------------------------------------------
void ofApp::stop() {

}

//--------------------------------------------------------------
void ofApp::resume() {

}

//--------------------------------------------------------------
void ofApp::reloadTextures() {

}

//--------------------------------------------------------------
bool ofApp::backPressed() {
	return false;
}

//--------------------------------------------------------------
void ofApp::okPressed() {

}

//--------------------------------------------------------------
void ofApp::cancelPressed() {

}

void ofApp::cleanupCircles() {
    ofLog() << "cleanupCircles() started";
    stringstream ss;
    ss << circles.size();
    ofLog() << "NUMBER OF CIRCLES: " + ss.str();
    std::random_shuffle(circles.begin(), circles.end());
    ofLog() << "cleanupCircles() ended";
    ss.str("");
    ss << circles.size();
    ofLog() << "NUMBER OF CIRCLES: " + ss.str();
}

bool ofApp::isBlankSpace(int x, int y) {
    for (std::vector<Circle>::size_type i = 0; i != circles.size(); i++) {

        Circle * circle = circles[i];



        float x2 = static_cast<int>(pow(static_cast<double>(circle->x - x), 2)+.005);
        float y2 = static_cast<int>(pow(static_cast<double>(circle->y - y), 2)+.005);
        float radius2 = static_cast<int>(pow(static_cast<double>(circle->radius), 2)+.005);

        if ((x2 + y2) < (radius2)/2) {
            return false;
        }

    }
    return true;

}

void ofApp::resetCoordinates() {
    xLow = screenWidth;
    xHigh = 0;
    yLow = screenHeight;
    yHigh = 0;
}

bool ofApp::colorFound() {
    return (xLow != screenWidth && xHigh != 0 && yLow != screenHeight && yHigh != 0);
}

void ofApp::saveImage() {
	stringstream ss;
	ss << ofGetElapsedTimef();
	string filename = "../../../../DCIM/AirDraw/" + ss.str() + ".png";
    ofSaveScreen(filename);
    image.loadImage(filename);
    image.crop(0, 0, screenWidth, screenHeight);
    image.saveImage(filename);

    ofxAndroidToast("Drawing Saved");
}

void ofApp::setColor(int x, int y) {
	x -= 450;
    xLow = x - 5;
    xHigh = x + 5;
    yLow = y - 5;
    yHigh = y + 5;

    minRed = 255;
    minGreen = 255;
    minBlue = 255;
    maxRed = 0;
    maxGreen = 0;
    maxBlue = 0;

    for (int a = xLow; a < xHigh; a++) {
        for (int b = yLow; b < yHigh; b++) {
            ofColor color = image.getColor(a,b);
            float red = float(color.r);
            float green = float(color.g);
            float blue = float(color.b);

            if (red < minRed) {
                minRed = red;
            }
            if (green < minGreen) {
                minGreen = green;
            }
            if (blue < minBlue) {
                minBlue = blue;
            }

            if (red > maxRed) {
                maxRed = red;
            }
            if (green > maxGreen) {
                maxGreen = green;
            }
            if (blue > maxBlue) {
                maxBlue = blue;
            }


        }
    }

    avgRed = (minRed + maxRed)/2;
    avgGreen = (minGreen + maxGreen)/2;
    avgBlue = (minBlue + maxBlue)/2;

    minRed = avgRed - 15;
    minGreen = avgGreen - 15;
    minBlue = avgBlue - 15;

    maxRed = avgRed + 15;
    maxGreen = avgGreen + 15;
    maxBlue = avgBlue + 15;

    circleColor = ofColor(avgRed, avgGreen, avgBlue);

    ofLogNotice() << "minRed: " << minRed << ", minGreen: " << minGreen << ", minBlue: " << minBlue;
    ofLogNotice() << "maxRed: " << maxRed << ", maxGreen: " << maxGreen << ", maxBlue: " << maxBlue;

    resetCoordinates();
}

void ofApp::changeBackground(bool selectedColor) {
    if (selectedColor) {
        background = circleColor;
    } else {
        background == ofColor(255,255,255) ? background = ofColor(0,0,0) : background = ofColor(255,255,255);
    }

}
