#pragma once

#include <vector>
#include <map>
#include <iostream>
#include "common.h"
#include "utils/BinaryStream.h"

class NBTTag {
private:
    static std::string getString(BinaryStream& stream);
    static NBTTag* getTag(BinaryStream& stream, bool hasName, char dataType);

protected:
    friend class NBTList;
    friend class NBTCompound;
    virtual void print(std::string prefix) {
        std::cout << prefix;
        if (name.length() > 0) {
            std::cout << name << ": ";
        }
    };

public:
    std::string name;
    const int ID;

    NBTTag(std::string name, int id) : name(name), ID(id) {};
    virtual ~NBTTag() {};

    inline void print() { print(""); };
    virtual std::string getTypeString() { return "unknown"; };

    static inline NBTTag* getTag(BinaryStream& stream) { return getTag(stream, true, -1); };
};

class NBTEnd : public NBTTag {
public:
    NBTEnd() : NBTTag("", 0) {};
};

class NBTByte : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << (int) val << std::endl;
    };

public:
    static const int ID = 1;

    char val;

    NBTByte(std::string name, char val) : NBTTag(name, ID), val(val) {};
    virtual std::string getTypeString() { return "byte"; };
};

class NBTShort : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << val << std::endl;
    };

public:
    static const int ID = 2;

    short val;

    NBTShort(std::string name, short val) : NBTTag(name, ID), val(val) {};
    virtual std::string getTypeString() { return "short"; };
};

class NBTInt : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << val << std::endl;
    };

public:
    static const int ID = 3;

    int val;

    NBTInt(std::string name, int val) : NBTTag(name, ID), val(val) {};
    virtual std::string getTypeString() { return "int"; };
};

class NBTLong : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << val << std::endl;
    };

public:
    static const int ID = 4;

    long long val;

    NBTLong(std::string name, long long val) : NBTTag(name, ID), val(val) {};
    virtual std::string getTypeString() { return "long"; };
};

class NBTFloat : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << val << std::endl;
    };

public:
    static const int ID = 5;

    float val;

    NBTFloat(std::string name, float val) : NBTTag(name, ID), val(val) {};
    virtual std::string getTypeString() { return "float"; };
};

class NBTDouble : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << val << std::endl;
    };

public:
    static const int ID = 6;

    double val;

    NBTDouble(std::string name, double val) : NBTTag(name, ID), val(val) {};
    virtual std::string getTypeString() { return "double"; };
};

class NBTByteArray : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << "byte[" << size << "]" << std::endl;
    };

public:
    static const int ID = 7;

    byte* val;
    unsigned int size;

    NBTByteArray(std::string name, byte* val, unsigned int size) : NBTTag(name, ID), val(val), size(size) {};
    virtual ~NBTByteArray() { if(val != null) delete val; };
    virtual std::string getTypeString() { return "byte[]"; };
};

class NBTString : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << '"' << val << '"' <<  std::endl;
    };

public:
    static const int ID = 8;

    std::string val;

    NBTString(std::string name, std::string val) : NBTTag(name, ID), val(val) {};
    virtual std::string getTypeString() { return "string"; };
};

class NBTList : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        if (val.size() <= 0) {
            std::cout << "empty list" << std::endl;
            return;
        }
        std::cout << val[0]->getTypeString() << "[]" << std::endl;
        for (NBTTag* tag : val) {
            tag->print(prefix + "  ");
        }
    };

public:
    static const int ID = 9;

    std::vector<NBTTag*> val;

    NBTList(std::string name, std::vector<NBTTag*> val) : NBTTag(name, ID), val(val) {};
    virtual ~NBTList() {
        for (NBTTag* tag : val) {
            delete tag;
        }
    };
    virtual std::string getTypeString() { return "list"; };
};

class NBTCompound : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << "compound" << std::endl;
        for (auto e : val) {
            e.second->print(prefix + "  ");
        }
    };

public:
    static const int ID = 10;

    std::map<std::string, NBTTag*> val;

    NBTCompound(std::string name, std::map<std::string, NBTTag*> val) : NBTTag(name, ID), val(val) {};
    virtual ~NBTCompound() {
        for (auto tag : val) {
            delete tag.second;
        }
    };
    virtual std::string getTypeString() { return "compound"; };
};

class NBTIntArray : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << "int[] {";
        bool f = true;
        for (int e : val) {
            if (!f) std::cout << ", ";
            else f = false;
            std::cout << e;
        }
        std::cout << "}" << std::endl;
    };

public:
    static const int ID = 11;

    std::vector<int> val;

    NBTIntArray(std::string name, std::vector<int> val) : NBTTag(name, ID), val(val) {};
    virtual std::string getTypeString() { return "int[]"; };
};


