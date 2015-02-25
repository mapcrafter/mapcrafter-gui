#include "mapselectionwidget.h"

MapSelectionWidget::MapSelectionWidget(QWidget* parent)
    : QTreeWidget(parent) {
    addMap("world_day");
}

void MapSelectionWidget::addMap(const QString& name) {
    QTreeWidgetItem* item = new QTreeWidgetItem(this);
    item->setText(0, name);
    item->setCheckState(1, Qt::Checked);
    //item->setText(1, "top-left");
    item->setCheckState(2, Qt::Checked);
    //item->setText(2, "top-right");
    item->setCheckState(3, Qt::Checked);
    //item->setText(3, "bottom-right");
    item->setCheckState(4, Qt::Checked);
    //item->setText(4, "bottom-left");
    addTopLevelItem(item);
}
