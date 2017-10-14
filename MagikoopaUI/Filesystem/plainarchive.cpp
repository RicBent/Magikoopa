#include "filesystem.h"

PlainArchive::PlainArchive()
{

}

bool PlainArchive::directoryExists(QString path)
{
    if (path[0] == '/')
        path.remove(0, 1);

    if (path.length() && !path.endsWith('/'))
        path += '/';

    foreach (QString file, files()->keys())
        if (file.startsWith(path))
            return true;

    return false;
}

void PlainArchive::directoryContents(QString path, Filter filter, QList<QString>& out)
{
    if (path[0] == '/')
        path.remove(0, 1);

    if (path.length() && !path.endsWith('/'))
        path += '/';

    out.clear();

    for (int i = 0; i < files()->size(); i++)
    {
        QString thispath = files()->keys()[i];

        if (thispath.startsWith(path))
        {
            thispath.remove(0, path.length());

            int slashidx = thispath.indexOf('/');
            bool isdir = (slashidx != -1);

            if ((isdir && (filter & Filter::Dirs)) || ((!isdir) && (filter & Filter::Files)))
            {
                if (isdir) thispath = thispath.left(slashidx);
                if (!out.contains(thispath))
                    out.append(thispath);
            }
        }
    }
}

bool PlainArchive::fileExists(QString path)
{
    if (path[0] == '/')
        path.remove(0, 1);

    return files()->contains(path);
}

FileBase* PlainArchive::openFile(QString path)
{
    if (path[0] == '/')
        path.remove(0, 1);

    FileBase* ret;

    if (files()->contains(path))
    {
        InternalFile* entry = files()->value(path);
        ret = container->getSubfile(this, dataOffset + entry->offset, entry->size);
    }
    else
        ret = new MemoryFile(this);

    ret->setIdPath(path);
    ret->setEndianess(endianess);
    return ret;
}

bool PlainArchive::save(FileBase* file)
{
    if (!files()->contains(file->getIdPath()))
    {
        InternalFile* newEntry = new InternalFile();
        newEntry->name = file->getIdPath();
        files()->insert(newEntry->name, newEntry);
        numFiles++;
    }

    files()->value(file->getIdPath())->newData = file->clone(NULL);

    if (mode == FilesystemMode::AutoSave)
        return repack();
    return true;
}

bool PlainArchive::save()
{
    return repack();
}

bool PlainArchive::copy(QString, QString)
{
    qWarning("SarcFilesystem stub");
    return false;
}

bool PlainArchive::rename(QString oldPath, QString newPath)
{
    if (oldPath[0] == '/')
        oldPath.remove(0, 1);

    if (newPath[0] == '/')
        newPath.remove(0, 1);

    if (oldPath.endsWith('/'))
        oldPath.chop(1);

    if (newPath.endsWith('/'))
        newPath.chop(1);

    bool renamed = false;
    foreach (InternalFile* file, files()->values())
    {
        if (file->name.startsWith(oldPath))
        {
            QString subpath = file->name.remove(0, oldPath.length());
            if (!newPath.endsWith('/') && !subpath.startsWith('/')) subpath = '/' + subpath;
            file->name = newPath + subpath;

            if (file->name[0] == '/')
                file->name.remove(0, 1);

            files()->remove(oldPath + subpath);
            files()->insert(file->name, file);
            renamed = true;
        }
    }

    if (renamed && mode == FilesystemMode::AutoSave)
        repack();

    return renamed;
}

bool PlainArchive::remove(QString path)
{
    if (path[0] == '/')
        path.remove(0, 1);

    if (path.endsWith('/'))
        path.chop(1);

    bool removed = false;
    foreach (InternalFile* file, files()->values())
    {
        if (path == "" || file->name == path || file->name.startsWith(path + '/'))
        {
            files()->remove(file->name);
            delete file;
            numFiles--;
            removed = true;
        }
    }

    if (removed && mode == FilesystemMode::AutoSave)
        repack();

    return removed;
}
