#include "filesystem.h"

#include <QDir>
#include <QFile>

ExternalFilesystem::ExternalFilesystem(QString path, Endianess endianess)
{
    this->endianess = endianess;

    if (!path.endsWith('/'))
        path += '/';

    basepath = path;
    mode = AutoSave;
}

ExternalFilesystem::~ExternalFilesystem()
{
}

bool ExternalFilesystem::directoryExists(QString path)
{
    return QDir(basepath + path).exists();
}

void ExternalFilesystem::directoryContents(QString path, Filter filter, QList<QString>& out)
{
    out.clear();

    QStringList res = QDir(basepath + path).entryList((QDir::Filter)filter | QDir::NoDotAndDotDot, QDir::Name|QDir::IgnoreCase|QDir::DirsFirst);
    for (int i = 0; i < res.size(); i++)
    {
        out.append(res[i]);
    }
}

bool ExternalFilesystem::fileExists(QString path)
{
    return QFile(basepath + path).exists();
}

FileBase* ExternalFilesystem::openFile(QString path)
{
    FileBase* ret = new ExternalFile(this, basepath + path);
    ret->setEndianess(this->endianess);
    return ret;
}

bool ExternalFilesystem::save(FileBase*)
{
    return true;
}

bool ExternalFilesystem::save()
{
    qWarning("ExternalFilesystem: save() called in automatic mode");
    return true;
}

bool ExternalFilesystem::copy(QString path, QString newPath)
{
    QString rnpath = basepath + path;
    newPath = basepath + newPath;
    QFileInfo info(rnpath);

    if (!info.exists())
        return false;
    if (info.isFile())
        return QFile(rnpath).copy(newPath);
    else if (info.isDir())
    {
        if (newPath.endsWith('/'))
            newPath.chop(1);
        copyRecursive(path, newPath);
        return true;
    }
    return false;
}

bool ExternalFilesystem::rename(QString oldPath, QString newPath)
{
    QString rnpath = basepath + oldPath;
    newPath = basepath + newPath;
    QFileInfo info(rnpath);

    if (!info.exists())
        return false;
    if (info.isFile())
        return QFile(rnpath).rename(newPath);
    else if (info.isDir())
        return QDir(rnpath).rename(rnpath, newPath);
    return false;
}

bool ExternalFilesystem::remove(QString path)
{
    QString rmpath = basepath + path;
    QFileInfo info(rmpath);

    if (!info.exists())
        return false;
    if (info.isFile())
        return QFile(rmpath).remove();
    else if (info.isDir())
        return QDir(rmpath).removeRecursively();
    return false;

}

void ExternalFilesystem::copyRecursive(QString path, QString newPath)
{
    QDir dir(path);
    if (!dir.exists())
        return;

    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString newdir = newPath + '/' + d;
        dir.mkpath(newdir);
        copyRecursive(path + '/' + d, newdir);
    }

    foreach (QString f, dir.entryList(QDir::Files))
        QFile::copy(path + '/' + f, newPath + '/' + f);
}
