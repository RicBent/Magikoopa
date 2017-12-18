#include "patchmaker.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <QTextStream>

#include "Filesystem/filesystem.h"
#include "exheader.h"


static const QStringList requiredFiles =
{
    "Makefile", "loader/Makefile", "code.bin", "exheader.bin"
};


PatchMaker::PatchMaker(QObject* parent) :
    QObject(parent),
    m_pathValid(false)
{
    loaderCompiler = new PatchCompiler(this);
    compiler = new PatchCompiler(this);

    connect(loaderCompiler, SIGNAL(finished(int)), this, SLOT(loaderCompilerDone(int)));
    connect(compiler, SIGNAL(finished(int)), this, SLOT(compilerDone(int)));

    connect(loaderCompiler, SIGNAL(outputUpdate(QString)), this, SLOT(onLoaderCompilerOutput(QString)));
    connect(compiler, SIGNAL(outputUpdate(QString)), this, SLOT(onCompilerOutput(QString)));

    connect(&m_hookLinker, SIGNAL(outputUpdate(QString)), this, SLOT(onHookLinkerOutput(QString)));
    connect(&m_loaderHookLinker, SIGNAL(outputUpdate(QString)), this, SLOT(onLoaderHookLinkerOutput(QString)));

    emit setBusy(false);
}

PatchMaker::~PatchMaker()
{

}

bool PatchMaker::setPath(const QString& newPath)
{
    QStringList missingFiles;
    for (int i = 0; i < requiredFiles.count(); i++)
    {
        if (!QFile(newPath + "/" + requiredFiles.at(i)).exists())
            missingFiles.append("/" + requiredFiles.at(i));
    }

    if (missingFiles.count() > 0)
    {
        QString missingFilesStr = "The working directory in invalid. The following files are missing:\n";
        for (int i = 0; i < missingFiles.count(); i++)
            missingFilesStr += "\n - " + missingFiles.at(i);

        QMessageBox::information(NULL, "Magikoopa", missingFilesStr, QMessageBox::Ok);
        return false;
    }


    m_path = newPath;
    m_pathValid = true;

    loaderCompiler->setPath(m_path + "/loader");
    compiler->setPath(m_path);

    checkBackup();

    Exheader exHeader(new ExternalFile(NULL, m_path + "/bak/exheader.bin"));
    m_loaderOffset = (exHeader.data.sci.textCodeSetInfo.size + 0x100000 + 0xF) & ~0xF;
    m_loaderMaxSize = exHeader.data.sci.readOnlyCodeSetInfo.address - m_loaderOffset;
    m_newCodeOffset = exHeader.data.sci.dataCodeSetInfo.address + (exHeader.data.sci.dataCodeSetInfo.physicalRegionSize << 12) + ((exHeader.data.sci.bssSize + 0xFFF) & ~0xFFF);

    emit addOutput("Info", QString("Game Name:           %1").arg(exHeader.data.sci.title), false);
    emit addOutput("Info", QString("Loader Offset:       %1").arg(m_loaderOffset, 8, 0x10, QChar('0')), false);
    emit addOutput("Info", QString("Loader maximum Size: %1").arg(m_loaderMaxSize, 8, 0x10, QChar('0')), false);
    emit addOutput("Info", QString("New Code Offset:     %1").arg(m_newCodeOffset, 8, 0x10, QChar('0')), false);

    emit updateStatus("Ready");

    return true;
}

void PatchMaker::makeInsert()
{
    emit setBusy(true);
    restoreFromBackup();
    emit updateStatus("Running Make...");
    compiler->make(m_newCodeOffset);
}

void PatchMaker::makeClean()
{
    emit updateStatus("Making Clean...");
    emit setBusy(true);
    compiler->clean();
}

