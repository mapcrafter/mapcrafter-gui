#include "inisyntaxhighlighter.h"

INISyntaxHighlighter::INISyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent) {
}

void INISyntaxHighlighter::highlightBlock(const QString& text) {
    QTextCharFormat commentFormat, sectionFormat, keyFormat, valueSpecialFormat;
    commentFormat.setForeground(Qt::darkGreen);
    sectionFormat.setForeground(Qt::red);
    keyFormat.setForeground(Qt::darkCyan);
    valueSpecialFormat.setForeground(Qt::blue);

    QStringList lines = text.split("\n");
    int startIndex = 0, endIndex = 0;
    for (int i = 0; i < lines.size(); i++) {
        QString line = lines[i];
        endIndex = startIndex + line.size();
        if (line.trimmed().startsWith("#"))
            setFormat(startIndex, endIndex, commentFormat);
        else if (line.trimmed().startsWith("["))
            setFormat(startIndex, endIndex, sectionFormat);
        else {
            int mid = line.indexOf('=');
            if (mid == -1)
                mid = line.size();
            setFormat(startIndex, startIndex + mid + 1, keyFormat);

            QString key = line.mid(0, mid);
            QString value = line.mid(mid+1);

            bool isNum;
            value.toDouble(&isNum);
            if (isNum || value == "true" || value == "false")
                setFormat(startIndex + mid + 1, endIndex, valueSpecialFormat);
        }
        startIndex = endIndex;
    }
}
