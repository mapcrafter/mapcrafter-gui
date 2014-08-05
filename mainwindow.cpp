#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <mapcraftercore/version.h>
#include <mapcraftercore/config/mapcrafterconfig.h>

#include <sstream>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QMessageBox>
#include <QSettings>
#include <QStringList>
#include <QTextStream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

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
    std::stringstream stream;
    stream << "<b>Mapcrafter GUI version:</b> " << QCoreApplication::applicationVersion().toStdString() << "<br />";
    stream << "<b>Mapcrafter version:</b> " << mapcrafter::MAPCRAFTER_VERSION;
    if(!QString(mapcrafter::MAPCRAFTER_GITVERSION).isEmpty())
        stream << " (" << mapcrafter::MAPCRAFTER_GITVERSION << ")";
    stream << "<br /><br />";

    fs::path mapcrafter_bin = mapcrafter::util::findExecutablePath();
    stream << "<b>Your home directory:</b> " << mapcrafter::util::findHomeDir().string().c_str() << "<br />";
    stream << "<b>Mapcrafter binary:</b> " << mapcrafter_bin.string().c_str() << "<br /><br />";

    mapcrafter::util::PathList resources = mapcrafter::util::findResourceDirs(mapcrafter_bin);
    stream << "<b>Resource directories:</b><ol>";
    for (size_t i = 0; i < resources.size(); i++)
        stream << "<li>" << BOOST_FS_ABSOLUTE1(resources[i]).string().c_str() << "</li>";
    if (resources.size() == 0)
        stream << "<li>Nothing found.</li>";
    stream << "</ol>";

    mapcrafter::util::PathList templates = mapcrafter::util::findTemplateDirs(mapcrafter_bin);
    stream << "<b>Template directories:</b><ol>";
    for (size_t i = 0; i < templates.size(); i++)
        stream << "<li>" << BOOST_FS_ABSOLUTE1(templates[i]).string().c_str() << "</li>";
    if (templates.size() == 0)
        stream << "<li>Nothing found.</li>";
    stream << "</ol>";

    mapcrafter::util::PathList textures = mapcrafter::util::findTextureDirs(mapcrafter_bin);
    stream << "<b>Texture directories:</b><ol>";
    for (size_t i = 0; i < textures.size(); i++)
        stream << "<li>" << BOOST_FS_ABSOLUTE1(textures[i]).string().c_str() << "</li>";
    if (textures.size() == 0)
        stream << "<li>Nothing found.</li>";
    stream << "</ol>";

    mapcrafter::util::PathList configs = mapcrafter::util::findLoggingConfigFiles(mapcrafter_bin);
    stream << "<b>Logging configuration file:</b><ol>";
    for (size_t i = 0; i < configs.size(); i++)
        stream << "<li>" << BOOST_FS_ABSOLUTE1(configs[i]).string().c_str() << "</li>";
    if (configs.size() == 0)
        stream << "<li>Nothing found.</li>";
    stream << "</ol>";
    QMessageBox::information(this, "About", stream.str().c_str());
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
    mapcrafter::config::ValidationMap validation = config.parse(filename.toStdString());
    validation.log();

    ui->validationWidget->setValidation(validation);

    if (validation.isEmpty())
        QMessageBox::information(this, "Configuration validation", "Configuration validation was successful!");
    else if (validation.isCritical())
        QMessageBox::critical(this, "Configuration validation", "There are critical errors in your configuration file!");
    else
        QMessageBox::information(this, "Configuration validation", "Just some small things about your configuration file.");
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
