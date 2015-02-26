#include "renderbehaviorswidget.h"

#include <QtGui/QHeaderView>

RenderBehaviorsWidget::RenderBehaviorsWidget(QWidget* parent)
    : QTreeWidget(parent) {
    QStringList labels;
    labels << "Map" << "tl" << "tr" << "br" << "bl";
    setHeaderLabels(labels);
    setColumnCount(labels.size());
    header()->setStretchLastSection(false);
    header()->setResizeMode(0, QHeaderView::Stretch);

    setSelectionMode(SelectionMode::NoSelection);
}

void RenderBehaviorsWidget::setRenderBehaviors(const renderer::RenderBehaviorMap& behaviors,
                                               const config::MapcrafterConfig& config) {
    clear();
    combo_boxes.clear();

    auto maps = config.getMaps();
    for (auto map_it = maps.begin(); map_it != maps.end(); ++map_it) {
        std::string map = map_it->getShortName();

        QTreeWidgetItem* item = new QTreeWidgetItem(this);
        item->setText(0, QString::fromStdString(map));
        addTopLevelItem(item);

        combo_boxes.erase(map);
        for (int rotation = 0; rotation < 4; rotation++) {
            QComboBox& combo = combo_boxes[map][rotation];
            combo.addItem("auto");
            combo.addItem("force");
            combo.addItem("skip");
            combo.setEnabled(map_it->getRotations().count(rotation));
            renderer::RenderBehavior behavior = behaviors.getRenderBehavior(map, rotation);
            if (behavior == renderer::RenderBehavior::FORCE)
                combo.setCurrentIndex(1);
            else if (behavior == renderer::RenderBehavior::SKIP)
                combo.setCurrentIndex(2);
            setItemWidget(item, rotation + 1, &combo);
        }
    }
}

renderer::RenderBehaviorMap RenderBehaviorsWidget::getRenderBehaviors() const {
    renderer::RenderBehaviorMap render_behaviors;
    for (auto it = combo_boxes.begin(); it != combo_boxes.end(); ++it) {
        std::string map = it->first;
        for (int rotation = 0; rotation < 4; rotation++) {
            QString str = it->second.at(rotation).currentText();
            renderer::RenderBehavior behavior = renderer::RenderBehavior::AUTO;
            if (str == "force")
                behavior = renderer::RenderBehavior::FORCE;
            else if (str == "skip")
                behavior = renderer::RenderBehavior::SKIP;
            render_behaviors.setRenderBehavior(map, rotation, behavior);
        }
    }
    return render_behaviors;
}
