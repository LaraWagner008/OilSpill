#ifndef RISKCALCULATOR_H
#define RISKCALCULATOR_H

#include <QVector>
#include <QString>

#include "models/riskscenario.h"

struct RiskCalculationResult
{
    double riskValue = 0.0;
    QString riskLevel;
    QString riskColor;

    QVector<double> riskTimes;
    QVector<double> riskValues;

    QVector<RiskScenario> scenarios;
};

class RiskCalculator
{
public:
    RiskCalculator();

    RiskCalculationResult calculate(
            double oilVolume,
            double distanceShore,
            double driftVelocity,
            double vulnerabilityCoefficient,
            double timeLimit,
            double bestOperationTime,
            const QVector<double>& graphOperationTimes,
            const QVector<double>& graphCosts,
            const QVector<QString>& graphUAVNames,
            const QVector<QString>& graphHeliNames,
            const QVector<QString>& graphPlaneNames,
            const QVector<int>& graphUAVCounts,
            const QVector<int>& graphHeliCounts,
            const QVector<int>& graphPlaneCounts);
};

#endif // RISKCALCULATOR_H
