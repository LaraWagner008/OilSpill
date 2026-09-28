#ifndef ENVIRONMENTDATA_H
#define ENVIRONMENTDATA_H

struct EnvironmentData
{
    double vCurrent = 0.0;
    double dCurrent = 0.0;

    double vWind = 0.0;
    double dWind = 0.0;

    double kWind = 0.0;

    double driftVelocity = 0.0;
    double driftAngle = 0.0;

    double distanceShore = 0.0;
    double distanceBase = 0.0;

    double vulnerabilityCoefficient = 0.0;
};

#endif // ENVIRONMENTDATA_H
