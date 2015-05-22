#pragma once

#include "App.h"
#include "lbclientViewController.h"

class iOSApp : public App {
    
    lbclientViewController* controller;
    
public:
    iOSApp(lbclientViewController* controller);
    
    virtual MultiLogger* initLogger();
    virtual void destroyOpenGL();
    virtual void showKeyboard(std::string text);
    virtual void hideKeyboard();
    virtual byte* readGameFile(std::string name, unsigned int& size);
    
};
