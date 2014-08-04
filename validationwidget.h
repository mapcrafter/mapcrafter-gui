#ifndef VALIDATIONWIDGET_H
#define VALIDATIONWIDGET_H

#include <mapcraftercore/config/validation.h>

#include <QTreeWidget>

class ValidationWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit ValidationWidget(QWidget *parent = 0);

signals:

public slots:
    void setValidation(const mapcrafter::config::ValidationMap& validation);
};

#endif // VALIDATIONWIDGET_H
