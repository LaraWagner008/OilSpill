#include "uavcalculator.h"

#include <cmath>
#include <QtMath>

UAVCalculator::UAVCalculator()
{
}

UAVCalculationResult UAVCalculator::calculate(
        double driftVelocity,
        double tDetect,
        double flightHeight,
        double viewingAngle,
        double rangeUAV,
        double distanceBase,
        double vUAV,
        double fuelWeightUAV,
        double fuelConsumptionUAV)
{
    UAVCalculationResult result;

    result.flightTime =
            fuelWeightUAV /
            fuelConsumptionUAV;

    result.searchRadius =
            driftVelocity *
            tDetect;

    result.stripWidth =
            2.0 *
            flightHeight *
            tan(
                qDegreesToRadians(
                    viewingAngle / 2.0));

    result.thetaSpiral =
            (2.0 * M_PI * result.searchRadius)
            /
            result.stripWidth;

    result.totalSearchLength =
            (result.stripWidth /
             (4.0 * M_PI))
            *
            (
                result.thetaSpiral *
                sqrt(
                    1.0 +
                    result.thetaSpiral *
                    result.thetaSpiral)
                +
                log(
                    result.thetaSpiral +
                    sqrt(
                        1.0 +
                        result.thetaSpiral *
                        result.thetaSpiral))
            );

    result.searchLengthOneUAV =
            rangeUAV
            -
            2.0 *
            distanceBase;

    result.flightsCount =
            ceil(
                result.totalSearchLength
                /
                result.searchLengthOneUAV);

    result.flightMissionTime =
            (
                result.searchLengthOneUAV
                +
                2.0 *
                distanceBase
            )
            /
            vUAV;

    result.uavCount =
            ceil(
                result.flightsCount *
                result.flightMissionTime
                /
                tDetect);

    result.flightsPerUAV =
            ceil(
                result.flightsCount /
                result.uavCount);

    result.realUAVTime =
            result.flightsPerUAV *
            result.flightMissionTime;

    return result;
}
