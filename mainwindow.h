#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QSplitter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void open();
    void save();
    void saveAs();
    void about();

    void handleTextChanged();

    void handleValidateConfig();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void loadFile(const QString& filename);
    void saveFile(const QString& filename);
    void setCurrentFile(const QString& filename, bool dirty = false);

    void readSettings();
    void writeSettings();

    Ui::MainWindow *ui;

    QString currentFile;
    bool currentFileDirty;
};

#endif // MAINWINDOW_H
