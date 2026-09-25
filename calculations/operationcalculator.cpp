#include "operationcalculator.h"

OperationCalculator::OperationCalculator()
{
}

OperationCalculationResult OperationCalculator::calculate(
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
        double tLiquid)
{
    OperationCalculationResult result;

    result.operationCost =
            uavOperationCost
            +
            heliOperationCost
            +
            planeOperationCost;

    result.fuelCost =
            uavFuelCost
            +
            heliFuelCost
            +
            planeFuelCost;

    double dispersantMaterialCost =
            dispersantMass *
            dispersantPrice;

    double boomsMaterialCost =
            boomsMass *
            boomPrice;

    result.materialCost =
            dispersantMaterialCost
            +
            boomsMaterialCost;

    result.totalCost =
            result.operationCost
            +
            result.fuelCost
            +
            result.materialCost;

    result.operationTime =
            tDetect +
            tLiquid;

    return result;
}
