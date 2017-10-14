#ifndef PLAINARCHIVE_H
#define PLAINARCHIVE_H

class PlainArchive : public FilesystemBase
{
public:
    PlainArchive();
    virtual ~PlainArchive() {}

    bool directoryExists(QString path);
    void directoryContents(QString path, Filter filter, QList<QString>& out);

    bool fileExists(QString path);
    FileBase* openFile(QString path);
    bool save(FileBase* file);
    bool save();

    bool copy(QString path, QString newPath);

    bool rename(QString oldPath, QString newPath);
    bool remove(QString path);

protected:

    struct InternalFile
    {
        QString name;
        quint32 offset, size;
        FileBase* newData;
    };

    virtual QHash<QString,InternalFile*>* files() = 0;
    virtual bool repack() = 0;

    FileBase* container;

    quint32 numFiles;
    quint32 dataOffset;
};

#endif // PLAINARCHIVE_H
