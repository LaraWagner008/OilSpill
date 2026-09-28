#ifndef CHARTSMANAGER_H
#define CHARTSMANAGER_H

#include <QVector>
#include <QWidget>

#include <QtCharts>

#include "../models/aircraftdata.h"
#include "../models/operationresult.h"
#include "../models/riskscenario.h"

QT_CHARTS_USE_NAMESPACE

class ChartsManager
{
public:
    ChartsManager();

    void createCharts(
            QWidget *parent,
            const AircraftData &aircraft,
            const OperationResult &operationResult,
            const QVector<double> &graphTimes,
            const QVector<double> &graphCosts,
            const QVector<RiskScenario> &scenarios);

    QChartView* uavChart() const;
    QChartView* heliChart() const;
    QChartView* planeChart() const;
    QChartView* costChart() const;

private:
    QWidget* createLegendItem(
            const QColor &color,
            const QString &text);

    QChartView *m_uavChart = nullptr;
    QChartView *m_heliChart = nullptr;
    QChartView *m_planeChart = nullptr;
    QChartView *m_costChart = nullptr;
};

#endif // CHARTSMANAGER_H
