#pragma once

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <gameocean/common.h>
#include <gameocean/utils/BinaryStream.h>

class NBTTag {
private:
    static std::string getString(BinaryStream& stream);
    static std::unique_ptr<NBTTag> getTag(BinaryStream& stream, bool hasName, char dataType);

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

    virtual void write(BinaryStream& stream) = 0;

    static inline std::unique_ptr<NBTTag> getTag(BinaryStream& stream) { return getTag(stream, true, -1); };
    static void writeTag(BinaryStream& stream, NBTTag& tag, bool writeName) {
        stream << (char) tag.ID;
        if (writeName) {
            stream << (short) tag.name.size();
            stream.write((byte*) &tag.name[0], (int) tag.name.size());
        }
        tag.write(stream);
    }
};

class NBTEnd : public NBTTag {
public:
    NBTEnd() : NBTTag("", 0) {};

    virtual void write(BinaryStream& stream) {
        stream << (char) 0;
    }
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

    virtual void write(BinaryStream& stream) {
        stream << val;
    }

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

    virtual void write(BinaryStream& stream) {
        stream << val;
    }
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

    virtual void write(BinaryStream& stream) {
        stream << val;
    }
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

    virtual void write(BinaryStream& stream) {
        stream << val;
    }
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

    virtual void write(BinaryStream& stream) {
        stream << val;
    }
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

    virtual void write(BinaryStream& stream) {
        stream << val;
    }
};

class NBTByteArray : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << "byte[" << val.size() << "]" << std::endl;
    };

public:
    static const int ID = 7;

    std::vector<byte> val;

    NBTByteArray(std::string name) : NBTTag(name, ID) {};
    virtual std::string getTypeString() { return "byte[]"; };

    virtual void write(BinaryStream& stream) {
        stream << (int) val.size();
        stream.write(&val[0], (int) val.size());
    }
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

    virtual void write(BinaryStream& stream) {
        stream << (short) val.size();
        stream.write((byte*) &val[0], (int) val.size());
    }
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
        for (std::unique_ptr<NBTTag>& tag : val) {
            tag->print(prefix + "  ");
        }
    };

public:
    static const int ID = 9;

    std::vector<std::unique_ptr<NBTTag>> val;

    NBTList(std::string name) : NBTTag(name, ID) {};
    virtual std::string getTypeString() { return "list"; };

    virtual void write(BinaryStream& stream) {
        if (val.size() <= 0) {
            stream << (char) 0 << (int) 0;
            return;
        }
        stream << (char) val[0]->ID;
        stream << (int) val.size();
        for (std::unique_ptr<NBTTag> const& tag : val) {
            tag->write(stream);
        }
    }
};

class NBTCompound : public NBTTag {
protected:
    virtual void print(std::string prefix) {
        NBTTag::print(prefix);
        std::cout << "compound" << std::endl;
        for (auto const& e : val) {
            e.second->print(prefix + "  ");
        }
    };

public:
    static const int ID = 10;

    std::map<std::string, std::unique_ptr<NBTTag>> val;

    NBTCompound(std::string name) : NBTTag(name, ID) {};
    virtual std::string getTypeString() { return "compound"; };

    virtual void write(BinaryStream& stream) {
        for (auto const& e : val) {
            stream << (char) e.second->ID;
            stream << (short) e.first.size();
            stream.write((byte*) &e.first[0], (int) e.first.size());
            e.second->write(stream);
        }
        stream << (char) 0;
    }
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

    virtual void write(BinaryStream& stream) {
        stream << (int) val.size();
        for (int i : val) {
            stream << i;
        }
    }
};


