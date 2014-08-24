#ifndef INISYNTAXHIGHLIGHTER_H
#define INISYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class INISyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit INISyntaxHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString& text);

};

#endif // INISYNTAXHIGHLIGHTER_H
