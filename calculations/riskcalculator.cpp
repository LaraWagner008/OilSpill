#include "riskcalculator.h"

#include <QtMath>

RiskCalculator::RiskCalculator()
{
}

RiskCalculationResult RiskCalculator::calculate(
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
        const QVector<int>& graphPlaneCounts)
{
    RiskCalculationResult result;

    //--------------------------------------------------
    // Основной экологический риск
    //--------------------------------------------------

    double shoreDistanceNow =
            qMax(
                0.0,
                distanceShore
                -
                driftVelocity
                *
                timeLimit);

    double shoreFactor =
            1.0 -
            shoreDistanceNow /
            distanceShore;

    if(shoreFactor < 0.0)
        shoreFactor = 0.0;

    if(shoreFactor > 1.0)
        shoreFactor = 1.0;

    result.riskValue =
            (oilVolume / 1000.0)
            *
            shoreFactor
            *
            (driftVelocity / 10.0)
            *
            vulnerabilityCoefficient;

    if(result.riskValue > 1)
        result.riskValue = 1;

    if(result.riskValue < 0)
        result.riskValue = 0;

    if(result.riskValue <= 0.25)
    {
        result.riskLevel = "Низкий";
        result.riskColor = "#00ff55";
    }
    else if(result.riskValue <= 0.35)
    {
        result.riskLevel = "Средний";
        result.riskColor = "#ffff00";
    }
    else if(result.riskValue <= 0.6)
    {
        result.riskLevel = "Высокий";
        result.riskColor = "#ff8800";
    }
    else
    {
        result.riskLevel = "Критический";
        result.riskColor = "#ff0000";
    }

    //--------------------------------------------------
    // Риск для графика
    //--------------------------------------------------

    result.riskTimes.clear();
    result.riskValues.clear();

    for(int i = 0;
        i < graphOperationTimes.size();
        i++)
    {
        double t =
                graphOperationTimes[i];

        double shoreDistance =
                qMax(
                    0.0,
                    distanceShore -
                    driftVelocity * t);

        double shoreFactor =
                1.0 -
                shoreDistance /
                distanceShore;

        shoreFactor =
                qBound(
                    0.0,
                    shoreFactor,
                    1.0);

        double risk =
                (oilVolume / 1000.0)
                *
                shoreFactor
                *
                (driftVelocity / 10.0)
                *
                vulnerabilityCoefficient;

        risk =
                qBound(
                    0.0,
                    risk,
                    1.0);

        result.riskTimes.push_back(t);
        result.riskValues.push_back(risk);
    }

    //--------------------------------------------------
    // Альтернативные сценарии
    //--------------------------------------------------

    result.scenarios.clear();

    QVector<double> scenarioFractions;

    scenarioFractions
            << 0.25
            << 0.50
            << 0.75
            << 1.00;

    for(double fraction : scenarioFractions)
    {
        if(graphOperationTimes.isEmpty())
            continue;

        double targetTime =
                bestOperationTime * fraction;

        int bestScenarioIndex = 0;

        double minDelta = 1e100;

        for(int i = 0;
            i < graphOperationTimes.size();
            i++)
        {
            double delta =
                    qAbs(
                        graphOperationTimes[i]
                        -
                        targetTime);

            if(delta < minDelta)
            {
                minDelta = delta;
                bestScenarioIndex = i;
            }
        }

        RiskScenario s;

        s.time =
                graphOperationTimes[bestScenarioIndex];

        s.cost =
                graphCosts[bestScenarioIndex]
                *
                1000000.0;

        s.uav =
                graphUAVNames[bestScenarioIndex];

        s.heli =
                graphHeliNames[bestScenarioIndex];

        s.plane =
                graphPlaneNames[bestScenarioIndex];

        s.uavCount =
                graphUAVCounts[bestScenarioIndex];

        s.heliCount =
                graphHeliCounts[bestScenarioIndex];

        s.planeCount =
                graphPlaneCounts[bestScenarioIndex];

        s.risk =
                result.riskValues[bestScenarioIndex];

        if(s.risk <= 0.25)
        {
            s.level = "Низкий";
            s.color = "#00ff00";
        }
        else if(s.risk <= 0.35)
        {
            s.level = "Средний";
            s.color = "#ffff00";
        }
        else if(s.risk <= 0.6)
        {
            s.level = "Высокий";
            s.color = "#ff8800";
        }
        else
        {
            s.level = "Критический";
            s.color = "#ff0000";
        }

        result.scenarios.push_back(s);
    }

    return result;
}
