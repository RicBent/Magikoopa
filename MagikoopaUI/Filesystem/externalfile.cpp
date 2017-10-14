#include "filesystem.h"

#include <QTemporaryFile>

ExternalFile::ExternalFile(FileContainer* parent, QString path)
{
    this->parent = parent;
    file = new QFile(path);

    openCount = 0;
    endianess = LittleEndian;
}

ExternalFile::ExternalFile(FileContainer* parent)
{
    this->parent = parent;
    file = new QTemporaryFile();

    openCount = 0;
    endianess = LittleEndian;
}

ExternalFile::ExternalFile(QString path)
{
    this->parent = NULL;
    file = new QFile(path);

    openCount = 0;
    endianess = LittleEndian;
}

ExternalFile::~ExternalFile()
{
    delete file;
}

void ExternalFile::open()
{
    if (openCount == 0)
        file->open(QIODevice::ReadWrite);

    openCount++;
}

void ExternalFile::close()
{
    openCount--;

    if (openCount == 0)
        file->close();

    if (openCount < 0)
        throw std::logic_error("MemoryFile: openCount < 0");
}

void ExternalFile::save()
{
    file->flush();
    if (parent)
        parent->save(this);
}

quint64 ExternalFile::readData(quint8* data, quint64 len)
{
    return file->read((char*)data, len);
}

quint64 ExternalFile::writeData(quint8* data, quint64 len)
{
    return file->write((const char*)data, len);
}

quint64 ExternalFile::pos()
{
    return file->pos();
}

bool ExternalFile::seek(quint64 pos)
{
    return file->seek(pos);
}

quint64 ExternalFile::size()
{
    return file->size();
}

bool ExternalFile::resize(quint64 size)
{
    return file->resize(size);
}
