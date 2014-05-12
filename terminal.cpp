#include "terminal.h"

#include <QDebug>

Terminal::Terminal(QWidget *parent) :
    QTextEdit(parent), process(NULL) {
    lines.append("");
    updateTerminal();

    setReadOnly(true);
    setLineWrapMode(QTextEdit::WidgetWidth);

    process = new QProcess(this);
    process->setReadChannelMode(QProcess::MergedChannels);

    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readData()));
}

void Terminal::start(const QString& program, const QStringList& args, bool clear_before) {
    if( process->state() != QProcess::NotRunning)
        return;

    clear();
    process->start(program, args);
}

void Terminal::clear() {
    lines.clear();
    lines.append("");
    updateTerminal();
}

void Terminal::readData() {
    QString raw = process->readAllStandardOutput();
    qDebug() << "Got data: " << raw;

    bool last_changed = false;

    QStringList lines_read = raw.split("\n");
    for(int i = 0; i < lines_read.size(); i++) {
        QString line = lines_read[i];
        //qDebug() << line << i+1 << "/" << lines_read.size();
        if(line.startsWith("\033[K") && lines.size() != 0) {
            lines[lines.size() - 1] = line.replace("\033[K", "");
            last_changed = true;
        } else {
            if(last_changed)
                lines.append("");
            lines[lines.size() - 1] += line;
            if(i < lines_read.size() - 1)
                lines.append("");
            last_changed = false;
        }
    }

    updateTerminal();
}

void Terminal::updateTerminal() {
    QString html = "";
    for(int i = 0; i < lines.size(); i++) {
        html += lines[i];
        if(i < lines.size() - 1)
            html += "\n";
    }

    setHtml("<body style='color: rgb(230, 230, 230); background-color: black; font-weight: bold; font-family: Monospace'><pre>" + html + "</pre></body>");
}
