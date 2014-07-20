#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    readSettings();

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(handleActionOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(handleActionSave()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(handleActionSaveAs()));

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(handleTextChanged()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::handleActionOpen()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open config file");
    if(filename.isEmpty())
        return;
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    ui->textEdit->setPlainText(stream.readAll());
    file.close();

    this->filename = filename;
    this->filenameShort = QFileInfo(file).fileName();
    setWindowTitle(filenameShort);
}

void MainWindow::handleActionSave()
{
    if(this->filename.isEmpty()) {
        QString filename = QFileDialog::getSaveFileName(this, "Save config file");
        if(filename.isEmpty())
            return;
        this->filename = filename;
        this->filenameShort = QFileInfo(QFile(filename)).fileName();
    }

    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream stream(&file);
    stream << ui->textEdit->toPlainText();
    file.close();

    setWindowTitle(filenameShort);
}

void MainWindow::handleActionSaveAs()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save as config file");
    if(filename.isEmpty())
        return;
    this->filename = filename;
    this->filenameShort = QFileInfo(QFile(filename)).fileName();

    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream stream(&file);
    stream << ui->textEdit->toPlainText();
    file.close();

    setWindowTitle(filenameShort);
}

void MainWindow::handleTextChanged()
{
    if(this->filename.isEmpty())
        setWindowTitle("Empty file*");
    else
        setWindowTitle(filenameShort + "*");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
}

void MainWindow::readSettings()
{
    QSettings settings;

    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());
}
