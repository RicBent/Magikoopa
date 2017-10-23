#ifndef HOOKLINKER_H
#define HOOKLINKER_H

#include "Filesystem/filesystem.h"
#include "symtable.h"
#include "hooks.h"

#include <QObject>
#include <QDebug>

struct HookInfo
{
    HookInfo() {}
    HookInfo(const QString& name, const QString& path, quint32 line) : name(name), path(path), line(line) {}

    QString name;
    QString path;
    quint32 line;
    QHash<QString, QString> values;

    QString operator[](const QString& key) { return values.value(key); }
    qint32 size() { return values.size(); }

    QString get(const QString& key)
    {
        return values.value(key);
    }

    bool has(const QString& key)
    {
        return values.contains(key);
    }

    bool getBool(const QString& key)
    {
        return (values.value(key).toLower() == "true");
    }

    quint32 getUint(const QString& key, bool* ok = NULL)
    {
        QString value = values.value(key);
        if (value.startsWith("0x"))
            return value.mid(2).toUInt(ok, 0x10);
        return value.toUInt(ok, 10);
    }
};

class HookExeption : public std::exception
{
public:
    HookExeption(HookInfo* info, const QString& msg)
    {
        m_info = *info;
        m_msg = msg;
    }

    const HookInfo& info() { return m_info; }
    const QString& msg() { return m_msg; }

    virtual const char* what() const throw()
    {
        return m_msg.toLatin1().data();
    }

private:
    HookInfo m_info;
    QString m_msg;
};

class HookLinker : public QObject
{
    Q_OBJECT
public:
    explicit HookLinker(QObject* parent = nullptr);
    ~HookLinker();

    enum LoadMode { LoadFile, LoadDir, LoadSubdirs };
    void loadHooks(const QString& path, LoadMode mode = LoadDir);

    void setExtraDataptr(quint32 extraDataPtr) { m_extraDataPtr = extraDataPtr; }
    quint32 extraDataSize();

    void setSymTable(SymTable* symTable) { m_symTable = symTable; }
    SymTable* symTable() { return m_symTable; }

    Hook* hookFromInfo(HookInfo* info);

    void applyTo(FileBase* file);
    void clear();

signals:
    void outputUpdate(QString text);

private:
    SymTable* m_symTable = NULL;

    void loadHooksFromFile(const QString& path);

    QList<Hook*> hooks;

    quint32 m_extraDataPtr;
};

#endif // HOOKLINKER_H
