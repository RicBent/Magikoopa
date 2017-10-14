#ifndef PATCHMAKER_H
#define PATCHMAKER_H

#include <QObject>

#include "patchcompiler.h"

class PatchMaker : public QObject
{
    Q_OBJECT
public:
    explicit PatchMaker(QObject* parent = nullptr);
    ~PatchMaker();

    bool setPath(const QString& newPath);

    bool pathValid() { return m_pathValid; }
    const QString& path() { return m_path; }

    void makeInsert();
    void makeClean();

signals:
    void setBusy(bool busy);
    void addOutput(const QString& category, const QString& text, bool showCategory);
    void updateStatus(const QString& status);

private slots:
    void loaderCompilerDone(int exitCode);
    void compilerDone(int exitCode);

    void onLoaderCompilerOutput(const QString& text);
    void onCompilerOutput(const QString& text);

private:
    void checkBackup();
    void restoreFromBackup();

    void insert();
    void fixExheader(quint32 newCodeSize);

    static quint32 makeBranchOpcode(quint32 src, quint32 dest, bool link);

private:
    bool m_pathValid;
    QString m_path;

    PatchCompiler* loaderCompiler;
    PatchCompiler* compiler;

    quint32 m_loaderOffset;
    quint32 m_loaderMaxSize;
    quint32 m_newCodeOffset;
    quint32 m_loaderDataOffset;
};

#endif // PATCHMAKER_H
