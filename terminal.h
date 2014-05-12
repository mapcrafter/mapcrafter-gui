#ifndef TERMINAL_H
#define TERMINAL_H

#include <QTextEdit>
#include <QStringList>
#include <QProcess>
#include <QVector>
#include <QString>

class Terminal : public QTextEdit
{
    Q_OBJECT
public:
    explicit Terminal(QWidget *parent = 0);

    void start(const QString& program, const QStringList& args, bool clear_before = true);

    void clear();

public slots:
    void readData();
    
private:
    void updateTerminal();

    QProcess* process;

    QVector<QString> lines;
};

#endif // TERMINAL_H
