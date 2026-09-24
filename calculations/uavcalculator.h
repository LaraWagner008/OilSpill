#ifndef UAVCALCULATOR_H
#define UAVCALCULATOR_H

struct UAVCalculationResult
{
    double flightTime = 0.0;

    double searchRadius = 0.0;
    double stripWidth = 0.0;
    double thetaSpiral = 0.0;
    double totalSearchLength = 0.0;

    double searchLengthOneUAV = 0.0;
    double flightsCount = 0.0;
    double flightMissionTime = 0.0;

    double uavCount = 0.0;
    double flightsPerUAV = 0.0;
    int realUAVTime = 0;

    double operationCost = 0.0;
    double fuelCost = 0.0;
    double totalCost = 0.0;
};

class UAVCalculator
{
public:
    UAVCalculator();

    UAVCalculationResult calculate(
            double driftVelocity,
            double tDetect,
            double flightHeight,
            double viewingAngle,
            double rangeUAV,
            double distanceBase,
            double vUAV,
            double fuelWeightUAV,
            double fuelConsumptionUAV,
            double costUAV,
            double fuelPriceUAV);
};

#endif // UAVCALCULATOR_H
