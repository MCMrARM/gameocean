#pragma once

#include <string>

class Server {

public:
    std::string name;

    void start();

protected:
    void loadConfiguation();

};


