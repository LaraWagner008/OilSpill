#include "environmentcalculator.h"

#include <cmath>

EnvironmentCalculator::EnvironmentCalculator()
{
}

EnvironmentCalculationResult EnvironmentCalculator::calculate(
        double vCurrent,
        double dCurrent,
        double vWind,
        double dWind,
        double kWind)
{
    EnvironmentCalculationResult result;

    double sx =
            vCurrent
            * sin(dCurrent * M_PI / 180.0)
            +
            kWind
            * vWind
            * sin(dWind * M_PI / 180.0);

    double sy =
            vCurrent
            * cos(dCurrent * M_PI / 180.0)
            +
            kWind
            * vWind
            * cos(dWind * M_PI / 180.0);

    result.driftVelocity =
            sqrt(sx * sx + sy * sy);

    result.driftAngle =
            atan2(sy, sx);

    return result;
}
