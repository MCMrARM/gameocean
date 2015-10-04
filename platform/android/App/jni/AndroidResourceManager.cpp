#include "AndroidResourceManager.h"
#include "common.h"
#include "utils/BinaryStream.h"
#include <iostream>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>

class AndroidResourceBuf : public std::streambuf {
	AAsset* asset;
	std::vector<char> buffer_;
	const std::size_t put_back_;
public:
	AndroidResourceBuf(AAsset* asset) : asset(asset),
		put_back_(8), buffer_(256 + 8) {
		char *end = &buffer_.front() + buffer_.size();
    		setg(end, end, end);
	}
	~AndroidResourceBuf() {
		AAsset_close(asset);
	}
	int underflow() {
		if (gptr() < egptr())
			return traits_type::to_int_type(*gptr());
		char *base = &buffer_.front();
		char *start = base;
		if (eback() == base) {
			std::memmove(base, egptr() - put_back_, put_back_);
			start += put_back_;
		}
		size_t n = AAsset_read(asset, start, buffer_.size() - (start - base));
		if (n <= 0)
			return traits_type::eof();
		setg(base, start, start + n);
		return traits_type::to_int_type(*gptr());
	}
};
class AndroidResourceStream : public std::istream {
public:
	AndroidResourceBuf streamBuf;

	AndroidResourceStream(AAsset* asset) : streamBuf(asset), std::istream(&streamBuf) {
	}
};
AndroidResourceManager::AndroidResourceManager(android_app* androidApp) : FileResourceManager("", androidApp->activity->internalDataPath), androidApp(androidApp) {
	//
}
std::unique_ptr<std::istream> AndroidResourceManager::openAssetFile(std::string name, std::ios_base::openmode mode) {
    AAssetManager* assets = androidApp->activity->assetManager;
    AAsset* asset = AAssetManager_open(assets, name.c_str(), AASSET_MODE_UNKNOWN);
    if(asset == null) {
        Logger::main->error("AndroidApp", "Couldn't read asset: %s", name.c_str());
        return std::unique_ptr<std::istream>();
    }
    Logger::main->trace("AndroidApp", "Reading asset: %s", name.c_str());

    return std::unique_ptr<std::istream>(new AndroidResourceStream(asset));
}

class AndroidAssetBinaryStream : public BinaryStream {
private:
	AAsset* asset;
public:
	AndroidAssetBinaryStream(AAsset* asset) : asset(asset) {}
	virtual void read(byte *data, unsigned int size) {
		size_t n = AAsset_read(asset, &data[0], size);
		if (n <= 0)
			throw new EOFException();
	}
	virtual void write(const byte *data, unsigned int size) { }
};

std::unique_ptr<BinaryStream> AndroidResourceManager::openBinaryAssetFile(std::string name) {
    AAssetManager* assets = androidApp->activity->assetManager;
    AAsset* asset = AAssetManager_open(assets, name.c_str(), AASSET_MODE_UNKNOWN);
    if(asset == null) {
        Logger::main->error("AndroidApp", "Couldn't read asset: %s", name.c_str());
        return std::unique_ptr<BinaryStream>();
    }
    Logger::main->trace("AndroidApp", "Reading asset as binary: %s", name.c_str());

    return std::unique_ptr<BinaryStream>(new AndroidAssetBinaryStream(asset));
}
