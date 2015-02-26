#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "inisyntaxhighlighter.h"

#include <mapcraftercore/version.h>
#include <mapcraftercore/config/mapcrafterconfig.h>

#include <sstream>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
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
    currentTab(0), currentlyRendering(false), currentFile(""), currentFileDirty(false),
    manager(NULL)
{
    ui->setupUi(this);

    readSettings();
    setCurrentFile("", false);

    // configure file menu
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    recentFilesSeparator = new QAction(this);
    recentFilesSeparator->setSeparator(true);
    ui->menuFile->addAction(recentFilesSeparator);
    for (int i = 0; i < MAX_RECENT_FILES; i++) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        ui->menuFile->addAction(recentFileActions[i]);
        connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
    updateRecentFiles();

    // configure config file tab
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    ui->textEdit->setFont(font);
    INISyntaxHighlighter* highlighter = new INISyntaxHighlighter(ui->textEdit->document());

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(handleTextChanged()));
    connect(ui->buttonValidateConfig, SIGNAL(clicked()), this, SLOT(handleValidateConfig()));

    // configure tab widget
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleTabChanged(int)));

    // configure rendering tab
    QMenu* menu = new QMenu(this);
    QAction* action1 = new QAction("auto", this);
    QAction* action2 = new QAction("force", this);
    QAction* action3 = new QAction("skip", this);
    connect(action1, SIGNAL(triggered()), this, SLOT(handleSetRenderBehaviorsTo()));
    connect(action2, SIGNAL(triggered()), this, SLOT(handleSetRenderBehaviorsTo()));
    connect(action3, SIGNAL(triggered()), this, SLOT(handleSetRenderBehaviorsTo()));
    menu->addAction(action1);
    menu->addAction(action2);
    menu->addAction(action3);
    ui->buttonSetAllRenderBehaviorsTo->setMenu(menu);
    ui->buttonSetAllRenderBehaviorsTo->setPopupMode(QToolButton::InstantPopup);

    worker = new RenderWorker();
    worker->moveToThread(&thread);
    thread.start();
    connect(ui->buttonRender, SIGNAL(clicked()), this, SLOT(handleRender()));
    connect(this, SIGNAL(startRendering()), worker, SLOT(scanWorlds()));
    connect(worker, SIGNAL(scanWorldsFinished()), worker, SLOT(renderMaps()));
    connect(worker, SIGNAL(renderMapsFinished()), this, SLOT(handleRenderFinished()));
    connect(worker, SIGNAL(progress1MaxChanged(int)), ui->progress1, SLOT(setMaximum(int)));
    connect(worker, SIGNAL(progress1ValueChanged(int)), ui->progress1, SLOT(setValue(int)));
    connect(worker, SIGNAL(progress2MaxChanged(int)), ui->progress2, SLOT(setMaximum(int)));
    connect(worker, SIGNAL(progress2ValueChanged(int)), ui->progress2, SLOT(setValue(int)));

    // configure command line rendering tab
    connect(ui->customMapcrafterBinary, SIGNAL(toggled(bool)), this, SLOT(updateMapcrafterCommand()));
    connect(ui->mapcrafterBinary, SIGNAL(textChanged(QString)), this, SLOT(updateMapcrafterCommand()));
    connect(ui->threads, SIGNAL(valueChanged(int)), this, SLOT(updateMapcrafterCommand()));
    connect(ui->mapsSkipAll, SIGNAL(toggled(bool)), this, SLOT(updateMapcrafterCommand()));
    connect(ui->mapsSkip, SIGNAL(textChanged(QString)), this, SLOT(updateMapcrafterCommand()));
    connect(ui->mapsAuto, SIGNAL(textChanged(QString)), this, SLOT(updateMapcrafterCommand()));
    connect(ui->mapsForce, SIGNAL(textChanged(QString)), this, SLOT(updateMapcrafterCommand()));
    updateMapcrafterCommand();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newFile() {
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
            return;
    }
    ui->textEdit->setText("");
    setCurrentFile("", false);
}

