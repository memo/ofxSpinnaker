#include "ofMain.h"
#include "ofxSpinnaker.h"
#include "ofxMachineVision.h"
#include "ofxGui.h"


// I don't like this, but couldnt' think of another way
class DeviceParams {
public:

	// contains device parameters as ofParameters
	ofParameterGroup pGroup;

	// given the name of a device parameter, returns the corresponding ofxMachineVision::AbstractParameter
	map<string, shared_ptr<ofxMachineVision::AbstractParameter>> mvpMap;

	// add a device parameter to both the ofParameterGroup and the ofxMachineVision::AbstractParameter map
	void add(shared_ptr<ofxMachineVision::AbstractParameter> deviceMVParam) {
		ofAbstractParameter& deviceParam = deviceMVParam->getParameter();
		ofLogVerbose() << "Device parameter : " << deviceParam.getName() << " " << deviceParam.type() << " " << deviceParam.valueType();
		auto key = deviceParam.getName();
		if (mvpMap.count(key) > 0) {
			ofLogError() << deviceParam.getName() << " already exists.";
		}
		else {
			pGroup.add(deviceParam); // add to ofParameterGroup for display in GUI
			mvpMap[key] = deviceMVParam; // save to map for inverse retrieval
		}
	}

	// setup a listener to sync changes in the ofParameterGroup to the device
	// TODO, otherway sync?
	void startListening() {
		ofAddListener(pGroup.parameterChangedE(), this, &DeviceParams::onParameterChanged);
	}

private:
	//--------------------------------------------------------------
	void onParameterChanged(ofAbstractParameter& e) {
		mvpMap[e.getName()]->syncToDevice();
	}
};


class ofApp : public ofBaseApp {

	ofxMachineVision::SimpleGrabber<ofxMachineVision::Device::Spinnaker> grabber;
	
	// Add device parameters to a gui
	ofxPanel gui;
	ofXml settingsXml;

	DeviceParams deviceParams;


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
		ofSetLogLevel(OF_LOG_VERBOSE);

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
		auto deviceMVParamsList = grabber.getDeviceParameters();
		for (auto&& deviceMVParam : deviceMVParamsList) deviceParams.add(deviceMVParam);
		deviceParams.startListening(); // important! sets listener to sync changes in gui to the device

		gui.setup(deviceParams.pGroup);
		gui.loadFromFile("settings.xml");

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
		gui.draw();
	}

	//--------------------------------------------------------------
	void keyPressed(int key) {
		if (key == 's') {
			ofSerialize(settingsXml, deviceParams.pGroup);
			settingsXml.save("settings.xml");
		}
		if (key == 'l') {
			settingsXml.load("settings.xml");
			ofDeserialize(settingsXml, deviceParams.pGroup);
		}
	}
};


//========================================================================
int main() {
	ofSetupOpenGL(1024, 768, OF_WINDOW);
	ofRunApp(new ofApp());

}
