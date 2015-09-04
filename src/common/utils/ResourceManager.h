#pragma once

#include "common.h"
#include "BinaryStream.h"
#include <fstream>
#include <vector>
#include <memory>

class ResourceManager {

public:
    static ResourceManager* instance;

    struct PNGInfo {
        byte* data;
        unsigned long width, height, dataSize;

        bool init(std::vector<byte>& pngData);
        bool init(std::istream& dataStream);
    };

    virtual std::unique_ptr<std::istream> openAssetFile(std::string name, std::ios_base::openmode mode) = 0;
    virtual std::unique_ptr<BinaryStream> openBinaryAssetFile(std::string name) = 0;
    virtual std::vector<byte> readAssetFile(std::string name)  {
        std::unique_ptr<std::istream> s = openAssetFile(name, std::ios_base::in | std::ios::binary);
        std::vector<byte> val (std::istreambuf_iterator<char>(*s), (std::istreambuf_iterator<char>()));
        return val;
    };
    virtual std::string readAssetTextFile(std::string name) {
        std::unique_ptr<std::istream> s = openAssetFile(name, std::ios_base::in);
        std::string val (std::istreambuf_iterator<char>(*s), (std::istreambuf_iterator<char>()));
        return val;
    };
    virtual PNGInfo readAssetImageFile(std::string name) {
        PNGInfo ret;
        std::unique_ptr<std::istream> s = openAssetFile(name, std::ios_base::out | std::ios_base::binary);
        ret.init(*s);
        return ret;
    };

    virtual std::unique_ptr<std::iostream> openDataFile(std::string name, std::ios_base::openmode mode) = 0;
    virtual std::unique_ptr<BinaryStream> openBinaryDataFile(std::string name) = 0;
    virtual std::vector<byte> readDataFile(std::string name)  {
        std::unique_ptr<std::iostream> s = openDataFile(name, std::ios_base::in | std::ios::binary);
        std::vector<byte> val (std::istreambuf_iterator<char>(*s), (std::istreambuf_iterator<char>()));
        return val;
    };
    virtual void writeDataTextFile(std::string name, std::string& val) {
        std::unique_ptr<std::iostream> s = std::move(openDataFile(name, std::ios_base::out));
        *s << val;
    };
    virtual std::string readDataTextFile(std::string name) {
        std::unique_ptr<std::iostream> s = std::move(openDataFile(name, std::ios_base::in));
        std::string val (std::istreambuf_iterator<char>(*s), (std::istreambuf_iterator<char>()));
        return val;
    };
    virtual PNGInfo readDataImageFile(std::string name) {
        PNGInfo ret;
        std::unique_ptr<std::istream> s = openDataFile(name, std::ios_base::out | std::ios_base::binary);
        ret.init(*s);
        return ret;
    };
    //virtual void downloadFile(std::string name);

};


#include <fcntl.h>

class FileResourceManager : public ResourceManager {

protected:
    std::string assetPath, dataPath;

public:
    FileResourceManager(std::string assetPath, std::string dataPath) : assetPath(assetPath), dataPath(dataPath) { };

    virtual std::unique_ptr<std::istream> openAssetFile(std::string name, std::ios_base::openmode mode) {
        return std::unique_ptr<std::istream>(new std::fstream(assetPath + "/" + name, mode));
    };
    virtual std::unique_ptr<BinaryStream> openBinaryAssetFile(std::string name) {
        return std::unique_ptr<BinaryStream>(new FileBinaryStream(::open((assetPath + "/" + name).c_str(), O_RDONLY), true));
    };

    virtual std::unique_ptr<std::iostream> openDataFile(std::string name, std::ios_base::openmode mode) {
        return std::unique_ptr<std::iostream>(new std::fstream(dataPath + "/" + name, mode));
    };
    virtual std::unique_ptr<BinaryStream> openBinaryDataFile(std::string name) {
        return std::unique_ptr<BinaryStream>(new FileBinaryStream(::open((dataPath + "/" + name).c_str(), O_RDWR), true));
    };

};
