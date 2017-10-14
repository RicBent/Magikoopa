#ifndef SARCFILESYSTEM_H
#define SARCFILESYSTEM_H

#include <QHash>

#define SARC_DATA_ALIGN 0x100

class SarcFilesystem : public FilesystemBase
{
public:
    SarcFilesystem(FileBase* file, FilesystemMode mode = AutoSave);
    ~SarcFilesystem();

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
    FileBase* sarcFile;

    struct InternalSarcFile
    {
        QString name;

        quint32 offset, size;

        quint32 nameOffset;
        quint32 nameHash;

        quint32 entryOffset;

        FileBase* newData;
    };

    quint32 numFiles;
    quint32 hashMult;

    quint32 sfatOffset;
    quint32 sfntOffset;
    quint32 dataOffset;


    bool repack();


    QHash<QString,InternalSarcFile*> files;


    quint32 filenameHash(QString& name);

    static bool hashSort(InternalSarcFile* f1, InternalSarcFile* f2)
    {
        return f1->nameHash < f2->nameHash;
    }

    quint32 align(quint32 val, quint32 aln)
    {
        return (val + aln - 1) / aln * aln;
    }

    void writeAlign(quint32 aln, quint8 val = 0x00)
    {
        quint32 padSize = align(sarcFile->pos(), aln) - sarcFile->pos();
        for (quint32 i = 0; i < padSize; i++) sarcFile->write8(val);
    }
};

#endif // SARCFILESYSTEM_H
