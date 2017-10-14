#ifndef YAZ0FILE_H
#define YAZ0FILE_H

class Yaz0File : public CompressedFile
{
public:
    Yaz0File(FileBase* file);
    ~Yaz0File();

    bool save(FileBase* file);
    FileBase* getFile();


private:
    FileBase* yaz0file;
};

#endif // YAZ0FILE_H
