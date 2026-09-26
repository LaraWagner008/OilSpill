#ifndef ENVIRONMENTCALCULATOR_H
#define ENVIRONMENTCALCULATOR_H

struct EnvironmentCalculationResult
{
    double driftVelocity = 0.0;
    double driftAngle = 0.0;
};

class EnvironmentCalculator
{
public:
    EnvironmentCalculator();

    EnvironmentCalculationResult calculate(
            double vCurrent,
            double dCurrent,
            double vWind,
            double dWind,
            double kWind);
};

#endif // ENVIRONMENTCALCULATOR_H