void PatchMaker::loaderCompilerDone(int exitCode)
{
    if (loaderCompiler->lastAction() == PatchCompiler::CompilerAction_Clean)
    {
        emit updateStatus("Clean");
        emit setBusy(false);
    }

    else if (loaderCompiler->lastAction() == PatchCompiler::CompilerAction_Make)
    {
        if (exitCode != 0)
        {
            emit updateStatus("Compilation Failed (Loader)");
            emit setBusy(false);
            return;
        }

        m_loaderSymTable.clear();
        m_loaderSymTable.load(m_path + "/loader/loader.sym");

        m_loaderHookLinker.clear();
        m_loaderHookLinker.setSymTable(&m_loaderSymTable);
        m_loaderHookLinker.loadHooks(m_path + "/loader/source");

        quint32 loaderInsertSize = m_loaderSymTable.get("__text_end") - m_loaderSymTable.get("__text_start") + m_loaderHookLinker.extraDataSize();
        if (loaderInsertSize > m_loaderMaxSize)
        {
            emit updateStatus("Loader text size exeeds maximum");
            emit setBusy(false);
            return;
        }

        insert();

    }
}

void PatchMaker::compilerDone(int exitCode)
{
    if (compiler->lastAction() == PatchCompiler::CompilerAction_Clean)
        loaderCompiler->clean();

    else if (compiler->lastAction() == PatchCompiler::CompilerAction_Make)
    {
        if (exitCode == 0)
        {
            m_symTable.clear();
            m_symTable.load(m_path + "/newcode.sym");

            m_hookLinker.clear();
            m_hookLinker.setSymTable(&m_symTable);
            m_hookLinker.loadHooks(m_path + "/source");

            emit updateStatus("Running Make (Loader)...");

            // Export Header File
            QFile newcodeinfoFile(m_path + "/loader/source/newcodeinfo.h");
            newcodeinfoFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
            QTextStream out(&newcodeinfoFile);
            out << "#ifndef NEWCODEINFO_H\n"
                << "#define NEWCODEINFO_H\n"
                << "\n"
                << QString("#define NEWCODE_OFFSET 0x%1\n").arg(m_newCodeOffset, 8, 0x10, QChar('0'))
                << QString("#define NEWCODE_SIZE 0x%1\n").arg(QFile(m_path + "/newcode.bin").size() + m_hookLinker.extraDataSize(), 8, 0x10, QChar('0'))
                << "\n"
                << "#endif // NEWCODEINFO_H\n";
            newcodeinfoFile.close();

            QFile newcodeFile(m_path + "/newcode.bin");
            m_loaderDataOffset = m_newCodeOffset + ((newcodeFile.size() + 0xF) & ~0xF);

            loaderCompiler->make(m_loaderOffset, m_loaderDataOffset);
        }
        else
        {
            emit updateStatus("Compilation Failed");
            emit setBusy(false);
        }
    }
}

