#include "symtable.h"

#include <QFile>
#include <QTextStream>

#include <cxxabi.h>

void SymTable::load(QString path)
{
    QFile file(path);
    if (!file.exists())
        return;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream read(&file);
    QString line;
    while(!read.atEnd())
    {
        line = read.readLine();
        line.replace('\t', ' ');

        QStringList segs = line.split(' ', QString::SkipEmptyParts);
        if (segs.count() < 3) continue;
        bool ok;

        SymTableEntry entry;
        entry.wasMangled = false;

        entry.offset = segs.at(0).toInt(&ok, 0x10);
        if (!ok) continue;

        entry.size = segs.at(segs.count()-2).toInt(&ok, 0x10);
        if (!ok) continue;

        QString name = segs.at(segs.count()-1);

        // Mangled C++ name
        if (name.startsWith("_Z"))
        {
            char* mangledName = name.toLatin1().data();
            char* demangledName;

            int status = -1;
            demangledName = abi::__cxa_demangle(mangledName, NULL, NULL, &status);
            if (status < 0) continue;

            name = QString(demangledName);
            entry.wasMangled = true;
        }

        symbols.insert(name, entry);
    }

    file.close();
}

quint32 SymTable::get(const QString& name, bool* ok)
{
    // TODO: maybe parse names a bit so you do not have to be that exact with them?
    //       C++ fuctions have to be formatter correctly otherwise

    if (!symbols.contains(name))
    {
        if (ok) *ok = false;
        return 0xFFFFFFFF;
    }

    const SymTableEntry& entry = symbols.value(name);

    if (ok) *ok = true;
    return entry.offset;
}

void SymTable::clear()
{
    symbols.clear();
}
