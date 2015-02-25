#ifndef MAPSELECTIONWIDGET_H
#define MAPSELECTIONWIDGET_H

#include <QtGui/QTreeWidget>

class MapSelectionWidget : public QTreeWidget {
    Q_OBJECT
public:
    explicit MapSelectionWidget(QWidget* parent = 0);

protected:
    void addMap(const QString& name);
};

#endif // MAPSELECTIONWIDGET_H
