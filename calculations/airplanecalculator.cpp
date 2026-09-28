#include "airplanecalculator.h"

#include <cmath>

AirplaneCalculator::AirplaneCalculator()
{
}

AirplaneCalculationResult AirplaneCalculator::calculate(
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
        double fuelPricePlane)
{
    AirplaneCalculationResult result;

    result.areaAtArrival =
            currentArea
            +
            M_PI *
            kSpread *
            1000000.0 *
            (
                tDetect
                +
                beta * tLiquid
            );

    result.areaAtArrivalKm2 =
            result.areaAtArrival /
            1000000.0;

    result.dispersantVolume =
            result.areaAtArrivalKm2 *
            sprayRate *
            100.0;

    result.dispersantMass =
            result.dispersantVolume *
            density /
            1000.0;

    result.sprayCapacity =
            vPlane *
            sprayWidth *
            sprayRate *
            100.0;

    result.sprayTime =
            result.areaAtArrival /
            result.sprayCapacity;

    result.flightsCount =
            ceil(
                result.dispersantMass /
                capacityPlane);

    result.flightTime =
            2.0 *
            distanceBase /
            vPlane;

    result.missionTime =
            result.flightTime
            +
            result.sprayTime
            +
            loadingTime;

    result.airplaneCount =
            ceil(
                result.flightsCount *
                result.missionTime /
                tLiquid);

    result.flightsPerAirplane =
            ceil(
                result.flightsCount /
                result.airplaneCount);

    result.realAirplaneTime =
            result.flightsPerAirplane *
            result.missionTime;

    result.operationCost =
            result.airplaneCount *
            result.realAirplaneTime *
            costPlane;

    result.fuelCost =
            result.airplaneCount *
            result.realAirplaneTime *
            fuelConsumptionPlane *
            fuelPricePlane;

    result.totalCost =
            result.operationCost +
            result.fuelCost;

    return result;
}
