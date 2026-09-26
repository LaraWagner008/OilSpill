#ifndef HUDRENDERER_H
#define HUDRENDERER_H

#include <QLabel>

#include "../simulation/oilsimulation.h"

class HUDRenderer
{
public:
    HUDRenderer(
            QLabel *hud,
            OilSimulation *simulation);

    void draw(
            double driftVelocity,
            double driftAngle);

private:
    QLabel *m_hud;
    OilSimulation *m_simulation;
};

#endif // HUDRENDERER_H
