#pragma once

#include <cstring>
#include "../common.h"
#include <string>
#include <memory>
#include <exception>

class EOFException : public std::exception {

public:
    virtual const char *what() const throw() {
        return "End of stream reached";
    }

};

/**
 * This class allows reading and writing binary data.
 */
class BinaryStream {

public:
    bool swapEndian = false;

    virtual ~BinaryStream() { };

    virtual unsigned int read(byte *data, unsigned int size) = 0;

    virtual void write(const byte *data, unsigned int size) = 0;

    inline void readFully(byte *data, unsigned int size) {
        if (read(data, size) != size)
            throw EOFException ();
    }

    static const int LONG_SIZE = 8;
    static const int INT_SIZE = 4;
    static const int SHORT_SIZE = 2;
    static const int BYTE_SIZE = 1;
    static const int FLOAT_SIZE = 4;
    static const int DOUBLE_SIZE = 8;

    static inline unsigned int getStringSize(const std::string &str) { return (unsigned int) (INT_SIZE + str.length()); };

    BinaryStream &operator<<(float val) {
        if(swapEndian) swapBytes((byte*) &val, FLOAT_SIZE);
        this->write((byte *) &val, FLOAT_SIZE);
        return *this;
    }

    BinaryStream &operator<<(double val) {
        if(swapEndian) swapBytes((byte*) &val, DOUBLE_SIZE);
        this->write((byte *) &val, DOUBLE_SIZE);
        return *this;
    }

    BinaryStream &operator<<(long long val) {
        if(swapEndian) swapBytes((byte*) &val, LONG_SIZE);
        this->write((byte *) &val, LONG_SIZE);
        return *this;
    }

    BinaryStream &operator<<(int val) {
        if(swapEndian) swapBytes((byte*) &val, INT_SIZE);
        this->write((byte *) &val, INT_SIZE);
        return *this;
    }

    BinaryStream &operator<<(short val) {
        if(swapEndian) swapBytes((byte*) &val, SHORT_SIZE);
        this->write((byte *) &val, SHORT_SIZE);
        return *this;
    }

    BinaryStream &operator<<(char val) {
        this->write((byte *) &val, BYTE_SIZE);
        return *this;
    }

    BinaryStream &operator<<(std::string val) {
        *this << (int) val.length();
        this->write((const byte *) val.c_str(), (unsigned int) val.length());
        return *this;
    }

private:
    byte _buf[LONG_SIZE];

public:
    BinaryStream &operator>>(float &val) {
        this->read(_buf, FLOAT_SIZE);
        if(swapEndian) swapBytes(_buf, FLOAT_SIZE);
        val = ((float *) _buf)[0];
        return *this;
    }

    BinaryStream &operator>>(double &val) {
        this->read(_buf, DOUBLE_SIZE);
        if(swapEndian) swapBytes(_buf, DOUBLE_SIZE);
        val = ((double *) _buf)[0];
        return *this;
    }

    BinaryStream &operator>>(long long &val) {
        this->read(_buf, LONG_SIZE);
        if(swapEndian) swapBytes(_buf, LONG_SIZE);
        val = ((int64_t *) _buf)[0];
        return *this;
    }

    BinaryStream &operator>>(unsigned long long &val) {
        *this >> (long long&) val;
        return *this;
    }

    BinaryStream &operator>>(int &val) {
        this->read(_buf, INT_SIZE);
        if(swapEndian) swapBytes(_buf, INT_SIZE);
        val = ((int32_t *) _buf)[0];
        return *this;
    }

    BinaryStream &operator>>(unsigned int &val) {
        *this >> (int&) val;
        return *this;
    }

    BinaryStream &operator>>(short &val) {
        this->read(_buf, SHORT_SIZE);
        if(swapEndian) swapBytes(_buf, SHORT_SIZE);
        val = ((int16_t *) _buf)[0];
        return *this;
    }

    BinaryStream &operator>>(unsigned short &val) {
        *this >> (short&) val;
        return *this;
    }

    BinaryStream &operator>>(char &val) {
        this->read(_buf, BYTE_SIZE);
        val = ((int8_t *) _buf)[0];
        return *this;
    }

    BinaryStream &operator>>(unsigned char &val) {
        *this >> (char&) val;
        return *this;
    }

