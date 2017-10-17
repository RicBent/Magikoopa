#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPushButton>
#include <QListWidgetItem>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_issueCount(0)
{
    ui->setupUi(this);

    settings = new QSettings("Blarg City", "Magikoopa");
    loadSettings();

    patchMaker = new PatchMaker(this);
    connect(patchMaker, SIGNAL(setBusy(bool)), SLOT(setActionsDisabled(bool)));
    connect(patchMaker, SIGNAL(addOutput(QString,QString,bool)), this, SLOT(appendOutput(QString,QString,bool)));
    connect(patchMaker, SIGNAL(updateStatus(QString)), ui->statusLabel, SLOT(setText(QString)));

    setActionsDisabled(true);
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete settings;

    delete ui;
}

void MainWindow::setActionsDisabled(bool disabled)
{
    ui->makeInsertButton->setEnabled(!disabled);
    ui->makeCleanButton->setEnabled(!disabled);
}

void MainWindow::appendOutput(const QString& catergory, const QString& text, bool showCategory)
{
    QString logText = text;

    if (logText.endsWith('\n'))
        logText.chop(1);

    if (showCategory)
        logText.prepend("[" + catergory + "] ");
    ui->output->appendPlainText(logText);

    QTextCursor c = ui->output->textCursor();
    c.movePosition(QTextCursor::End);
    ui->output->setTextCursor(c);


    QStringList lines = text.split('\n');

    foreach (const QString& line, lines)
    {
        QString lowerLine = line.toLower();
        if (lowerLine.contains("warning"))
        {
            addIssue(logText, Warning);
            break;
        }
        if (lowerLine.contains("error"))
        {
            addIssue(logText, Error);
            break;
        }
    }
}

void MainWindow::addIssue(QString text, IssueType type)
{
    QString iconPath = QCoreApplication::applicationDirPath() + "/3dsPatcher_data/icons/";
    if (type == Warning) iconPath += "warning.png";
    else if (type == Error) iconPath += "error.png";

    int index = text.indexOf(' ');

    QString location = text.left(index-1);
    QStringList locSegs = location.split(':');
    if (locSegs.count() < 2) return;
    QString path = locSegs.at(0);
    int line = locSegs.at(1).toInt();
    int column = 0;

    if (locSegs.count() >= 3)
    {
        bool ok;
        int column_ = locSegs.at(2).toInt(&ok);
        if (ok) column = column_;
    }

    QString message = text.mid(text.indexOf(' ', index+1));

    QListWidgetItem* item = new QListWidgetItem(QIcon(iconPath), QString("%1\n%2, Line %3").arg(message).arg(path).arg(line));
    item->setData(Qt::UserRole, path);
    item->setData(Qt::UserRole + 1, line);
    item->setData(Qt::UserRole + 2, column);

    ui->issues->addItem(item);

    m_issueCount++;
    ui->tabWidget->setTabText(1, QString("Issues (%1)").arg(m_issueCount));
}

void MainWindow::clearIssues()
{
    ui->issues->clear();
    m_issueCount = 0;
    ui->tabWidget->setTabText(1, "Issues");
}

void MainWindow::on_actionSet_Working_Directory_triggered()
{
    QString newPath = QFileDialog::getExistingDirectory(this, "Select Working Directory...", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (newPath == "")
        return;

    if (patchMaker->setPath(newPath))
    {
        this->setWindowTitle("Magikoopa - " + patchMaker->path());
        setActionsDisabled(false);
    }
}

void MainWindow::on_makeInsertButton_clicked()
{
    clearIssues();
    ui->output->clear();
    patchMaker->makeInsert();
}

void MainWindow::on_makeCleanButton_clicked()
{
    clearIssues();
    ui->output->clear();
    patchMaker->makeClean();
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

// Open Issue in text editor if it is valid
void MainWindow::on_issues_doubleClicked(const QModelIndex& index)
{
    QString commandString = settings->value("textEditorCmd", "").toString();
    if (commandString == "")
    {
        QMessageBox::information(this, "Magikoopa", "You have to set the editor to be able to jump to issues:\n\nSettings -> Set Text Editor", QMessageBox::Ok);
        return;
    }

    QString path = ui->issues->model()->data(index, Qt::UserRole).toString();

    if (path.isEmpty())
        return;

    qint32 line = ui->issues->model()->data(index, Qt::UserRole + 1).toInt();
    qint32 column = ui->issues->model()->data(index, Qt::UserRole + 2).toInt();

    commandString.replace("%path%", path);
    commandString.replace("%line%", QString::number(line));
    commandString.replace("%column%", QString::number(column));

    int idx = commandString.indexOf(" ");
    QString command = commandString.left(idx);

    commandString = commandString.mid(idx+1);

    QStringList args = commandString.split(" ", QString::SkipEmptyParts);

    QProcess* textEditor = new QProcess(this);
    textEditor->startDetached(command, args);
}


//
// Settings Related
//

void MainWindow::loadSettings()
{

}

void MainWindow::saveSettings()
{

}

void MainWindow::on_actionSetEditor_triggered()
{
    QString oldEditor = settings->value("textEditorCmd", "").toString();
    bool ok;
    QString newEditor = QInputDialog::getText(this, "Magikoopa",
                                              "Enter a command that opens a text editor:\n\nAvailable variables: %path% %line% %column%\n",
                                              QLineEdit::Normal, oldEditor, &ok);

    if (ok)
        settings->setValue("textEditorCmd", newEditor);
}
