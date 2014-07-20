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
    void handleActionOpen();
    void handleActionSave();
    void handleActionSaveAs();
    void handleActionAbout();

    void handleTextChanged();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void readSettings();
    void writeSettings();

    Ui::MainWindow *ui;

    QString filename, filenameShort;
};

#endif // MAINWINDOW_H