void MainWindow::open()
{
    // TODO handle this differently
    if (currentlyRendering) {
        QMessageBox::critical(this, "TODO", "Not allowed at the moment!");
        return;
    }

    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.isEmpty())
        return;
    loadFile(filename);
}

void MainWindow::openRecentFile() {
    // TODO handle this differently
    if (currentlyRendering) {
        QMessageBox::critical(this, "TODO", "Not allowed at the moment!");
        return;
    }

    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
        loadFile(action->data().toString());
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

void MainWindow::updateMapcrafterCommand()
{
    QString mapcrafter;
    if (ui->customMapcrafterBinary->isChecked()) {
        mapcrafter = ui->mapcrafterBinary->text();
    } else {
        fs::path mapcrafterDir = mapcrafter::util::findExecutableMapcrafterDir();
        mapcrafter = QString::fromStdString((mapcrafterDir / "mapcrafter").string());
        ui->mapcrafterBinary->setText(mapcrafter);
    }

    if (currentFile.isEmpty() || mapcrafter.isEmpty()) {
        ui->mapcrafterCommand->setText("");
        return;
    }

    QStringList args;
    args << "\"" + mapcrafter + "\"";
    args << "-c" << "\"" + currentFile + "\"";
    args << "-j" << QString::number(ui->threads->value());

    if (ui->mapsSkipAll->isChecked()) {
        args << "-r";
    } else if (!ui->mapsSkip->text().isEmpty()) {
        args << "-s" << ui->mapsSkip->text();
    }

    if (!ui->mapsAuto->text().isEmpty())
        args << "-a" << ui->mapsAuto->text();
    if (!ui->mapsForce->text().isEmpty())
        args << "-f" << ui->mapsForce->text();

    ui->mapcrafterCommand->setText(args.join(" "));
}

void MainWindow::handleTextChanged()
{
    setCurrentFile(currentFile, true);
}

void MainWindow::handleValidateConfig()
{
    // TODO validate config file as you edit it in the text field
    int status = validateConfig();
    // TODO output message?

    /*
    if (status == 0) {
        QMessageBox::information(this, "Configuration validation", "Validation successful.");
    } else if (status == 1) {
        QMessageBox::warning(this, "Configuration validation", "Validation successful, but some minor problems appeared.");
    } else {
        QMessageBox::critical(this, "Configuration validation", "Validation not successful.");
        return;
    }
    */
}

void MainWindow::handleTabChanged(int tab) {
    // make sure config is valid when switching to rendering tab
    if (currentTab != 1 && tab == 1) {
        int status = validateConfig();
        if (status == 1) {
            QMessageBox dialog(this);
            dialog.setWindowTitle("Configuration validation");
            dialog.setIcon(QMessageBox::Warning);
            dialog.setText("There are warnings about your configuration file. Do still you want to proceed to render your map(s)?");
            dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            dialog.setDefaultButton(QMessageBox::Yes);
            int result = dialog.exec();
            if (result == QMessageBox::No) {
                ui->tabWidget->setCurrentIndex(0);
                return;
            }
        } else if (status == 2) {
            QMessageBox::critical(this, "Configuration validation", "You have to fix the errors in your configuration file before you can proceed to render your map(s).");
            ui->tabWidget->setCurrentIndex(0);
            return;
        }
    }

    // TODO handle this differently
    if (currentTab == 1 && tab != 1 && currentlyRendering) {
        QMessageBox::critical(this, "TODO", "Can't change the tab while rendering!");
        ui->tabWidget->setCurrentIndex(1);
        return;
    }

    currentTab = tab;
}

void MainWindow::handleSetRenderBehaviorsTo() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action != nullptr) {
        renderer::RenderBehavior behavior = renderer::RenderBehavior::AUTO;
        if (action->text() == "force")
            behavior = renderer::RenderBehavior::FORCE;
        else if (action->text() == "skip")
            behavior = renderer::RenderBehavior::SKIP;
        else if (action->text() != "auto")
            return;
        ui->inputRenderBehaviors->setRenderBehaviors(renderer::RenderBehaviorMap(behavior), config);
    }
}