void PatchMaker::insert()
{
    emit updateStatus("Inserting...");

    bool ok0, ok1, ok2;
    quint32 loaderTextEnd = m_loaderSymTable.get("__text_end", &ok0);
    quint32 loaderDataStart = m_loaderSymTable.get("__data_start", &ok1);
    quint32 loaderDataEnd = m_loaderSymTable.get("__data_end", &ok2);
    if (!ok0 || !ok1 || !ok2)
    {
        emit updateStatus("Parsing Loader sections failed");
        emit setBusy(false);
        return;
    }


    ExternalFile* codeFile = new ExternalFile(m_path + "/code.bin");
    ExternalFile* loaderFile = new ExternalFile(m_path + "/loader/loader.bin");
    ExternalFile* newCodeFile = new ExternalFile(m_path + "/newcode.bin");

    codeFile->open();
    loaderFile->open();
    newCodeFile->open();

    quint32 oldCodeSize = codeFile->size();
    codeFile->resize(loaderDataEnd + m_hookLinker.extraDataSize() - 0x100000);

    // Insert Loader Text
    quint32 loaderTextSize = loaderTextEnd - m_loaderOffset;
    quint8* loaderText = new quint8[loaderTextSize];
    loaderFile->seek(0);
    loaderFile->readData(loaderText, loaderTextSize);
    codeFile->seek(m_loaderOffset - 0x100000);
    codeFile->writeData(loaderText, loaderTextSize);
    delete loaderText;

    // Clear BSS section
    codeFile->seek(oldCodeSize);
    while (codeFile->pos() < m_newCodeOffset - 0x100000)
        codeFile->write8(0);

    // Insert NewCode
    quint8* newCode = new quint8[newCodeFile->size()];
    newCodeFile->seek(0);
    newCodeFile->readData(newCode, newCodeFile->size());
    codeFile->seek(m_newCodeOffset - 0x100000);
    codeFile->writeData(newCode, newCodeFile->size());
    delete[] newCode;

    // Clear Padding to Loader Data
    while (codeFile->pos() < loaderDataStart - 0x100000)
        codeFile->write8(0);

    // Insert Loader Data
    quint32 loaderDataSize = loaderDataEnd - loaderDataStart;
    quint8* loaderData = new quint8[loaderDataSize];
    loaderFile->seek(loaderDataStart - m_loaderOffset);
    loaderFile->readData(loaderData, loaderDataSize);
    codeFile->seek(loaderDataStart - 0x100000);
    codeFile->writeData(loaderData, loaderDataSize);

    quint32 nextPageOffset = (codeFile->pos() + 0xFFF) & ~0xFFF;
    while (codeFile->pos() < nextPageOffset)
        codeFile->write8(0);

    qDebug() << QString("Loader Text Start: %1").arg(m_loaderOffset, 8, 0x10, QChar('0')).toLatin1().data();
    qDebug() << QString("Loader Text End:   %1").arg(loaderTextEnd, 8, 0x10, QChar('0')).toLatin1().data();
    qDebug() << QString("Loader Text Size:  %1").arg(loaderTextSize, 8, 0x10, QChar('0')).toLatin1().data();
    qDebug() << QString("Loader Data Start: %1").arg(loaderDataStart, 8, 0x10, QChar('0')).toLatin1().data();
    qDebug() << QString("Loader Data End:   %1").arg(loaderDataEnd, 8, 0x10, QChar('0')).toLatin1().data();
    qDebug() << QString("Loader Data Size:  %1").arg(loaderDataSize, 8, 0x10, QChar('0')).toLatin1().data();
    qDebug() << "";
    qDebug() << QString("New Code Start: %1").arg(m_newCodeOffset, 8, 0x10, QChar('0')).toLatin1().data();
    qDebug() << QString("New Code End:   %1").arg(m_newCodeOffset + newCodeFile->size(), 8, 0x10, QChar('0')).toLatin1().data();
    qDebug() << QString("New Code Size:  %1").arg(newCodeFile->size(), 8, 0x10, QChar('0')).toLatin1().data();


    // Hooks
    // Won't print ever but w/e
    emit updateStatus("Hooking...");

    m_hookLinker.setExtraDataptr(loaderDataEnd);
    m_loaderHookLinker.setExtraDataptr(loaderTextEnd);

    m_hookLinker.applyTo(codeFile);
    m_loaderHookLinker.applyTo(codeFile);


    codeFile->save();
    codeFile->close();

    loaderFile->close();
    newCodeFile->close();

    delete codeFile;
    delete loaderFile;
    delete newCodeFile;

    emit updateStatus("Fixing Exheader");
    fixExheader(loaderDataEnd - m_newCodeOffset);
}

