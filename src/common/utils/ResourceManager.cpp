#include "ResourceManager.h"

#include <sstream>
#include <png.h>
#include "membuf.h"

ResourceManager* ResourceManager::instance = null;

void readGameImageFile_callback(png_structp png_ptr, png_bytep out, png_size_t size) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if(!io_ptr) return;
    std::istream* dataStream = (std::istream*) io_ptr;
    dataStream->read((char*) out, size);
}

bool ResourceManager::PNGInfo::init(std::vector<byte>& pngData) {
    membuf sbuf((char*) &pngData[0], (char*) &pngData[pngData.size()]);
    std::istream dataStream (&sbuf);
    return init(dataStream);
}

bool ResourceManager::PNGInfo::init(std::istream& dataStream) {
    width = 0;
    height = 0;
    dataSize = -1;

    char sig [8];
    dataStream.read(&sig[0], 8);
    int i = png_sig_cmp((png_bytep) sig, 0, 8);
    if (i != 0) {
        Logger::main->warn("Image", "Not a valid png image [%i]", i);
        return false;
    }

    png_struct* png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, null, null, null);
    if(!png_ptr) return false;

    png_info* info_ptr = png_create_info_struct(png_ptr);

    png_set_read_fn(png_ptr, (void*) &dataStream, readGameImageFile_callback);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    int depth, colorType;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &depth, &colorType, null, null, null);

    if (colorType != PNG_COLOR_TYPE_RGBA) {
        Logger::main->warn("Image", "Color type (%i) is not RGBA", colorType);
    }

    png_size_t rowBytes = png_get_rowbytes(png_ptr, info_ptr);

    dataSize = rowBytes * height;
    png_byte* rows [height];
    data = std::unique_ptr<std::vector<byte>>(new std::vector<byte>(dataSize));

    for (int i = 0; i < height; i++) {
        rows[i] = &(*data)[(height - 1 - i) * rowBytes];
    }

    png_read_image(png_ptr, rows);

    png_destroy_read_struct(&png_ptr, &info_ptr, null);
    return true;
}

#include <dirent.h>

std::vector<ResourceManager::DirEntry> FileResourceManager::getAssetDirectoryFiles(std::string path) {
    std::vector<ResourceManager::DirEntry> ret;
    DIR* dir = opendir((assetPath + path).c_str());
    if (dir == null)
        return ret;

    dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] != '.')
            ret.push_back({ ent->d_name, (ent->d_type == DT_DIR) });
    }
    closedir(dir);
    return ret;
}

std::vector<ResourceManager::DirEntry> FileResourceManager::getDataDirectoryFiles(std::string path) {
    std::vector<ResourceManager::DirEntry> ret;
    DIR* dir = opendir((dataPath + path).c_str());
    if (dir == null)
        return ret;

    dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] != '.')
            ret.push_back({ ent->d_name, (ent->d_type == DT_DIR) });
    }
    closedir(dir);
    return ret;
}