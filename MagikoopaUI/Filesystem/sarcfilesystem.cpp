#include "filesystem.h"

SarcFilesystem::SarcFilesystem(FileBase* file, FilesystemMode mode)
{
    this->sarcFile = file;
    this->mode = mode;

    sarcFile->open();
    sarcFile->seek(0);

    QString magic;
    sarcFile->readStringASCII(magic, 4);
    if (magic != "SARC")
        throw new std::runtime_error("SARC: Bad tag");

    sarcFile->skip(2);                                              // Header Length
    quint16 bom = (sarcFile->read8() << 8) | sarcFile->read8();     // BOM
    if (bom == 0xFEFF) endianess = BigEndian;
    else if (bom == 0xFFFE) endianess = LittleEndian;
    else throw new std::runtime_error("Sarc: Bad BOM");
    sarcFile->skip(4);                                              // File Size
    dataOffset = sarcFile->read32();                                // Data Offset
    sarcFile->skip(2);                                              // Unknown 0: Allways 0x0100?
    sarcFile->skip(2);                                              // Unknown 1: Allways 0x0000?

    sfatOffset = (quint32)file->pos();
    sarcFile->skip(6);
    numFiles = sarcFile->read16();
    hashMult = sarcFile->read32();

    sfntOffset = sfatOffset + 0xC + (numFiles * 0x10);

    for (quint32 i = 0; i < numFiles; i++)
    {
        file->seek(sfatOffset + 0xC + (i * 0x10));
        InternalSarcFile* entry = new InternalSarcFile();
        entry->entryOffset = (quint32)file->pos();

        entry->nameHash = file->read32();
        entry->nameOffset = (file->read32() & 0x00FFFFFF) * 4;
        entry->offset = file->read32();
        entry->size = file->read32() - entry->offset;

        file->seek(sfntOffset + 0x8 + entry->nameOffset);
        file->readStringASCII(entry->name);

        entry->newData = NULL;

        files.insert(entry->name, entry);
    }

    sarcFile->close();
}

SarcFilesystem::~SarcFilesystem()
{
    delete sarcFile;
}

bool SarcFilesystem::directoryExists(QString path)
{
    if (path[0] == '/')
        path.remove(0, 1);

    if (path.length() && !path.endsWith('/'))
        path += '/';

    foreach (QString file, files.keys())
        if (file.startsWith(path))
            return true;

    return false;
}

