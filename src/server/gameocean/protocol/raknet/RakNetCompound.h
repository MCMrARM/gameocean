#pragma once

#include <vector>
#include <cstring>

class RakNetCompound {

public:
    struct Frame {
        bool received = false;
        std::vector<char> data;
    };

private:
    std::vector<Frame> frames;
    int remainingFrames;
    unsigned long totalFrameSize = 0;

public:
    RakNetCompound() {
        remainingFrames = 0;
    }
    RakNetCompound(unsigned int frameCount) {
        frames.resize(frameCount);
        remainingFrames = frameCount;
    }
    void addFrame(unsigned int frameId, std::vector<char> data) {
        if (frameId >= frames.size())
            return;
        if (!frames[frameId].received)
            remainingFrames--;
        else
            totalFrameSize -= frames[frameId].data.size();
        frames[frameId].data = std::move(data);
        frames[frameId].received = true;
        totalFrameSize += frames[frameId].data.size();
    }
    inline bool isComplete() const {
        return (remainingFrames == 0);
    }
    inline unsigned long getSizeSum() const {
        return totalFrameSize;
    }
    std::vector<char> build() const {
        std::vector<char> ret;
        ret.resize(totalFrameSize);
        unsigned long off = 0;
        for (auto it = frames.begin(); it != frames.end(); it++) {
            if (!it->received)
                return std::vector<char>();
            memcpy(&ret[off], it->data.data(), it->data.size());
            off += it->data.size();
        }
        return std::move(ret);
    }

};