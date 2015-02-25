#include "renderworker.h"

#include <mapcraftercore/util.h>

void QtObjectProgressHandler::setMax(int max) {
    emit maxChanged(max);
    DummyProgressHandler::setMax(max);
}

void QtObjectProgressHandler::setValue(int value) {
    emit valueChanged(value);
    DummyProgressHandler::setValue(value);
}

RenderWorker::RenderWorker(QObject *parent) : QObject(parent)
{

}

RenderWorker::~RenderWorker()
{

}

void RenderWorker::setConfig(const config::MapcrafterConfig& config) {
    this->config = config;
}

void RenderWorker::setRenderManager(renderer::RenderManager* manager) {
    this->manager = manager;
}

void RenderWorker::scanWorlds() {
    assert(manager);

    emit progress1MaxChanged(0);
    LOG(INFO) << "Scanning worlds...";
    manager->scanWorlds();
    emit progress1MaxChanged(1);
    emit progress1ValueChanged(0);
    emit scanWorldsFinished();
}

void RenderWorker::renderMaps() {
    assert(manager);

    auto required_maps = manager->getRequiredMaps();

    int progress1_max = 0;
    int progress1_value = 0;
    for (auto map_it = required_maps.begin(); map_it != required_maps.end(); ++map_it)
        progress1_max += map_it->second.size();
    emit progress1MaxChanged(progress1_max);
    emit progress1ValueChanged(progress1_value);

    QtObjectProgressHandler* qt_progress = new QtObjectProgressHandler();
    connect(qt_progress, SIGNAL(maxChanged(int)), this, SLOT(handleProgress2MaxChanged(int)));
    connect(qt_progress, SIGNAL(valueChanged(int)), this, SLOT(handleProgress2ValueChanged(int)));
    util::Logging::getInstance().setSinkLogProgress("__output__", true);

    for (auto map_it = required_maps.begin(); map_it != required_maps.end(); ++map_it) {

        config::MapSection map_config = config.getMap(map_it->first);

        manager->initializeMap(map_it->first);

        auto required_rotations = map_it->second;
        for (auto rotation_it = required_rotations.begin();
                rotation_it != required_rotations.end(); ++rotation_it) {
            emit progress1ValueChanged(progress1_value++);
            LOG(INFO) << "Rendering rotation " << *rotation_it << " of map '" << map_it->first << "'.";

            util::MultiplexingProgressHandler progress;
            util::LogOutputProgressHandler* log_output = new util::LogOutputProgressHandler();
            progress.addHandler(log_output);
            progress.addHandler(qt_progress);
            manager->renderMap(map_config.getShortName(), *rotation_it, &progress);

            delete log_output;

        }
    }

    emit progress1ValueChanged(progress1_max);
    emit renderMapsFinished();
}

void RenderWorker::handleProgress2MaxChanged(int max) {
    emit progress2MaxChanged(max);
}

void RenderWorker::handleProgress2ValueChanged(int value) {
    emit progress2ValueChanged(value);
}
