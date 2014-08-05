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
    ui(new Ui::MainWindow),
    currentFile(""), currentFileDirty(false)
{
    ui->setupUi(this);

    readSettings();
    setCurrentFile("", false);

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(handleTextChanged()));

    connect(ui->buttonValidateConfig, SIGNAL(clicked()), this, SLOT(handleValidateConfig()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::open()
{
    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.isEmpty())
        return;
    loadFile(filename);
}

void MainWindow::save()
{
    if (currentFile.isEmpty())
        saveAs();
    else
        saveFile(currentFile);
}

void MainWindow::saveAs()
{
    QString filename = QFileDialog::getSaveFileName(this);
    if(filename.isEmpty())
        return;
    saveFile(filename);
}

void MainWindow::about()
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
    setCurrentFile(currentFile, true);
}

void MainWindow::handleValidateConfig()
{
    mapcrafter::config::MapcrafterConfig config;
    mapcrafter::config::ValidationMap validation = config.parse(currentFile.toStdString());
    validation.log();

    ui->validationWidget->setValidation(validation);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (currentFileDirty) {
        QMessageBox dialog(this);
        dialog.setWindowTitle("Save file");
        dialog.setText("Do you want to save the file?");
        dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        dialog.setDefaultButton(QMessageBox::Yes);
        int result = dialog.exec();
        if (result == QMessageBox::Yes)
            save();
        if (result == QMessageBox::Cancel)
            event->setAccepted(false);
    }
    writeSettings();
}

void MainWindow::loadFile(const QString& filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    ui->textEdit->setPlainText(stream.readAll());
    file.close();

    setCurrentFile(filename, false);
}

void MainWindow::saveFile(const QString& filename)
{
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream stream(&file);
    stream << ui->textEdit->toPlainText();
    file.close();

    setCurrentFile(filename, false);
}

void MainWindow::setCurrentFile(const QString &filename, bool dirty)
{
    currentFile = filename;
    currentFileDirty = dirty;
    QString shortName = QFileInfo(QFile(filename)).fileName();
    if (filename.isEmpty())
        shortName = "Empty file";
    if (dirty)
        shortName += "*";
    setWindowTitle(shortName + " - " + QCoreApplication::applicationName());
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
