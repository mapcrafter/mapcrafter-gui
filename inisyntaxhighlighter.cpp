#include "inisyntaxhighlighter.h"

INISyntaxHighlighter::INISyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent) {
}

void INISyntaxHighlighter::highlightBlock(const QString& text) {
    QTextCharFormat commentFormat, sectionFormat, optionFormat;
    commentFormat.setForeground(Qt::blue);
    sectionFormat.setForeground(Qt::red);
    optionFormat.setForeground(Qt::darkGreen);

    QStringList lines = text.split("\n");
    int startIndex = 0, endIndex = 0;
    for (int i = 0; i < lines.size(); i++) {
        QString line = lines[i];
        endIndex = startIndex + line.size();
        if (line.trimmed().startsWith("#"))
            setFormat(startIndex, endIndex, commentFormat);
        else if (line.trimmed().startsWith("["))
            setFormat(startIndex, endIndex, sectionFormat);
        else
            setFormat(startIndex, endIndex, optionFormat);
        startIndex = endIndex;
    }
}
