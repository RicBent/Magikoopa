#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "patchmaker.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void appendOutput(const QString& catergory, const QString& text, bool showCategory);

    void setActionsDisabled(bool disabled);

    void on_actionSet_Working_Directory_triggered();

    void on_makeInsertButton_clicked();

    void on_makeCleanButton_clicked();

    void on_actionQuit_triggered();

private:
    Ui::MainWindow* ui;

    PatchMaker* patchMaker;

    enum IssueType
    {
        Warning,
        Error
    };

    void addIssue(QString text, IssueType type);
    int m_issueCount;

    void clearIssues();
};

#endif // MAINWINDOW_H
