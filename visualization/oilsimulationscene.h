#ifndef OILSIMULATIONSCENE_H
#define OILSIMULATIONSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>

#include "../simulation/oilsimulation.h"

class OilSimulationScene
{
public:
    OilSimulationScene(
            QGraphicsScene *scene,
            OilSimulation *simulation);

    void drawWater();
    void drawOil();

    void updateCamera(
            QGraphicsView *view,
            double driftAngle);

    double cameraX() const;
    double cameraY() const;

private:
    QGraphicsScene *m_scene;
    OilSimulation *m_simulation;

    double m_cameraX = 0.0;
    double m_cameraY = 0.0;
};

#endif // OILSIMULATIONSCENE_H
