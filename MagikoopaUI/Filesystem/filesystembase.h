#ifndef FILESYSTEMBASE_H
#define FILESYSTEMBASE_H

class FilesystemBase : public FileContainer
{
public:
    virtual ~FilesystemBase() {}

    enum Filter
    {
        Dirs = 0x01,
        Files = 0x02,
        DirsFiles = 0x03
    };

    virtual bool directoryExists(QString path) = 0;
    virtual void directoryContents(QString path, Filter filter, QList<QString>& out) = 0;

    virtual bool fileExists(QString path) = 0;
    virtual FileBase* openFile(QString path) = 0;
    virtual bool save() = 0;    // Used for manual FS saving

    virtual bool copy(QString path, QString newPath) = 0;

    virtual bool rename(QString oldPath, QString newPath) = 0;
    virtual bool remove(QString path) = 0;

    Endianess getEndianess()
    {
        return endianess;
    }

    void setEndianess(Endianess endianess)
    {
        this->setEndianess(endianess);
    }

    FilesystemMode getMode()
    {
        return mode;
    }


protected:
    Endianess endianess;
    FilesystemMode mode;
};

#endif // FILESYSTEMBASE_H
