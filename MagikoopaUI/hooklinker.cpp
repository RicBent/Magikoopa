#include "hooklinker.h"

#include <QDirIterator>
#include <QDebug>
#include <QMessageBox>

Hook* HookLinker::hookFromInfo(HookInfo* info)
{
    if (!info->has("type"))
        throw new HookExeption(info, "No type given");

    QString type = info->get("type").toLower();

    if (type == "branch")
        return new BranchHook(this, info);
    if (type == "softbranch" || type == "soft_branch")
        return new SoftBranchHook(this, info);
    else
        throw new HookExeption(info, QString("Invalid type \"%1\"").arg(type));

    delete info;
    return NULL;
}


HookLinker::HookLinker(QObject* parent) : QObject(parent)
{

}

HookLinker::~HookLinker()
{
    clear();
}

void HookLinker::loadHooks(const QString& path, LoadMode mode)
{
    if (mode == LoadMode::LoadFile)
        loadHooksFromFile(path);

    else
    {
        QDirIterator::IteratorFlag iFlags;
        if (mode == LoadDir)
            iFlags = QDirIterator::NoIteratorFlags;
        else
            iFlags = QDirIterator::Subdirectories;

        QDirIterator dirIt(path, iFlags);
        while (dirIt.hasNext())
        {
            dirIt.next();

            if (dirIt.fileInfo().isFile() &&
                dirIt.fileInfo().suffix() == "hks")
            {
                loadHooksFromFile(dirIt.filePath());
            }
        }
    }
}

void HookLinker::loadHooksFromFile(const QString& path)
{
    QFile f(path);

    if (!f.exists()) return;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream s(&f);

    QList<HookInfo*> entries;

    HookInfo* current = NULL;
    quint32 lineNbr = 0;
    while (!s.atEnd())
    {
        lineNbr++;
        QString line = s.readLine();

        qint32 hashIndex = line.indexOf('#');
        if (hashIndex >= 0)
            line = line.left(hashIndex);

        // New Entry
        if (!line.startsWith(' ') && !line.startsWith(' ') && line.contains(':'))
        {
            current = new HookInfo(line.left(line.indexOf(':')), path, lineNbr);
            entries.append(current);
        }

        // Data
        if (current != NULL && line.contains(':'))
        {
            if (!line.startsWith('\t') && !line.startsWith(' '))
                continue;

            while (line.startsWith('\t') || line.startsWith(' '))
                line = line.mid(1);

            int index = line.indexOf(':');

            QString label = line.left(index);
            line = line.mid(index + 1);

            while (line.startsWith('\t') || line.startsWith(' '))
                line = line.mid(1);

            while (line.endsWith('\t') || line.endsWith(' '))
                line.chop(1);

            current->values.insert(label, line);
        }
    }

    f.close();

    foreach (HookInfo* info, entries)
    {
        try
        {
            Hook* hk = hookFromInfo(info);
            if (hk) hooks.append(hk);
        }
        catch (HookExeption* e)
        {
            const HookInfo& info = e->info();
            emit outputUpdate(info.path + ":" + QString::number(info.line) + ": error: Hook: " + e->msg());
        }
    }
}

quint32 HookLinker::extraDataSize()
{
    quint32 extraDataSize = 0;

    foreach (Hook* hook, hooks)
        extraDataSize += hook->extraDataSize();

    return extraDataSize;
}

void HookLinker::applyTo(FileBase* file)
{
    quint32 extraDataPtrCurr = m_extraDataPtr;
    foreach (Hook* hook, hooks)
    {
        hook->writeData(file, extraDataPtrCurr);
        extraDataPtrCurr += hook->extraDataSize();
    }
}

void HookLinker::clear()
{
    foreach (Hook* hook, hooks)
        delete hook;

    hooks.clear();
}
