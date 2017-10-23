#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <QHash>

class SymTable
{
public:
    SymTable() {}

    void load(QString path);
    quint32 get(const QString& labels, bool* ok = NULL);

    void clear();

private:

    struct SymTableEntry
    {
        quint32 offset;
        quint32 size;
        bool wasMangled;
    };

    QHash<QString, SymTableEntry> symbols;
};

#endif // SYMTABLE_H
