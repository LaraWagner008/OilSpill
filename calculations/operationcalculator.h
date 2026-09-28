#ifndef OPERATIONCALCULATOR_H
#define OPERATIONCALCULATOR_H

struct OperationCalculationResult
{
    double operationCost = 0.0;
    double fuelCost = 0.0;
    double materialCost = 0.0;

    double totalCost = 0.0;
    double operationTime = 0.0;
};

class OperationCalculator
{
public:
    OperationCalculator();

    OperationCalculationResult calculate(
            double uavOperationCost,
            double heliOperationCost,
            double planeOperationCost,
            double uavFuelCost,
            double heliFuelCost,
            double planeFuelCost,
            double dispersantMass,
            double dispersantPrice,
            double boomsMass,
            double boomPrice,
            double tDetect,
            double tLiquid);
};

#endif // OPERATIONCALCULATOR_H
