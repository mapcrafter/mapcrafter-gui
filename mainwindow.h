#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "renderworker.h"

#include <mapcraftercore/config/mapcrafterconfig.h>
#include <mapcraftercore/renderer/manager.h>

#include <QtCore/QThread>
#include <QCloseEvent>
#include <QMainWindow>
#include <QSplitter>

using namespace mapcrafter;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void startRendering();

public slots:
    void newFile();
    void open();
    void openRecentFile();
    void save();
    void saveAs();
    void about();
    void updateMapcrafterCommand();

    void handleTextChanged();

    void handleValidateConfig();
    void handleRender();
    void handleRenderFinished();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void loadFile(const QString& filename);
    void saveFile(const QString& filename);
    void setCurrentFile(const QString& filename, bool dirty = false);
    void updateRecentFiles();

    void readSettings();
    void writeSettings();

    Ui::MainWindow *ui;

    QString currentFile;
    bool currentFileDirty;

    static const int MAX_RECENT_FILES = 5;
    QAction* recentFilesSeparator;
    QAction* recentFileActions[MAX_RECENT_FILES];

    config::MapcrafterConfig config;
    renderer::RenderManager* manager;
    RenderWorker* worker;
    QThread thread;
};

#endif // MAINWINDOW_H
