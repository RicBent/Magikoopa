#include "patchcompiler.h"

#include <QProcessEnvironment>
#include <QDebug>

PatchCompiler::PatchCompiler(QObject* parent) :
    QObject(parent), m_busy(false), m_lastAction(CompilerAction_None)
{
    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    QObject::connect(m_process, SIGNAL(readyRead()), this, SLOT(parseProcessOutput()));
    QObject::connect(m_process, SIGNAL(finished(int)), this, SLOT(handleProcessFinished(int)));
}

void PatchCompiler::setPath(const QString& path)
{
    m_path = path;
    m_process->setWorkingDirectory(m_path);
}

void PatchCompiler::clean()
{
    m_lastAction = CompilerAction_Clean;
    m_process->start("make", QStringList() << "clean");
}

void PatchCompiler::make(quint32 codeaddr)
{
    m_lastAction = CompilerAction_Make;
    m_process->start("make", QStringList() << QString("CODEADDR=0x%1").arg(codeaddr, 8, 0x10, QChar('0')));
}

void PatchCompiler::make(quint32 codeaddr, quint32 dataaddr)
{
    m_lastAction = CompilerAction_Make;
    m_process->start("make", QStringList() << QString("CODEADDR=0x%1").arg(codeaddr, 8, 0x10, QChar('0'))
                                           << QString("DATAADDR=0x%1").arg(dataaddr, 8, 0x10, QChar('0')));
}

void PatchCompiler::parseProcessOutput()
{
    m_currentOutput += m_process->readAll();

    while (true)
    {
        int i = m_currentOutput.indexOf('\n');
        if (i < 0) break;

        QString outputLine = m_currentOutput.left(i+1);
        emit outputUpdate(outputLine);
        qDebug() << outputLine;

        m_currentOutput = m_currentOutput.mid(i+1);
    }
}

void PatchCompiler::handleProcessFinished(int exitCode)
{
    emit finished(exitCode);
}
