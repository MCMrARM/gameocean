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
        throw EOFException();
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
    return pos;
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