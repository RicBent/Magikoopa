#ifndef EXTERNALFILE_H
#define EXTERNALFILE_H

#include <QFile>

class ExternalFile : public FileBase
{
public:
    ExternalFile(FileContainer* parent, QString path);
    ExternalFile(FileContainer* parent);
    ExternalFile(QString path);
    ~ExternalFile();

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
    QFile* file;
};

#endif // EXTERNALFILE_H
