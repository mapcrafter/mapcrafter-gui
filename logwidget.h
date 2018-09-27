#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <mapcraftercore-legacy/util.h>
#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <QtGui/QTreeWidget>

using namespace mapcrafter;

class QtObjectLogSink : public QObject, public util::LogSink {
    Q_OBJECT
public:
    virtual void sink(const util::LogMessage& message);

signals:
    void gotMessage(const util::LogMessage& message);
};

class LogWidget : public QTreeWidget {
    Q_OBJECT
public:
    explicit LogWidget(QWidget* parent = 0);

public slots:
    void addLogMessage(const util::LogMessage& message);
};

#endif // LOGWIDGET_H
