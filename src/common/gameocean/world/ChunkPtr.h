#pragma once

#include <iostream>

class Chunk;

class ChunkPtr {
protected:
    Chunk* chunk;

    void setChunk(Chunk *chunk);
    void releaseChunk();
public:
    ChunkPtr() : chunk(nullptr) {
    }
    ChunkPtr(Chunk *chunk)  {
        setChunk(chunk);
    }
    ChunkPtr(const ChunkPtr &org) : ChunkPtr(org.chunk) {
        //
    }
    ChunkPtr(ChunkPtr &&org) {
        this->chunk = org.chunk;
        org.chunk = nullptr;
    }
    ~ChunkPtr() {
        releaseChunk();
    }
    Chunk *operator->() {
        return chunk;
    }
    Chunk &operator*() {
        return *chunk;
    }
    operator bool() {
        return (chunk != nullptr);
    }
    bool operator==(const ChunkPtr &sec) {
        return sec.chunk == chunk;
    }
    bool operator!=(const ChunkPtr &sec) {
        return sec.chunk != chunk;
    }
    ChunkPtr &operator=(const ChunkPtr &sec) {
        if (sec.chunk != chunk) {
            releaseChunk();
            setChunk(sec.chunk);
        }
    }
};