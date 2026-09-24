#ifndef AIRCRAFTDATA_H
#define AIRCRAFTDATA_H

#include <QString>

struct AircraftData
{
    QStringList uavNames;
    QStringList heliNames;
    QStringList planeNames;

    QVector<double> uavCosts;
    QVector<double> heliCosts;
    QVector<double> planeCosts;

    QVector<int> uavCounts;
    QVector<int> heliCounts;
    QVector<int> planeCounts;
};

#endif // AIRCRAFTDATA_H