    BinaryStream &operator>>(std::string &val) {
        unsigned int strLen;
        *this >> strLen;
        byte buf[strLen];
        this->read(&buf[0], strLen);
        val = std::string((char *) buf, strLen);
        return *this;
    }

    static void swapBytes(byte* array, int size) {
        byte* array2 = new byte[size];
        memcpy(array2, array, size);
        for (int i = size - 1; i >= 0; i--) {
            array[size - 1 - i] = array2[i];
        }
    }

};

class MemoryBinaryStream : public BinaryStream {

protected:
    unsigned int size;
    byte *data;
    unsigned int pos = 0;
    bool allowRealloc = false;

    virtual void resize(unsigned int minimalSize) { };

public:
    MemoryBinaryStream(byte *data, int size);

    /**
     * This function returns the current used buffer size when writing or the current buffer position when reading.
     */
    inline int getSize() { return pos; };

    /**
     * This function returns the actual size of the buffer.
     */
    inline int getBufferSize() { return size; };

    virtual void write(const byte *data, unsigned int size);

    virtual unsigned int read(byte *data, unsigned int size);

};

class DynamicMemoryBinaryStream : public MemoryBinaryStream {

protected:
    virtual void resize(unsigned int minimalSize);

public:
    DynamicMemoryBinaryStream(int size);
    DynamicMemoryBinaryStream() : DynamicMemoryBinaryStream(1024) {
        //
    }

    virtual ~DynamicMemoryBinaryStream() {
        if (data != null)
            delete data;
    }

    /**
     * Pass 'true' if you want to avoid freeing the buffer but are not going to use this object later (trying to read
     * or write on it will result in a crash!).
     */
    byte* getBuffer(bool release);

};

class FileBinaryStream : public BinaryStream {

    int fd = -1;
    bool closeFdOnDestroy = false;

public:
    FileBinaryStream() {};
    FileBinaryStream(int fd);
    FileBinaryStream(int fd, bool closeFdOnDestroy) : FileBinaryStream(fd) {
        this->closeFdOnDestroy = closeFdOnDestroy;
    };
    virtual ~FileBinaryStream();

    void setFileDescriptor(int fd) { this->fd = fd; };

    virtual void write(const byte *data, unsigned int size);

    virtual unsigned int read(byte *data, unsigned int size);

};

class WrapperBinaryStream : public BinaryStream {

protected:
    std::unique_ptr<BinaryStream> stream;

public:
    WrapperBinaryStream(std::unique_ptr<BinaryStream> stream) : stream(std::move(stream)) {
        //
    };

    virtual void write(const byte *data, unsigned int size) {
        stream->write(data, size);
    };

    virtual unsigned int read(byte *data, unsigned int size) {
        stream->read(data, size);
    };

};

/*
class BufferedBinaryStream : public WrapperBinaryStream {

protected:
    static const unsigned int CHUNK_SIZE = 1024 * 16;
    byte readData [CHUNK_SIZE];
    unsigned int readPos = 0;
    byte writeData [CHUNK_SIZE];
    unsigned int writePos = 0;

public:
    BufferedBinaryStream(std::unique_ptr<BinaryStream> stream) : WrapperBinaryStream(std::move(stream)) {
        //
    };

    virtual void write(const byte *data, unsigned int size) {
        if (size >= CHUNK_SIZE) {
            stream->write(data, size);
            return;
        }
        else if (size >= CHUNK_SIZE - writePos)
            flush();
        memcpy(&writeData[writePos], &data[0], size);
    };

    virtual unsigned int read(byte *data, unsigned int size) {
        int destOff = 0;
        if (readPos <= 0) {
            stream->read(&readData[0], size);
        }

        if (size < CHUNK_SIZE - readPos) {
            memcpy(&data[0], &readData[0], size);
            readPos += size;
            return;
        }
        memcpy(&data[0], &readData[0], CHUNK_SIZE - readPos);
        destOff += CHUNK_SIZE - readPos;
        readPos = 0;

        stream->read(&data[destOff], size - destOff);
    };

    virtual void flush() {
        if (writePos <= 0)
            return;
        stream->write(&writeData[0], writePos);
        writePos = 0;
    };

};
 */