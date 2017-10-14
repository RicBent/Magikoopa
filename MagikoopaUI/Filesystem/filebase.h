#ifndef FILEBASE_H
#define FILEBASE_H

#include "filecontainer.h"

#include <QtEndian>

class FileBase
{
public:
    virtual ~FileBase() {}

    virtual void open() = 0;
    virtual void close() = 0;
    virtual void save() = 0;

    virtual quint64 readData(quint8* data, quint64 len) = 0;
    virtual quint64 writeData(quint8* data, quint64 len) = 0;
    virtual quint64 pos() = 0;
    virtual bool seek(quint64 pos) = 0;

    virtual quint64 size() = 0;
    virtual bool resize(quint64 size) = 0;

    quint8 read8()
    {
        quint8 ret;
        readData((quint8*)&ret, 1);
        return ret;
    }

    quint16 read16()
    {
        quint16 ret;
        readData((quint8*)&ret, 2);
        if (endianess == BigEndian) qbswap(ret, &ret);
        return ret;
    }

    quint32 read32()
    {
        quint32 ret;
        readData((quint8*)&ret, 4);
        if (endianess == BigEndian) qbswap(ret, &ret);
        return ret;
    }

    quint64 read64()
    {
        quint64 ret;
        readData((quint8*)&ret, 8);
        if (endianess == BigEndian) qbswap(ret, &ret);
        return ret;
    }

    float readFloat()
    {
        float ret;
        readData((quint8*)&ret, 4);
        if (endianess == BigEndian) qbswap(ret, &ret);
        return ret;
    }

    double readDouble()
    {
        double ret;
        readData((quint8*)&ret, 8);
        if (endianess == BigEndian) qbswap(ret, &ret);
        return ret;
    }

    quint64 readStringASCII(QString& str, quint32 len = 0)
    {
        str.clear();
        quint64 readlen = 0;

        if (len == 0)
        {
            while (true)
            {
                quint8 val = read8();
                readlen++;
                if (val == 0x00) break;
                else str += QChar(val);
            }
        }
        else
        {
            for (quint32 i = 0; i < len; i++)
            {
                str += QChar(read8());
                readlen++;
            }
        }

        return readlen;
    }

    void write8(quint8 val)
    {
        if (endianess == BigEndian) qbswap(val, &val);
        writeData((quint8*)&val, 1);
    }

    void write16(quint16 val)
    {
        if (endianess == BigEndian) qbswap(val, &val);
        writeData((quint8*)&val, 2);
    }

    void write32(quint32 val)
    {
        if (endianess == BigEndian) qbswap(val, &val);
        writeData((quint8*)&val, 4);
    }

    void write64(quint64 val)
    {
        if (endianess == BigEndian) qbswap(val, &val);
        writeData((quint8*)&val, 8);
    }

    void writeFloat(quint8 val)
    {
        if (endianess == BigEndian) qbswap(val, &val);
        writeData((quint8*)&val, 4);
    }

    void writeDouble(quint8 val)
    {
        if (endianess == BigEndian) qbswap(val, &val);
        writeData((quint8*)&val, 8);
    }

    void writeStringASCII(QString val, qint32 len = 0)
    {
        if (len == 0)
            len = val.length() + 1;

        if (len < val.length())
            val = val.left(len);

        for (int i = 0; i < val.length(); i++)
            write8(val.at(i).toLatin1());

        for (int i = 0; i < len - val.length(); i++)
            write8(0x00);
    }

    void skip(qint64 num)
    {
        seek(pos()+num);
    }

    FileBase* clone(FilesystemBase *container);
    FileBase* getSubfile(FilesystemBase* container, quint64 offset, quint64 size);

    void setIdPath(QString path)
    {
        if (path[0] == '/')
            path.remove(0, 1);

        if (idPath.isEmpty())
            idPath = path;
        else
            throw std::logic_error("FileBase: cannot set ID path twice!");
    }

    QString& getIdPath()
    {
        return idPath;
    }

    int& getOpenCount()
    {
        return openCount;
    }

    Endianess getEndianess()
    {
        return endianess;
    }

    void setEndianess(Endianess endianess)
    {
        this->endianess = endianess;
    }


protected:
    FileContainer* parent;
    QString idPath;

    qint32 openCount;

    Endianess endianess;
};

#endif // FILEBASE_H
