#ifndef PATCHCOMPILER_H
#define PATCHCOMPILER_H

#include <QObject>
#include <QProcess>

class PatchCompiler : public QObject
{
    Q_OBJECT
public:
    explicit PatchCompiler(QObject* parent = nullptr);

    void setPath(const QString& path);

    void clean();
    void make(quint32 codeaddr);
    void make(quint32 codeaddr, quint32 dataaddr);

    enum CompilerAction
    {
        CompilerAction_None,
        CompilerAction_Make,
        CompilerAction_Clean
    };

    CompilerAction lastAction() { return m_lastAction; }

signals:
    void finished(int exitCode);
    void outputUpdate(QString text);

private slots:
    void parseProcessOutput();
    void handleProcessFinished(int exitCode);

private:
    QString m_path;

    bool m_busy;
    QProcess* m_process;
    QString m_currentOutput;

    CompilerAction m_lastAction;
};

#endif // PATCHCOMPILER_H
