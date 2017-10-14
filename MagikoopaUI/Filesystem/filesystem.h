#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QString>


/*
 * Filesystem - TODO:
 *
 * - SARC:
 *   - Renaming with non-full paths
 *   - Copy
 *
 * - Yaz0:
 *   - Real compression
 *   - Fake compression: optimize
 *   - Decompression: Optimize. A bit slow for external files. Parse in blocks?
 *
 */


enum Endianess
{
    LittleEndian,
    BigEndian
};

enum FilesystemMode
{
    AutoSave,
    ManualSave
};

class FileBase;
class FileContainer;
class FilesystemBase;

class ExternalFile;
class MemoryFile;

#include "filebase.h"
#include "memoryfile.h"
#include "externalfile.h"

#include "filecontainer.h"

#include "filesystembase.h"
#include "externalfilesystem.h"
#include "plainarchive.h"
#include "sarcfilesystem.h"

#include "compressedfile.h"
#include "yaz0file.h"

#endif // FILESYSTEM_H
