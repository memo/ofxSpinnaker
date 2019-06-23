#include "ofMain.h"
#include "ofxSpinnaker.h"
#include "ofxMachineVision.h"
//#include "ofxGui.h"

class ofApp : public ofBaseApp {

	ofxMachineVision::SimpleGrabber<ofxMachineVision::Device::Spinnaker> grabber;
	//string specsStr;
	ofParameterGroup parameters;
	//ofxPanel gui;

	string getInfo(ofxMachineVision::SimpleGrabber<ofxMachineVision::Device::Spinnaker>& grabber) {
		auto specs = grabber.getDeviceSpecification();
		stringstream specsStream;
		specsStream
			<< "Manufacturer : " << specs.getManufacturer() << " \n"
			<< "Model : " << specs.getModelName() << " \n"
			<< "DeviceID : " << specs.getDeviceID() << " \n"
			<< "Serial : " << specs.getSerialNumber() << " \n"
			<< "Resolution : " << specs.getCaptureWidth() << "x" << specs.getCaptureHeight() << " \n"
			<< "Fps : " << grabber.getFps() << " \n";
		return specsStream.str();
	}

	//--------------------------------------------------------------
	void setup() {
		cout << "Listing devices..." << endl;
		auto deviceDescriptions = grabber.getDevice()->listDevices();
		for (auto&& deviceDescription : deviceDescriptions) {
			cout << deviceDescription.manufacturer << ", " << deviceDescription.model << endl;
		}

		cout << "Opening grabber..." << endl;
		grabber.open();

		string info = getInfo(grabber);
		cout << "Device Specifications:\n" << info << endl;
		ofSetWindowTitle(info);

		cout << "Parameters:" << endl;
		auto params = grabber.getDeviceParameters();
		for (auto&& param : params) {
			auto& p = param->getParameter();
			cout << p.getName() << " " << p.type() << " " << p.valueType() << endl;
		}

		cout << "Starting Capture..." << endl;
		grabber.startCapture();
	}

	//--------------------------------------------------------------
	void update() {
		grabber.update();
	}

	//--------------------------------------------------------------
	void draw() {
		grabber.draw(0, 0);
		ofDrawBitmapStringHighlight(ofToString(grabber.getFps()), 10, 10);
		ofSetWindowTitle(getInfo(grabber));
	}

	//--------------------------------------------------------------
	void keyPressed(int key) {

	}
};


//========================================================================
int main() {
	ofSetupOpenGL(1024, 768, OF_WINDOW);
	ofRunApp(new ofApp());

}
