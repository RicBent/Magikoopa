#ifndef MEMORYFILE_H
#define MEMORYFILE_H

class MemoryFile : public FileBase
{
public:
    MemoryFile(FilesystemBase* parent, quint8* blob, quint32 size);
    MemoryFile(FileContainer* parent, quint32 size = 0);
    ~MemoryFile();

    void open();
    void close();
    void save();

    quint64 readData(quint8* data, quint64 len);
    quint64 writeData(quint8* data, quint64 len);
    quint64 pos();
    bool seek(quint64 pos);

    quint64 size();
    bool resize(quint64 size);

private:
    quint8* data;
    quint32 _size;
    quint32 _pos;
};

#endif // MEMORYFILE_H
