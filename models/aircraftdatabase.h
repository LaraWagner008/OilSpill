#ifndef AIRCRAFTDATABASE_H
#define AIRCRAFTDATABASE_H

#include <QString>
#include <QVector>

struct UAVData
{
    int id = -1;
    QString name;

    double speed = 0.0;
    double range = 0.0;
    double fuelWeight = 0.0;
    double fuelConsumption = 0.0;
    double cost = 0.0;
};

struct HelicopterData
{
    int id = -1;
    QString name;

    double speed = 0.0;
    double range = 0.0;
    double capacity = 0.0;
    double fuelWeight = 0.0;
    double fuelConsumption = 0.0;
    double cost = 0.0;
};

struct AirplaneData
{
    int id = -1;
    QString name;

    double speed = 0.0;
    double range = 0.0;
    double capacity = 0.0;
    double sprayWidth = 0.0;
    double fuelWeight = 0.0;
    double fuelConsumption = 0.0;
    double cost = 0.0;
};

struct AircraftDatabase
{
    QVector<UAVData> uavs;
    QVector<HelicopterData> helicopters;
    QVector<AirplaneData> airplanes;
};

#endif // AIRCRAFTDATABASE_H
