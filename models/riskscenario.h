#ifndef RISKSCENARIO_H
#define RISKSCENARIO_H

#include <QString>

struct RiskScenario
{
    QString level;
    QString color;

    double risk;
    double time;
    double cost;

    QString uav;
    QString heli;
    QString plane;

    int uavCount;
    int heliCount;
    int planeCount;
};

#endif // RISKSCENARIO_H
