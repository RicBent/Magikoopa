#include "filesystem.h"

#include <QDebug>

Yaz0File::Yaz0File(FileBase* file)
{
    this->yaz0file = file;
}

Yaz0File::~Yaz0File()
{
    delete yaz0file;
}

FileBase* Yaz0File::getFile()
{
    yaz0file->open();
    yaz0file->seek(0);

    QString magic;
    yaz0file->readStringASCII(magic, 4);
    if (magic != "Yaz0")
        throw new std::runtime_error("Yaz0: Invalid tag");

    quint32 decompSize = yaz0file->read32();
    yaz0file->skip(8);

    FileBase* ret;
    if (decompSize >= 64*1024*1024)
        ret = new ExternalFile(this);
    else
        ret = new MemoryFile(this, decompSize);

    ret->setEndianess(yaz0file->getEndianess());

    ret->open();
    ret->resize(decompSize);
    ret->seek(0);

    clock_t t = clock();

    quint8 header = 0;
    quint8 header_bits = 0;
    while (ret->pos() < decompSize)
    {
        if (header_bits == 0)
        {
            header = yaz0file->read8();
            header_bits = 8;
        }

        if (header & 0x80)
        {
            ret->write8(yaz0file->read8());
        }
        else
        {
            quint8 data1 = yaz0file->read8();
            quint8 data2 = yaz0file->read8();

            quint32 length = data1 >> 4;
            if (length == 0) length = yaz0file->read8() + 0x12;
            else length += 2;

            quint32 offset = ((data1 & 0x0F) << 8 | data2) + 1;

            quint32 runningsize = ret->pos();

            ret->seek(runningsize - offset);

            qint32 overflow = -(qint32)offset + length;
            quint8* tempBuf;
            if (overflow < 0)
            {
                tempBuf = new quint8[length];
                ret->readData(tempBuf, length);
                ret->seek(runningsize);
                ret->writeData(tempBuf, length);
            }
            else
            {
                // Overflow into not yet written data
                quint32 readlength = length - overflow;
                tempBuf = new quint8[readlength];
                ret->readData(tempBuf, readlength);
                ret->seek(runningsize);
                ret->writeData(tempBuf, readlength);
                while ((unsigned)overflow >= (readlength))
                {
                    ret->writeData(tempBuf, readlength);
                    overflow -= readlength;
                }
                ret->writeData(tempBuf, overflow);
            }
            delete[] tempBuf;
        }

        header <<= 1;
        header_bits--;
    }

    t = clock() - t;
    printf ("Decompression finished: %lu ticks (%f seconds)\n",t , ((float)t)/CLOCKS_PER_SEC);

    ret->close();
    yaz0file->close();

    return ret;
}

bool Yaz0File::save(FileBase* file)
{
    if (file->size() > 0xFFFFFFFF)
        return false;

    yaz0file->open();
    yaz0file->seek(0);
    file->open();
    file->seek(0);

    time_t t = clock();

    if (true) //compressionLevel == 0)
    {
        qWarning("Yaz0: Fake comression: Not tested.");

        // Fake Yaz0
        quint32 fileSize = 16;
        fileSize += (file->size() + 7) / 8;
        fileSize += file->size();
        yaz0file->resize(fileSize);

        // Header
        yaz0file->writeStringASCII("Yaz0", 4);
        yaz0file->write32((quint32)file->size());
        yaz0file->write64(0);

        // Data
        for (quint32 i = 0; i < (file->size() / 8); i++)
        {
            yaz0file->write8(0xFF);
            yaz0file->write64(file->read64());
        }
        if (file->pos() < file->size())
        {
            yaz0file->write8(0xFF);
            while (file->pos() < file->size())
                yaz0file->write8(file->read8());
        }

    }

    t = clock() - t;
    printf ("Compression finished: %lu ticks (%f seconds)\n",t , ((float)t)/CLOCKS_PER_SEC);

    file->close();
    yaz0file->save();
    yaz0file->close();

    return true;
}
