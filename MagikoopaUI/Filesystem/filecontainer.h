#ifndef FILECONTAINER_H
#define FILECONTAINER_H

class FileContainer
{
public:
    virtual ~FileContainer() {}

    virtual bool save(FileBase* file) = 0;
};

#endif // FILECONTAINER_H