void SarcFilesystem::directoryContents(QString path, Filter filter, QList<QString>& out)
{
    if (path[0] == '/')
        path.remove(0, 1);

    if (path.length() && !path.endsWith('/'))
        path += '/';

    out.clear();

    for (int i = 0; i < files.size(); i++)
    {
        QString thispath = files.keys()[i];

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

bool SarcFilesystem::fileExists(QString path)
{
    if (path[0] == '/')
        path.remove(0, 1);

    return files.contains(path);
}

FileBase* SarcFilesystem::openFile(QString path)
{
    if (path[0] == '/')
        path.remove(0, 1);

    FileBase* ret;

    if (files.contains(path))
    {
        InternalSarcFile* entry = files[path];
        ret = sarcFile->getSubfile(this, dataOffset + entry->offset, entry->size);
    }
    else
        ret = new MemoryFile(this);

    ret->setIdPath(path);
    ret->setEndianess(endianess);
    return ret;
}

bool SarcFilesystem::save(FileBase* file)
{
    if (!files.contains(file->getIdPath()))
    {
        InternalSarcFile* newEntry = new InternalSarcFile();
        newEntry->name = file->getIdPath();
        files.insert(newEntry->name, newEntry);
        numFiles++;
    }

    files[file->getIdPath()]->newData = file->clone(NULL);

    if (mode == FilesystemMode::AutoSave)
        return repack();
    return true;
}

bool SarcFilesystem::save()
{
    if (mode != FilesystemMode::ManualSave)
        qWarning("SarcFilesystem: save() called in automatic mode");
    return repack();
}

bool SarcFilesystem::repack()
{
    numFiles = files.count();       // just to be sure


    // Generate sorted List, since SARCs want to be ordered after their File Name Hash
    QList<InternalSarcFile*> sortedFiles;
    for (int i = 0; i < files.size(); i++)
    {
        InternalSarcFile* ifile = files.values()[i];
        ifile->nameHash = filenameHash(ifile->name);
        sortedFiles.append(ifile);
    }
    std::sort(sortedFiles.begin(), sortedFiles.end(), hashSort);


    // Calculate File Size

    quint32 fileSize = 0x14;        // SARC Header
    sfatOffset = 0x14;
    fileSize += 0xC;                // SFAT Header
    fileSize += 0x10 * numFiles;    // SFAT Nodes
    fileSize += 0x8;                // SFNT Header
    quint32 sfntNodeOffset = fileSize;

    // SFNT Nodes
    for (quint16 i = 0; i < numFiles; i++)
    {
        fileSize = align(fileSize, 4);
        sortedFiles[i]->nameOffset = (fileSize - sfntNodeOffset) / 4;
        fileSize += sortedFiles[i]->name.length() + 1;
    }


    fileSize = align(fileSize, SARC_DATA_ALIGN);
    quint32 oldDataOffset = dataOffset;
    dataOffset = fileSize;


    // Data
    quint32* oldOffsets = new quint32[numFiles];

    for (quint16 i = 0; i < numFiles; i++)
    {
        InternalSarcFile* ifile = sortedFiles[i];
        oldOffsets[i] = ifile->offset;

        if (i != 0)
            fileSize = align(fileSize, SARC_DATA_ALIGN);

        if (ifile->newData != NULL)
            ifile->size = ifile->newData->size();

        ifile->offset = fileSize - dataOffset;
        fileSize += ifile->size;
    }


    // Backup old data
    FileBase* oldSarc = sarcFile->clone(NULL);
    oldSarc->open();

    // Write SARC

    sarcFile->open();
    sarcFile->resize(fileSize);
    sarcFile->seek(0);

    // SARC Header
    sarcFile->writeStringASCII("SARC", 4);      // SARC Magic
    sarcFile->write16(0x14);                    // SARC Header Length
    sarcFile->write16(0xFEFF);                  // BOM
    sarcFile->write32(fileSize);                // Archive Size
    sarcFile->write32(dataOffset);              // Data Offset
    sarcFile->write16(0x0100);                  // Unknown 0; Allways 0x0100?
    sarcFile->write16(0x0000);                  // Unknown 1; Padding?

    // SFAT Header
    sarcFile->writeStringASCII("SFAT", 4);      // SFAT Magic
    sarcFile->write16(0xC);                     // SFAT Header Length
    sarcFile->write16(numFiles);                // Node Count
    sarcFile->write32(hashMult);                // Hash Multiplier

    //SFAT Nodes
    for (quint16 i = 0; i < numFiles; i++)
    {
        InternalSarcFile* ifile = sortedFiles[i];
        writeAlign(4);
        sarcFile->write32(ifile->nameHash);                                 // Name Hash
        sarcFile->write32((ifile->nameOffset & 0x00FFFFFF) | 0x01000000);   // Attributes
        sarcFile->write32(ifile->offset);                                   // Data Beginning
        sarcFile->write32(ifile->offset + ifile->size);                     // Data End
    }

    // SFNT Header
    sarcFile->writeStringASCII("SFNT", 4);      // SFNT Header
    sarcFile->write16(0x8);                     // SFNT Header Length
    sarcFile->write16(0x0000);                  // Allways 0x0000; padding?

    // SFNT Nodes
    for (quint16 i = 0; i < numFiles; i++)
    {
        writeAlign(4);
        sarcFile->writeStringASCII(sortedFiles[i]->name);
    }

    writeAlign(SARC_DATA_ALIGN);

    // Data
    for (quint16 i = 0; i < numFiles; i++)
    {
        if (i != 0)
            writeAlign(SARC_DATA_ALIGN);

        InternalSarcFile* ifile = sortedFiles[i];

        quint8* tempbuf = new quint8[4096];
        quint64 pos = 0;
        if (ifile->newData == NULL)
        {
            // Write existing, not modified data

            oldSarc->seek(oldDataOffset + oldOffsets[i]);

            while (pos < ifile->size)
            {
                quint64 toread = 4096;
                if ((pos+toread) > ifile->size) toread = ifile->size-pos;

                oldSarc->readData(tempbuf, toread);
                sarcFile->writeData(tempbuf, toread);
                pos += toread;
            }
        }
        else
        {
            // Write modified data

            ifile->newData->open();
            ifile->newData->seek(0);

            while (pos < ifile->size)
            {
                quint64 toread = 4096;
                if ((pos+toread) > ifile->size) toread = ifile->size-pos;

                ifile->newData->readData(tempbuf, toread);
                sarcFile->writeData(tempbuf, toread);
                pos += toread;
            }

            ifile->newData->close();
            delete ifile->newData;
            ifile->newData = NULL;
        }
        delete[] tempbuf;
    }


    sarcFile->save();
    sarcFile->close();

    oldSarc->close();
    delete[] oldOffsets;
    delete oldSarc;

    return true;
}

bool SarcFilesystem::copy(QString, QString)
{
    qWarning("SarcFilesystem stub");
    return false;
}

bool SarcFilesystem::rename(QString oldPath, QString newPath)
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
    foreach (InternalSarcFile* file, files.values())
    {
        if (file->name.startsWith(oldPath))
        {
            QString subpath = file->name.remove(0, oldPath.length());
            if (!newPath.endsWith('/') && !subpath.startsWith('/')) subpath = '/' + subpath;
            file->name = newPath + subpath;

            if (file->name[0] == '/')
                file->name.remove(0, 1);

            files.remove(oldPath + subpath);
            files.insert(file->name, file);
            renamed = true;
        }
    }

    if (renamed && mode == FilesystemMode::AutoSave)
        repack();

    return renamed;
}

bool SarcFilesystem::remove(QString path)
{
    if (path[0] == '/')
        path.remove(0, 1);

    if (path.endsWith('/'))
        path.chop(1);

    bool removed = false;
    foreach (InternalSarcFile* file, files.values())
    {
        if (path == "" || file->name == path || file->name.startsWith(path + '/'))
        {
            files.remove(file->name);
            delete file;
            numFiles--;
            removed = true;
        }
    }

    if (removed && mode == FilesystemMode::AutoSave)
        repack();

    return removed;
}

quint32 SarcFilesystem::filenameHash(QString& name)
{
    quint32 ret = 0;
    for (int i = 0; i < name.size(); i++)
    {
        ret *= hashMult;
        ret += name[i].toLatin1();
    }
    return ret;
}
