#include "renderworker.h"

#include <mapcraftercore/util.h>

void QtObjectProgressHandler::setMax(int max) {
    emit maxChanged(max);
    AbstractOutputProgressHandler::setMax(max);
}

void QtObjectProgressHandler::setValue(int value) {
    emit valueChanged(value);
    AbstractOutputProgressHandler::setValue(value);
}

void QtObjectProgressHandler::update(double percentage, double average_speed, int eta) {
    emit statsChanged(percentage, average_speed, eta);
}

RenderWorker::RenderWorker(QObject *parent)
    : QObject(parent), thread_count(1), qt_progress(new QtObjectProgressHandler())
{
}

RenderWorker::~RenderWorker()
{
    delete qt_progress;
}

void RenderWorker::setConfig(const config::MapcrafterConfig& config) {
    this->config = config;
}

void RenderWorker::setRenderManager(renderer::RenderManager* manager) {
    this->manager = manager;
}

void RenderWorker::setThreadCount(int thread_count) {
    this->thread_count = thread_count;
}

void RenderWorker::scanWorlds() {
    assert(manager);

    emit labelMapsProgressChanged("Scanning worlds...");
    emit progressMapsMaxChanged(0);
    LOG(INFO) << "Scanning worlds...";
    manager->scanWorlds();
    emit labelMapsProgressChanged("Finished scanning worlds.");
    emit progressMapsMaxChanged(1);
    emit progressMapsValueChanged(0);
    emit scanWorldsFinished();
}

void RenderWorker::renderMaps() {
    assert(manager);

    auto required_maps = manager->getRequiredMaps();

    int progress1_max = 0;
    int progress1_value = 0;
    for (auto map_it = required_maps.begin(); map_it != required_maps.end(); ++map_it)
        progress1_max += map_it->second.size();
    emit progressMapsMaxChanged(progress1_max);
    emit progressMapsValueChanged(progress1_value);

    util::Logging::getInstance().setSinkLogProgress("__output__", true);

    for (auto map_it = required_maps.begin(); map_it != required_maps.end(); ++map_it) {
        config::MapSection map_config = config.getMap(map_it->first);

        auto required_rotations = map_it->second;
        for (auto rotation_it = required_rotations.begin();
                rotation_it != required_rotations.end(); ++rotation_it) {
            emit labelMapsProgressChanged(QString("Map '%1' - Rotation %2").arg(QString::fromStdString(map_it->first)).arg(*rotation_it));
            emit progressMapsValueChanged(progress1_value++);
            LOG(INFO) << "Rendering rotation " << *rotation_it << " of map '" << map_it->first << "'.";

            qt_progress = new QtObjectProgressHandler();
            connect(qt_progress, SIGNAL(maxChanged(int)), this, SLOT(handleTilesProgressMaxChanged(int)));
            connect(qt_progress, SIGNAL(valueChanged(int)), this, SLOT(handleTilesProgressValueChanged(int)));
            connect(qt_progress, SIGNAL(statsChanged(double, double, int)), this, SLOT(handleTilesProgressStatsChanged(double, double, int)));

            util::MultiplexingProgressHandler progress;
            util::LogOutputProgressHandler* log_output = new util::LogOutputProgressHandler();
            progress.addHandler(log_output);
            progress.addHandler(qt_progress);
            manager->renderMap(map_config.getShortName(), *rotation_it, thread_count, &progress);

            delete log_output;
            delete qt_progress;
        }
    }

    emit progressMapsValueChanged(progress1_max);
    emit renderMapsFinished();
}

void RenderWorker::handleTilesProgressMaxChanged(int max) {
    if (!qt_progress)
        return;
    emit labelTilesProgressLeftChanged(QString("%1 of %2 tiles rendered").arg(qt_progress->getValue()).arg(qt_progress->getMax()));
    emit progressTilesMaxChanged(max);
}

void RenderWorker::handleTilesProgressValueChanged(int value) {
    if (!qt_progress)
        return;
    emit labelTilesProgressLeftChanged(QString("%1 of %2 tiles rendered").arg(qt_progress->getValue()).arg(qt_progress->getMax()));
    emit progressTilesValueChanged(value);
}

void RenderWorker::handleTilesProgressStatsChanged(double percentage, double average_speed, int eta) {
    if (!qt_progress)
        return;
    emit labelTilesProgressCenterChanged(QString("%1 tiles/s").arg(QString::number(average_speed, 'f', 1)));
    emit labelTilesProgressRightChanged(QString("ETA: %1").arg(QString::fromStdString(util::format_eta(eta))));
}
