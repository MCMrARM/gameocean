#pragma once

#include "utils/ResourceManager.h"

class android_app;
class AndroidResourceManager : public FileResourceManager {
private:
	android_app* androidApp;

public:
	AndroidResourceManager(android_app* androidApp);

	virtual std::vector<DirEntry> getAssetDirectoryFiles(std::string path) {
		return {};
	}
	virtual std::unique_ptr<std::istream> openAssetFile(std::string name, std::ios_base::openmode mode);
	virtual std::unique_ptr<BinaryStream> openBinaryAssetFile(std::string name);
};