void PatchMaker::fixExheader(quint32 newCodeSize)
{
    Exheader exHeader(new ExternalFile(m_path + "/exheader.bin"));

    exHeader.data.sci.textCodeSetInfo.size = exHeader.data.sci.textCodeSetInfo.physicalRegionSize << 12;

    exHeader.data.sci.dataCodeSetInfo.physicalRegionSize += ((exHeader.data.sci.bssSize + 0xFFF) & ~0xFFF) >> 12 ;
    exHeader.data.sci.dataCodeSetInfo.physicalRegionSize += ((newCodeSize + 0xFFF) & ~0xFFF) >> 12;
    exHeader.data.sci.dataCodeSetInfo.size = exHeader.data.sci.dataCodeSetInfo.physicalRegionSize << 12;

    exHeader.data.sci.bssSize = 0;


    // Set ARM11 Kernel Caps
    Exheader::ACI* aci = &exHeader.data.aci1;

    QList<quint32> otherCaps;

    bool svcs[0x100];
    memset(&svcs, 0, sizeof(svcs));

    for (int i = 0; i < 28; i++)
    {
        quint32 cap = aci->arm11kernelCaps.desciptors[i];

        if ((cap & 0xF8000000) == 0xF0000000)
        {
            quint32 mask = cap & 0x00FFFFFF;
            quint32 tableIndex = (cap & 0x03000000) >> 24;

            for (int i = 0; i < 24; i++)
            {
                if (mask & (1 << i))
                {
                    quint32 svc = tableIndex*24 + i;
                    if (svc < 0x100)
                        svcs[svc] = true;
                }
            }
        }
        else if (cap != 0xFFFFFFFF)
            otherCaps.append(cap);
    }

    // Allow SVC 0x70: ControlProcessMemory
    svcs[0x70] = true;
    QList<quint32> caps;

    for (int i = 0; i < 8; i++)
    {
        quint32 newCap = 0xF0000000;
        newCap |= i << 24;

        for (int j = 0; j < 24; j++)
            newCap |= svcs[i*24 + j] << j;

        if (newCap & 0x00FFFFFF)
            caps.append(newCap);
    }

    caps.append(otherCaps);

    if (caps.count() > 28)
    {
        emit setBusy(false);
        emit updateStatus("Setting ARM11 LSCs failed");
        return;
    }

    int i = 0;
    for (; i < caps.size(); i++)
        aci->arm11kernelCaps.desciptors[i] = caps[i];
    for (; i < caps.size(); i++)
        aci->arm11kernelCaps.desciptors[i] = 0xFFFFFFFF;


    exHeader.save();

    emit setBusy(false);
    emit updateStatus("All done");
}

quint32 PatchMaker::makeBranchOpcode(quint32 src, quint32 dest, bool link)
{
    quint32 ret = 0xEA000000;
    if (link) ret |= 0x01000000;

    int offset = (dest / 4) - (src / 4) - 2;
    offset &= 0x00FFFFFF;

    ret |= offset;

    return ret;
}

void PatchMaker::checkBackup()
{
    QDir(m_path).mkdir("bak");

    if (!QFile(m_path + "/bak/code.bin").exists())
        QFile(m_path + "/code.bin").copy(m_path + "/bak/code.bin");

    if (!QFile(m_path + "/bak/exheader.bin").exists())
        QFile(m_path + "/exheader.bin").copy(m_path + "/bak/exheader.bin");
}

void PatchMaker::restoreFromBackup()
{
    QDir dir(m_path);

    if (QFile(m_path + "/bak/code.bin").exists())
    {
        dir.remove("code.bin");
        QFile(m_path + "/bak/code.bin").copy(m_path + "/code.bin");
    }

    if (QFile(m_path + "/bak/exheader.bin").exists())
    {
        dir.remove("exheader.bin");
        QFile(m_path + "/bak/exheader.bin").copy(m_path + "/exheader.bin");
    }
}

void PatchMaker::onLoaderCompilerOutput(const QString& text)
{
    emit addOutput("Loader", text, false);
}

void PatchMaker::onCompilerOutput(const QString& text)
{
    emit addOutput("Compiler", text, false);
}

void PatchMaker::onHookLinkerOutput(const QString& text)
{
    emit addOutput("Hook Linker", text, false);
}

void PatchMaker::onLoaderHookLinkerOutput(const QString& text)
{
    emit addOutput("Loader Hook Linker", text, false);
}
