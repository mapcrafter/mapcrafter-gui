#include "logwidget.h"

#include <QtCore/QDateTime>
#include <QtCore/QString>

void QtObjectLogSink::sink(const util::LogMessage& message) {
    emit gotMessage(message);
}

LogWidget::LogWidget(QWidget* parent)
    : QTreeWidget(parent) {
    QtObjectLogSink* sink = new QtObjectLogSink();
    util::Logging::getInstance().setSink("__qtobject__", sink);
    connect(sink, SIGNAL(gotMessage(util::LogMessage)), this, SLOT(addLogMessage(util::LogMessage)));

    /*
    LOG(INFO) << "Test info";
    LOG(NOTICE) << "Test notice";
    LOG(WARNING) << "Test warning";
    LOG(ERROR) << "Test error";
    LOG(EMERGENCY) << "Holy shit what's happening?!";
    */
}

void LogWidget::addLogMessage(const util::LogMessage& message) {
    QTreeWidgetItem* item = new QTreeWidgetItem(this);
    item->setText(0, QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    item->setText(1, QString::fromStdString(util::LogLevelHelper::levelToString(message.level)));
    item->setText(2, QString::fromStdString(message.logger));
    item->setText(3, QString::fromStdString(message.message));

    QString icon = "dialog-information";
    if (message.level == util::LogLevel::WARNING) {
        icon = "dialog-warning";
    } else if (message.level <= util::LogLevel::ERROR) {
        icon = "dialog-error";
    }
    item->setIcon(1, QIcon::fromTheme(icon));
    addTopLevelItem(item);
    scrollToBottom();
}
