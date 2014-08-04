#include "validationwidget.h"

ValidationWidget::ValidationWidget(QWidget *parent) :
    QTreeWidget(parent)
{
}

void ValidationWidget::setValidation(const mapcrafter::config::ValidationMap& validation)
{
    clear();
    auto sections = validation.getSections();
    for (auto section_it = sections.begin(); section_it != sections.end(); ++section_it) {
        auto messages = section_it->second.getMessages();
        if (messages.empty())
            continue;
        QTreeWidgetItem* item = new QTreeWidgetItem(this);
        item->setText(0, QString::fromStdString(section_it->first));
        item->setExpanded(true);
        for (auto message_it = messages.begin(); message_it != messages.end(); ++message_it) {
            QTreeWidgetItem* other_item = new QTreeWidgetItem;
            QString icon = "dialog-information";
            if (message_it->getType() == mapcrafter::config::ValidationMessage::ERROR)
                icon = "dialog-error";
            else if (message_it->getType() == mapcrafter::config::ValidationMessage::WARNING)
                icon = "dialog-warning";
            other_item->setIcon(0, QIcon::fromTheme(icon));
            other_item->setText(0, QString::fromStdString(message_it->getMessage()));
            item->addChild(other_item);
        }
        addTopLevelItem(item);
    }
}
