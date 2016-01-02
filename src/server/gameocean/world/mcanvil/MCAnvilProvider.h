#pragma once

#include <string>
#include <mutex>
#include <unordered_map>
#include <memory>
#include <gameocean/world/WorldProvider.h>
#include <gameocean/world/ChunkPos.h>

typedef ChunkPos RegionPos;

struct MCAnvilHeader {
    std::string filePath;
    unsigned int locations [1024];
    unsigned int timestamps [1024];
};

class MCAnvilProvider : public ThreadedWorldProvider {

protected:
    std::mutex lock;
    std::unordered_map<RegionPos, std::shared_ptr<MCAnvilHeader>> regions;

    void loadRegion(RegionPos pos);

    virtual void loadChunk(ChunkPos pos);

public:
    MCAnvilProvider(World& world);

};
