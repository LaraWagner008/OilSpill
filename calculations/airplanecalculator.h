#ifndef AIRPLANECALCULATOR_H
#define AIRPLANECALCULATOR_H

struct AirplaneCalculationResult
{
    double areaAtArrival = 0.0;
    double areaAtArrivalKm2 = 0.0;

    double dispersantVolume = 0.0;
    double dispersantMass = 0.0;

    double sprayCapacity = 0.0;
    double sprayTime = 0.0;

    double flightsCount = 0.0;
    double flightTime = 0.0;
    double missionTime = 0.0;

    double airplaneCount = 0.0;
    double flightsPerAirplane = 0.0;
    int realAirplaneTime = 0;

    double operationCost = 0.0;
    double fuelCost = 0.0;
    double totalCost = 0.0;
};

class AirplaneCalculator
{
public:
    AirplaneCalculator();

    AirplaneCalculationResult calculate(
            double currentArea,
            double kSpread,
            double tDetect,
            double beta,
            double tLiquid,
            double sprayRate,
            double density,
            double vPlane,
            double sprayWidth,
            double distanceBase,
            double loadingTime,
            double capacityPlane,
            double fuelConsumptionPlane,
            double costPlane,
            double fuelPricePlane);
};

#endif // AIRPLANECALCULATOR_H
