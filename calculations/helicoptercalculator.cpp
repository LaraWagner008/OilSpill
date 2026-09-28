#include "helicoptercalculator.h"

#include <cmath>

HelicopterCalculator::HelicopterCalculator()
{
}

HelicopterCalculationResult HelicopterCalculator::calculate(
        double boomsMass,
        double capacityHeli,
        double perimeter,
        double boomSpeed,
        double loadingTime,
        double distanceBase,
        double vHeli,
        double beta,
        double tLiquid,
        double fuelConsumptionHeli,
        double costHeli,
        double fuelPriceHeli)
{
    HelicopterCalculationResult result;

    result.flightsCount =
            ceil(
                boomsMass /
                capacityHeli);

    result.boomTime =
            (perimeter / 1000.0) /
            boomSpeed;

    result.flightTime =
            2.0 *
            distanceBase /
            vHeli;

    result.missionTime =
            result.flightTime
            +
            result.boomTime
            +
            loadingTime;

    result.helicopterCount =
            ceil(
                result.flightsCount *
                result.missionTime /
                (beta * tLiquid));

    result.flightsPerHelicopter =
            ceil(
                result.flightsCount /
                result.helicopterCount);

    result.realHelicopterTime =
            result.flightsPerHelicopter *
            result.missionTime;

    result.operationCost =
            result.helicopterCount *
            result.realHelicopterTime *
            costHeli;

    result.fuelCost =
            result.helicopterCount *
            result.realHelicopterTime *
            fuelConsumptionHeli *
            fuelPriceHeli;

    result.totalCost =
            result.operationCost +
            result.fuelCost;

    return result;
}
