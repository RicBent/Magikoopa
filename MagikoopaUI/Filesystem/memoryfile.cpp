#include "filesystem.h"

MemoryFile::MemoryFile(FilesystemBase* parent, quint8* blob, quint32 size)
{
    this->parent = parent;
    this->data = blob;
    this->_size = size;
    this->_pos = 0;

    openCount = 0;
}

MemoryFile::MemoryFile(FileContainer* parent, quint32 size)
{
    this->parent = parent;
    this->data = new quint8[size];
    this->_size = size;
    this->_pos = 0;

    openCount = 0;
}

MemoryFile::~MemoryFile()
{
    delete[] data;
}

void MemoryFile::open()
{
    openCount++;
}
void MemoryFile::close()
{
    openCount--;
    if (openCount < 0)
        throw std::logic_error("MemoryFile: openCount < 0");
}

void MemoryFile::save()
{
    if (parent)
        parent->save(this);
}

quint64 MemoryFile::readData(quint8* data, quint64 len)
{
    // prevent out-of-range read
    if ((_pos+len) > _size)
        len = _size-_pos;

    if (len < 1 || len > 0xFFFFFFFF)
        return 0;

    switch (len)
    {
    case 1: *data = this->data[_pos]; break;
    case 2: *(quint16*)data = *(quint16*)&this->data[_pos]; break;
    case 4: *(quint32*)data = *(quint32*)&this->data[_pos]; break;
    case 8: *(quint64*)data = *(quint64*)&this->data[_pos]; break;
    default: memcpy(data, &this->data[_pos], len); break;
    }

    _pos += len;
    return len;
}

quint64 MemoryFile::writeData(quint8* data, quint64 len)
{
    if (len < 1 || len > 0xFFFFFFFF)
        return 0;

    // resize the file if needed
    // (it is still more efficient to resize the file prior to writing)
    if ((_pos + len) > _size)
        resize(_pos + len);

    switch (len)
    {
    case 1: this->data[_pos] = *data; break;
    case 2: *(quint16*)&this->data[_pos] = *(quint16*)data; break;
    case 4: *(quint32*)&this->data[_pos] = *(quint32*)data; break;
    case 8: *(quint64*)&this->data[_pos] = *(quint64*)data; break;
    default: memcpy(&this->data[_pos], data, len); break;
    }

    _pos += len;
    return len;
}

quint64 MemoryFile::pos()
{
    return _pos;
}

bool MemoryFile::seek(quint64 pos)
{
    this->_pos = pos;
    return true;
}

quint64 MemoryFile::size()
{
    return _size;
}

bool MemoryFile::resize(quint64 size)
{
    if (size == this->_size)
        return true;

    if (size > 0xFFFFFFFF)
        return false;

    quint8* newdata = new quint8[size];
    if (size >= this->_size) memcpy(newdata, this->data, this->_size);
    else memcpy(newdata, this->data, size);
    delete[] this->data;
    this->data = newdata;
    this->_size = size;

    return true;
}
