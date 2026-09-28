#ifndef HELICOPTERCALCULATOR_H
#define HELICOPTERCALCULATOR_H

struct HelicopterCalculationResult
{
    double flightsCount = 0.0;

    double boomTime = 0.0;
    double flightTime = 0.0;
    double missionTime = 0.0;

    double helicopterCount = 0.0;
    double flightsPerHelicopter = 0.0;
    int realHelicopterTime = 0;

    double operationCost = 0.0;
    double fuelCost = 0.0;
    double totalCost = 0.0;
};

class HelicopterCalculator
{
public:
    HelicopterCalculator();

    HelicopterCalculationResult calculate(
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
            double fuelPriceHeli);
};

#endif // HELICOPTERCALCULATOR_H
