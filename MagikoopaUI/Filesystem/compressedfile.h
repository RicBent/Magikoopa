#ifndef COMPRESSEDFILE_H
#define COMPRESSEDFILE_H

class CompressedFile : public FileContainer
{
public:
    virtual ~CompressedFile() {}

    virtual FileBase* getFile() = 0;

    void setCompressionLevel(quint8 compressionLevel)
    {
        this->compressionLevel = compressionLevel;
    }

    quint8 getCompressionLevel()
    {
        return compressionLevel;
    }

protected:
    quint8 compressionLevel;
};

#endif // COMPRESSEDFILE_H
