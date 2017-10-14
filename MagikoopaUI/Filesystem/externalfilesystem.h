#ifndef EXTERNALFILESYSTEM_H
#define EXTERNALFILESYSTEM_H

class ExternalFilesystem : public FilesystemBase
{
public:
    ExternalFilesystem(QString path, Endianess endianess = LittleEndian);
    ~ExternalFilesystem();

    bool directoryExists(QString path);
    void directoryContents(QString path, Filter filter, QList<QString>& out);

    bool fileExists(QString path);
    FileBase* openFile(QString path);
    bool save(FileBase* file);
    bool save();

    bool copy(QString path, QString newPath);

    bool rename(QString oldPath, QString newPath);
    bool remove(QString path);


private:
    QString basepath;

    void copyRecursive(QString path, QString newPath);
};

#endif // EXTERNALFILESYSTEM_H
