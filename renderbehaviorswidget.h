#ifndef RENDERBEHAVIORSWIDGET_H
#define RENDERBEHAVIORSWIDGET_H

#include <mapcraftercore-legacy/config/configsections/map.h>
#include <mapcraftercore-legacy/config/mapcrafterconfig.h>
#include <mapcraftercore-legacy/renderer/manager.h>

#include <QtGui/QComboBox>
#include <QtGui/QTreeWidget>
#include <array>
#include <map>
#include <string>

using namespace mapcrafter;

class RenderBehaviorsWidget : public QTreeWidget {
    Q_OBJECT
public:
    explicit RenderBehaviorsWidget(QWidget* parent = 0);

    void setRenderBehaviors(const renderer::RenderBehaviors& behaviors,
                            const config::MapcrafterConfig& config);
    renderer::RenderBehaviors getRenderBehaviors() const;

protected:
    std::map<std::string, std::array<QComboBox, 4> > combo_boxes;
};

#endif // RENDERBEHAVIORSWIDGET_H
