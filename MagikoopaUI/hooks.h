#ifndef HOOKS_H
#define HOOKS_H

#include "symtable.h"
#include "Filesystem/filesystem.h"

#include <exception>
#include <QByteArray>

class HookLinker;
class HookInfo;

// Extradata | Code | Symbols

class Hook
{
public:
    virtual ~Hook();

    virtual void writeData(FileBase*, quint32) {}

    virtual quint32 extraDataSize() { return 0; }

protected:
    Hook() {}
    void base(HookLinker* parent, HookInfo* info);

    static quint32 makeBranchOpcode(quint32 src, quint32 dest, bool link);
    static quint32 offsetOpcode(quint32 opcode, quint32 orgPosition, qint32 newPosition);

    QString m_name;
    quint32 m_address;
    QByteArray m_data;

    HookLinker* m_parent;
    HookInfo* m_info;
};

class BranchHook : public Hook
{
public:
    BranchHook(HookLinker* parent, HookInfo* info);
    void writeData(FileBase* file, quint32 extraDataPos);

private:
    bool m_link;
    quint32 m_destination;
};

class SoftBranchHook : public Hook
{
public:
    SoftBranchHook(HookLinker* parent, HookInfo* info);
    void writeData(FileBase* file, quint32 extraDataPtr);
    quint32 extraDataSize() { return 5*4; }

private:
    bool m_insertPre;
    quint32 m_destination;
};

#endif // HOOKS_H
