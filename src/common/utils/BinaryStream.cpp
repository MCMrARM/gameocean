#include "BinaryStream.h"

#include <unistd.h>

MemoryBinaryStream::MemoryBinaryStream(byte *data, int size) {
    this->data = data;
    this->size = size;
}

void MemoryBinaryStream::write(const byte *data, unsigned int size) {
    if (pos + size > this->size) {
        Logger::main->error("MemoryBinaryStream",
                            "Attempting to write %i bytes to a buffer with only %i bytes remaining!", size,
                            this->size - pos);
        throw new EOFException();
    }

    memcpy(&this->data[pos], data, size);
    pos += size;
}

void MemoryBinaryStream::read(byte *data, unsigned int size) {
    if (pos + size > this->size) {
        Logger::main->error("MemoryBinaryStream",
                            "Attempting to read %i bytes from a buffer with only %i bytes remaining!", size,
                            this->size - pos);
        throw new EOFException();
    }

    memcpy(data, &this->data[pos], size);
    pos += size;
}

FileBinaryStream::FileBinaryStream(int fd) {
    this->fd = fd;
}

void FileBinaryStream::write(const byte *data, unsigned int size) {
    int pos = 0;
    while(true) {
        ssize_t s = ::write(fd, &data[pos], size - pos);
        if (s < 0) {
            throw new EOFException();
        }
        pos += s;
        if (pos >= size) {
            return;
        }
    }
}

void FileBinaryStream::read(byte *data, unsigned int size) {
    int pos = 0;
    while(true) {
        ssize_t s = ::read(fd, &data[pos], size - pos);
        if (s < 0) {
            throw new EOFException();
        }
        pos += s;
        if (pos >= size) {
            return;
        }
    }
}