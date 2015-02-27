#ifndef RENDERWORKER_H
#define RENDERWORKER_H

#include <mapcraftercore/config/configsections/map.h>
#include <mapcraftercore/config/mapcrafterconfig.h>
#include <mapcraftercore/renderer/manager.h>

#include <QObject>

using namespace mapcrafter;

class QtObjectProgressHandler : public QObject, public util::AbstractOutputProgressHandler {
    Q_OBJECT

public:
    virtual void setMax(int max);
    virtual void setValue(int value);

    virtual void update(double percentage, double average_speed, int eta);

signals:
    void maxChanged(int max);
    void valueChanged(int value);
    void statsChanged(double percentage, double average_speed, int eta);
};

class RenderWorker : public QObject
{
    Q_OBJECT
public:
    explicit RenderWorker(QObject *parent = 0);
    ~RenderWorker();

    void setConfig(const config::MapcrafterConfig& config);
    void setRenderManager(renderer::RenderManager* manager);

signals:
    void scanWorldsFinished();

    void labelMapsProgressChanged(QString string);
    void progressMapsMaxChanged(int max);
    void progressMapsValueChanged(int value);

    void labelTilesProgressLeftChanged(QString string);
    void labelTilesProgressCenterChanged(QString string);
    void labelTilesProgressRightChanged(QString string);
    void progressTilesMaxChanged(int max);
    void progressTilesValueChanged(int value);

    void renderMapsFinished();

public slots:
    void scanWorlds();
    void renderMaps();

protected slots:
    void handleTilesProgressMaxChanged(int max);
    void handleTilesProgressValueChanged(int value);
    void handleTilesProgressStatsChanged(double percentage, double average_speed, int eta);

protected:
    config::MapcrafterConfig config;
    renderer::RenderManager* manager;

    QtObjectProgressHandler* qt_progress;
};

#endif // RENDERWORKER_H
