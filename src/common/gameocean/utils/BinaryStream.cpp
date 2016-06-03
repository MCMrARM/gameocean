#include "BinaryStream.h"

#include <unistd.h>

MemoryBinaryStream::MemoryBinaryStream(byte *data, int size) {
    this->data = data;
    this->size = size;
}

void MemoryBinaryStream::write(const byte *data, unsigned int size) {
    if (pos + size > this->size) {
        if (allowRealloc) {
            resize(pos + size);
        } else {
            Logger::main->error("MemoryBinaryStream",
                                "Attempting to write %i bytes to a buffer with only %i bytes remaining!", size,
                                this->size - pos);
            abort();
            throw EOFException();
        }
    }

    memcpy(&this->data[pos], data, size);
    pos += size;
}

unsigned int MemoryBinaryStream::read(byte *data, unsigned int size) {
    if (pos + size > this->size) {
        size = (this->size - pos);
    }

    memcpy(data, &this->data[pos], size);
    pos += size;
    return size;
}

DynamicMemoryBinaryStream::DynamicMemoryBinaryStream(int size) : MemoryBinaryStream(new byte[size], size) {
    allowRealloc = true;
}

void DynamicMemoryBinaryStream::resize(unsigned int minimalSize) {
    unsigned int newSize = size;
    while (newSize < minimalSize) {
        newSize *= 2;
    }
    if (newSize == size)
        return;
    byte* newBuf = new byte[newSize];
    memcpy(&newBuf[0], &data[0], size);
    delete data;
    data = newBuf;
    size = newSize;
}

byte* DynamicMemoryBinaryStream::getBuffer(bool release) {
    byte* b = data;
    if (release)
        data = nullptr;
    return b;
}

FileBinaryStream::FileBinaryStream(int fd) {
    this->fd = fd;
}

void FileBinaryStream::write(const byte *data, unsigned int size) {
    int pos = 0;
    while(true) {
        ssize_t s = ::write(fd, &data[pos], size - pos);
        if (s < 0) {
            throw EOFException();
        }
        pos += s;
        if (pos >= size) {
            return;
        }
    }
}

unsigned int FileBinaryStream::read(byte *data, unsigned int size) {
    unsigned int pos = 0;
    while(true) {
        ssize_t s = ::read(fd, &data[pos], size - pos);
        if (s <= 0) {
            return pos;
        }
        pos += s;
        if (pos >= size) {
            return pos;
        }
    }
}

FileBinaryStream::~FileBinaryStream() {
    if (closeFdOnDestroy) {
        ::close(fd);
    }
}