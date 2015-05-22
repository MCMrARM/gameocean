#include "iOSApp.h"

#include "App.h"

#include "iOSLogger.h"

iOSApp::iOSApp(lbclientViewController* controller) : App() {
    this->controller = controller;
}

MultiLogger* iOSApp::initLogger() {
    MultiLogger* logger = App::initLogger();
	logger->addLogger(new iOSLogger());
	return logger;
}

void iOSApp::destroyOpenGL() {
    //
}

void iOSApp::showKeyboard(std::string text) {
    [controller.kb setText:text];
    [controller.kb becomeFirstResponder];
}

void iOSApp::hideKeyboard() {
    [controller.kb resignFirstResponder];
}

byte* iOSApp::readGameFile(std::string name, unsigned int &size) {
    size_t iof = name.find_last_of("/");
    if(iof != std::string::npos) {
        name = name.substr(iof + 1);
    }
    iof = name.find_last_of(".");
    std::string file = name.substr(0, iof);
    std::string type = name.substr(iof + 1);
    
    NSString* path = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:file.c_str()] ofType:[NSString stringWithUTF8String:type.c_str()]];
    Logger::main->trace("iOSApp", "Loading resource: %s, type: %s; path: %s", file.c_str(), type.c_str(), path.UTF8String);
    
    NSData* nsdata = [NSData dataWithContentsOfFile:path];
    size = [nsdata length];
    byte* data = new byte[size];
    [nsdata getBytes:data];
    return data;
}