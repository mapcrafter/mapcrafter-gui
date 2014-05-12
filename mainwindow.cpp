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

    QSettings settings;
    ui->mapcrafterPath->setText(settings.value("mapcrafterPath", "").toString());

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(handleActionOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(handleActionSave()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(handleActionSaveAs()));

    connect(ui->buttonMapcrafterPath, SIGNAL(clicked()), this, SLOT(handleButtonMapcrafterPath()));
    connect(ui->buttonRender, SIGNAL(clicked()), this, SLOT(handleButtonRender()));

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

void MainWindow::handleButtonMapcrafterPath() {
    QString filename = QFileDialog::getOpenFileName(this, "Path to Mapcrafter executable");
    if(filename.isEmpty())
        return;

    QFileInfo info(filename);
    if(!info.isExecutable()) {
        QMessageBox::critical(this, "Error", "The Mapcrafter executable file must be executable!");
        return;
    }

    ui->mapcrafterPath->setText(filename);

    QSettings settings;
    settings.setValue("mapcrafterPath", filename);
}

void MainWindow::handleButtonRender()
{
    if(filename.isEmpty()) {
        QMessageBox::critical(this, "Error", "You have to save the configuration file before you can render it!");
        return;
    }

    if(ui->mapcrafterPath->text().isEmpty()) {
        QMessageBox::critical(this, "Error", "You have to specify a path to the Mapcrafter executable!");
        return;
    }

    QStringList args;
    args << "-c" << filename;
    args << "-j" << QString::number(ui->threads->value());
    args << ui->additionalOptions->text().split(" ");
    ui->terminal->start(ui->mapcrafterPath->text(), args);
}

void MainWindow::handleTextChanged()
{
    if(this->filename.isEmpty())
        setWindowTitle("Empty file*");
    else
        setWindowTitle(filenameShort + "*");
}