void MainWindow::handleRender()
{
    if (manager)
        delete manager;
    currentlyRendering = true;
    manager = new renderer::RenderManager(config);
    manager->initialize();
    manager->setThreadCount(ui->inputThreadCount->value());
    manager->setRenderBehaviors(ui->inputRenderBehaviors->getRenderBehaviors());

    worker->setConfig(config);
    worker->setRenderManager(manager);
    emit startRendering();

    ui->labelRenderBehaviors->setEnabled(false);
    ui->buttonSetAllRenderBehaviorsTo->setEnabled(false);
    ui->inputRenderBehaviors->setEnabled(false);
    ui->labelThreadCount->setEnabled(false);
    ui->inputThreadCount->setEnabled(false);
    ui->buttonRender->setEnabled(false);
}

void MainWindow::handleRenderFinished() {
    currentlyRendering = false;
    ui->labelRenderBehaviors->setEnabled(true);
    ui->buttonSetAllRenderBehaviorsTo->setEnabled(true);
    ui->inputRenderBehaviors->setEnabled(true);
    ui->labelThreadCount->setEnabled(true);
    ui->inputThreadCount->setEnabled(true);
    ui->buttonRender->setEnabled(true);

    ui->progress1->setMaximum(1);
    ui->progress1->setValue(0);
    ui->progress2->setMaximum(1);
    ui->progress2->setValue(0);
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

    int status = validateConfig();
    // goto rendering tab to config tab if config file contains errors
    if (status == 2 && ui->tabWidget->currentIndex() == 1)
        ui->tabWidget->setCurrentIndex(0);
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

    if (filename.isEmpty())
        return;

    QSettings settings;
    QStringList recentFiles = settings.value("recentFiles").toStringList();
    recentFiles.removeAll(filename);
    recentFiles.prepend(filename);
    while (recentFiles.size() > MAX_RECENT_FILES)
        recentFiles.removeLast();
    settings.setValue("recentFiles", recentFiles);
    updateRecentFiles();
    updateMapcrafterCommand();
}

void MainWindow::updateRecentFiles()
{
    QSettings settings;
    QStringList recentFiles = settings.value("recentFiles").toStringList();
    int numRecentFiles = qMin(recentFiles.size(), MAX_RECENT_FILES);
    for (int i = 0; i < numRecentFiles; i++) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(QFile(recentFiles[i])).fileName());
        recentFileActions[i]->setText(text);
        recentFileActions[i]->setData(recentFiles[i]);
        recentFileActions[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MAX_RECENT_FILES; j++)
        recentFileActions[j]->setVisible(false);
    recentFilesSeparator->setVisible(numRecentFiles != 0);
}

int MainWindow::validateConfig() {
    config = config::MapcrafterConfig();
    config::ValidationMap validation = config.parse(currentFile.toStdString());
    //validation.log();

    ui->validationWidget->setValidation(validation);
    ui->inputRenderBehaviors->setRenderBehaviors(renderer::RenderBehaviorMap(renderer::RenderBehavior::AUTO), config);

    if (validation.isEmpty()) {
        return 0;
    } else if (validation.isCritical()) {
        return 2;
    } else {
        return 1;
    }
}

void MainWindow::readSettings()
{
    QSettings settings;

    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
    ui->splitter->restoreState(settings.value("MainWindow/splitterState").toByteArray());

    if (settings.contains("mapcrafterBinary")) {
        ui->customMapcrafterBinary->setChecked(true);
        ui->mapcrafterBinary->setText(settings.value("mapcrafterBinary").toString());
        updateMapcrafterCommand();
    }
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());
    settings.setValue("MainWindow/splitterState", ui->splitter->saveState());

    if (ui->customMapcrafterBinary->isChecked()) {
        settings.setValue("mapcrafterBinary", ui->mapcrafterBinary->text());
    } else {
        settings.remove("mapcrafterBinary");
    }
}

const int MainWindow::MAX_RECENT_FILES;
