#ifndef OPERATIONRESULT_H
#define OPERATIONRESULT_H

#include <QString>

struct OperationResult
{
    QString bestUAV;
    QString bestHeli;
    QString bestPlane;

    double bestUAVCost = 0.0;
    double bestHeliCost = 0.0;
    double bestPlaneCost = 0.0;

    double bestUAVOperationCost = 0.0;
    double bestHeliOperationCost = 0.0;
    double bestPlaneOperationCost = 0.0;

    double bestUAVFuelCost = 0.0;
    double bestHeliFuelCost = 0.0;
    double bestPlaneFuelCost = 0.0;

    int bestUAVCount = 0;
    int bestHeliCount = 0;
    int bestPlaneCount = 0;

    double bestEpsilon = 0.0;
    double bestBeta = 0.0;

    double bestCost = 0.0;
    double bestOperationTime = 0.0;

    int bestUAVId = -1;
    int bestHeliId = -1;
    int bestPlaneId = -1;

    double operationCost = 0.0;
    double fuelCost = 0.0;
    double materialCost = 0.0;

    double riskValue = 0.0;
    QString riskLevel;
    QString riskColor;

    double searchArea = 0.0;

    double oilMass = 0.0;
    double dispersantMass = 0.0;
    double boomsMass = 0.0;
};

#endif // OPERATIONRESULT_H
