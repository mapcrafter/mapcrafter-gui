#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <mapcraftercore/version.h>
#include <mapcraftercore/config/mapcrafterconfig.h>

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
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
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(handleActionAbout()));

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(handleTextChanged()));

    connect(ui->buttonValidateConfig, SIGNAL(clicked()), this, SLOT(handleValidateConfig()));

    QString filename = "/media/mapcrafter-projects/mapcrafter_test/world1.4.conf";
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    ui->textEdit->setPlainText(stream.readAll());
    file.close();

    this->filename = filename;
    this->filenameShort = QFileInfo(file).fileName();
    setWindowTitle(filenameShort);
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

void MainWindow::handleActionAbout()
{
    QString text;
    text += "Mapcrafter GUI version: " + QCoreApplication::applicationVersion() + "<br />";
    text += QString("Mapcrafter version: ") + mapcrafter::MAPCRAFTER_VERSION;
    if(!QString(mapcrafter::MAPCRAFTER_GITVERSION).isEmpty())
        text += QString(" (") + mapcrafter::MAPCRAFTER_GITVERSION + ")";
    text += "<br />";
    QMessageBox::information(this, "About", text);
}

void MainWindow::handleTextChanged()
{
    if(this->filename.isEmpty())
        setWindowTitle("Empty file*");
    else
        setWindowTitle(filenameShort + "*");
}

void MainWindow::handleValidateConfig()
{
    mapcrafter::config::MapcrafterConfig config;
    mapcrafter::config::ValidationMap validation;
    bool ok = config.parse(filename.toStdString(), validation);
    validation.log();

    ui->treeWidget->clear();
    auto sections = validation.getSections();
    for (auto section_it = sections.begin(); section_it != sections.end(); ++section_it) {
        auto messages = section_it->second.getMessages();
        if (messages.empty())
            continue;
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, QString::fromStdString(section_it->first));
        item->setExpanded(true);
        for (auto message_it = messages.begin(); message_it != messages.end(); ++message_it) {
            QTreeWidgetItem* other_item = new QTreeWidgetItem;
            QString icon = "dialog-information";
            if (message_it->getType() == mapcrafter::config::ValidationMessage::ERROR)
                icon = "dialog-error";
            else if (message_it->getType() == mapcrafter::config::ValidationMessage::WARNING)
                icon = "dialog-warning";
            other_item->setIcon(0, QIcon::fromTheme(icon));
            other_item->setText(0, QString::fromStdString(message_it->getMessage()));
            item->addChild(other_item);
        }
        ui->treeWidget->addTopLevelItem(item);
    }
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
