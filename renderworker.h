#ifndef RENDERWORKER_H
#define RENDERWORKER_H

#include <mapcraftercore/config/configsections/map.h>
#include <mapcraftercore/config/mapcrafterconfig.h>
#include <mapcraftercore/renderer/manager.h>

#include <QObject>

using namespace mapcrafter;

class QtObjectProgressHandler : public QObject, public util::DummyProgressHandler {
    Q_OBJECT

public:
    virtual void setMax(int max);
    virtual void setValue(int value);

signals:
    void maxChanged(int max);
    void valueChanged(int value);
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

    void progress1MaxChanged(int max);
    void progress1ValueChanged(int value);
    void progress2MaxChanged(int max);
    void progress2ValueChanged(int value);

    void renderMapsFinished();

public slots:
    void scanWorlds();
    void renderMaps();

protected slots:
    void handleProgress2MaxChanged(int max);
    void handleProgress2ValueChanged(int value);

protected:
    config::MapcrafterConfig config;
    renderer::RenderManager* manager;
};

#endif // RENDERWORKER_H
