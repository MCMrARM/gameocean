#pragma once

#include <string>
#include <sstream>

class Version {

public:
    enum class Channel {
        STABLE, BETA, DEV
    };

    int major;
    int minor;
    int release;
    int build = 0;
    int netVersion = 1;

    Channel channel = Channel::STABLE;

    Version() {};
    Version(int major, int minor, int release) : major(major), minor(minor), release(release) {};
    Version(int major, int minor, int release, int build) : major(major), minor(minor), release(release), build(build) {};

    void set(int major, int minor, int release, int build) {
        this->major = major;
        this->minor = minor;
        this->release = release;
        this->build = build;
    };

    std::string toString() {
        std::stringstream ss;
        ss << major << "." << minor << "." << release << "." << build;
        return ss.str();
    }

};


