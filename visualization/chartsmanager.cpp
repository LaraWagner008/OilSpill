#include "chartsmanager.h"

#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QScatterSeries>
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QColor>

ChartsManager::ChartsManager()
{
}

void ChartsManager::createCharts(
        QWidget *parent,
        const AircraftData &aircraft,
        const OperationResult &operationResult,
        const QVector<double> &graphTimes,
        const QVector<double> &graphCosts,
        const QVector<RiskScenario> &scenarios)
{
    //--------------------------------------------------
    // БПЛА
    //--------------------------------------------------

    QBarSet *uavSet =
            new QBarSet("БПЛА");

    for(int i = 0;
        i < aircraft.uavCosts.size();
        i++)
    {
        *uavSet <<
                aircraft.uavCosts[i] /
                1000000.0;
    }

    uavSet->setColor(
                QColor(35, 182, 175));

    QBarSeries *uavSeries =
            new QBarSeries();

    uavSeries->append(uavSet);

    QChart *uav =
            new QChart();

    uav->addSeries(uavSeries);

    uav->setTitle("<b>БПЛА</b>");

    uav->setBackgroundPen(QPen(Qt::NoPen));
    uav->setBackgroundBrush(
                QBrush(QColor(255, 255, 255)));

    uav->setPlotAreaBackgroundVisible(true);

    uav->setPlotAreaBackgroundBrush(
                QBrush(QColor(255, 255, 255)));

    QStringList cats1;

    for(int i = 0;
        i < aircraft.uavNames.size();
        i++)
    {
        cats1 <<
            aircraft.uavNames[i]
            + "\n("
            + QString::number(
                aircraft.uavCounts[i])
            + ")";
    }

    QBarCategoryAxis *axis1 =
            new QBarCategoryAxis();

    axis1->append(cats1);

    uav->layout()->setContentsMargins(
                0, 0, 0, 0);

    uav->createDefaultAxes();

    uav->setAxisX(
                axis1,
                uavSeries);

    QFont axisFont;
    axisFont.setPointSize(5);

    axis1->setLabelsFont(axisFont);

    uav->legend()->hide();

    m_uavChart =
            new QChartView(uav);

    m_uavChart->setStyleSheet(
        "QChartView { border: none; background: transparent; }");

    QFrame *uavFrame =
            new QFrame(parent);

    uavFrame->setGeometry(
                0, 750, 660, 250);

    uavFrame->setStyleSheet(
        "QFrame{"
        "    background-color: rgba(4,14,28,120);"
        "    border: 1px solid rgba(0,255,255,120);"
        "    border-radius: 15px;"
        "}");

    QVBoxLayout *uavFrameLayout =
            new QVBoxLayout(uavFrame);

    uavFrameLayout->setContentsMargins(
                15, 15, 15, 15);

    uavFrameLayout->addWidget(
                m_uavChart);

    //--------------------------------------------------
    // ВК
    //--------------------------------------------------

    QBarSet *heliSet =
            new QBarSet("ВК");

    for(int i = 0;
        i < aircraft.heliCosts.size();
        i++)
    {
        *heliSet <<
                aircraft.heliCosts[i] /
                1000000.0;
    }

    heliSet->setColor(
                QColor(0, 214, 217));

    QBarSeries *heliSeries =
            new QBarSeries();

    heliSeries->append(heliSet);

    QChart *heli =
            new QChart();

    heli->addSeries(
                heliSeries);

    heli->setTitle("<b>ВК</b>");

    heli->setBackgroundPen(QPen(Qt::NoPen));

    heli->setBackgroundBrush(
                QBrush(QColor(255, 255, 255)));

    heli->setPlotAreaBackgroundVisible(true);

    heli->setPlotAreaBackgroundBrush(
                QBrush(QColor(255, 255, 255)));

    QStringList cats2;

    for(int i = 0;
        i < aircraft.heliNames.size();
        i++)
    {
        cats2 <<
            aircraft.heliNames[i]
            + "\n("
            + QString::number(
                aircraft.heliCounts[i])
            + ")";
    }

    QBarCategoryAxis *axis2 =
            new QBarCategoryAxis();

    axis2->append(cats2);

    heli->layout()->setContentsMargins(
                0, 0, 0, 0);

    heli->createDefaultAxes();

    heli->setAxisX(
                axis2,
                heliSeries);

    heli->legend()->hide();

    m_heliChart =
            new QChartView(heli);

    m_heliChart->setStyleSheet(
        "QChartView { border: none; background: transparent; }");

    QFrame *heliFrame =
            new QFrame(parent);

    heliFrame->setGeometry(
                660, 750, 630, 250);

    heliFrame->setStyleSheet(
        "QFrame{"
        "    background-color: rgba(4,14,28,120);"
        "    border: 1px solid rgba(0,255,255,120);"
        "    border-radius: 15px;"
        "}");

    QVBoxLayout *heliFrameLayout =
            new QVBoxLayout(heliFrame);

    heliFrameLayout->setContentsMargins(
                15, 15, 15, 15);

    heliFrameLayout->addWidget(
                m_heliChart);

    //--------------------------------------------------
    // АК
    //--------------------------------------------------

    QBarSet *planeSet =
            new QBarSet("АК");

    for(int i = 0;
        i < aircraft.planeCosts.size();
        i++)
    {
        *planeSet <<
                aircraft.planeCosts[i] /
                1000000.0;
    }

    planeSet->setColor(
                QColor(82, 253, 255));

    QBarSeries *planeSeries =
            new QBarSeries();

    planeSeries->append(
                planeSet);

    QChart *plane =
            new QChart();

    plane->addSeries(
                planeSeries);

    plane->setTitle("<b>АК</b>");

    plane->setBackgroundPen(QPen(Qt::NoPen));

    plane->setBackgroundBrush(
                QBrush(QColor(255, 255, 255)));

    plane->setPlotAreaBackgroundVisible(true);

    plane->setPlotAreaBackgroundBrush(
                QBrush(QColor(255, 255, 255)));

    QStringList cats3;

    for(int i = 0;
        i < aircraft.planeNames.size();
        i++)
    {
        cats3 <<
            aircraft.planeNames[i]
            + "\n("
            + QString::number(
                aircraft.planeCounts[i])
            + ")";
    }

    QBarCategoryAxis *axis3 =
            new QBarCategoryAxis();

    axis3->append(cats3);

    plane->layout()->setContentsMargins(
                0, 0, 0, 0);

    plane->createDefaultAxes();

    plane->setAxisX(
                axis3,
                planeSeries);

    plane->legend()->hide();

    m_planeChart =
            new QChartView(plane);

    m_planeChart->setStyleSheet(
        "QChartView { border: none; background: transparent; }");

    QFrame *planeFrame =
            new QFrame(parent);

    planeFrame->setGeometry(
                1290, 750, 620, 250);

    planeFrame->setStyleSheet(
        "QFrame{"
        "    background-color: rgba(4,14,28,120);"
        "    border: 1px solid rgba(0,255,255,120);"
        "    border-radius: 15px;"
        "}");

    QVBoxLayout *planeFrameLayout =
            new QVBoxLayout(planeFrame);

    planeFrameLayout->setContentsMargins(
                15, 15, 15, 15);

    planeFrameLayout->addWidget(
                m_planeChart);

    //--------------------------------------------------
    // ОБЩИЙ ГРАФИК
    //--------------------------------------------------

    QLineSeries *series =
            new QLineSeries();

    for(int i = 0;
        i < graphTimes.size();
        i++)
    {
        series->append(
            graphTimes[i],
            graphCosts[i]);
    }

    series->setColor(
                QColor(255,80,180));

    QChart *cost =
            new QChart();

    cost->addSeries(series);

    for(int i = 0;
        i < scenarios.size();
        i++)
    {
        QScatterSeries *riskPoint =
                new QScatterSeries();

        riskPoint->setMarkerShape(
                    QScatterSeries::MarkerShapeCircle);

        riskPoint->setMarkerSize(16);

        riskPoint->setColor(
                    QColor(scenarios[i].color));

        riskPoint->setBorderColor(Qt::white);

        riskPoint->setName(
                    scenarios[i].level);

        riskPoint->append(
                    scenarios[i].time,
                    scenarios[i].cost /
                    1000000.0);

        cost->addSeries(riskPoint);
    }

    cost->legend()->hide();

    cost->layout()->setContentsMargins(
                0, 0, 0, 0);

    cost->createDefaultAxes();

    QValueAxis *axisX =
            qobject_cast<QValueAxis*>(
                cost->axisX());

    QValueAxis *axisY =
            qobject_cast<QValueAxis*>(
                cost->axisY());

    axisX->setRange(
                0,
                operationResult.bestOperationTime *
                1.1);

    double maxY = 0.0;

    for(double chartCost : graphCosts)
    {
        maxY =
                qMax(
                    maxY,
                    chartCost);
    }

    for(const auto &s : scenarios)
    {
        maxY =
                qMax(
                    maxY,
                    s.cost / 1000000.0);
    }

    axisY->setRange(
                0,
                maxY * 1.15);

    cost->setTitle(
                "Затраты операции");

    QPen pen;

    pen.setColor(
                QColor(0, 105, 217));

    pen.setWidth(3);

    pen.setStyle(Qt::SolidLine);

    series->setPen(pen);

    m_costChart =
            new QChartView(cost);

    m_costChart->setStyleSheet(
        "QChartView { border: none; background: transparent; }");

    QFrame *costFrame =
            new QFrame(parent);

    costFrame->setGeometry(
                0, 400, 600, 350);

    costFrame->setStyleSheet(
        "QFrame{"
        "    background-color: rgba(4,14,28,120);"
        "    border: 1px solid rgba(0,255,255,120);"
        "    border-radius: 15px;"
        "}");

    QVBoxLayout *frameLayout =
            new QVBoxLayout(costFrame);

    QHBoxLayout *legendLayout =
            new QHBoxLayout;

    legendLayout->setSpacing(15);

    legendLayout->addWidget(
        createLegendItem(
            QColor("#00ff00"),
            "Низкий риск"));

    legendLayout->addWidget(
        createLegendItem(
            QColor("#ffff00"),
            "Средний риск"));

    legendLayout->addWidget(
        createLegendItem(
            QColor("#ff8800"),
            "Высокий риск"));

    legendLayout->addWidget(
        createLegendItem(
            QColor("#ff0000"),
            "Критический риск"));

    frameLayout->setContentsMargins(
                10, 10, 10, 10);

    frameLayout->addLayout(
                legendLayout);

    frameLayout->addWidget(
                m_costChart);
}

QWidget* ChartsManager::createLegendItem(
        const QColor &color,
        const QString &text)
{
    QWidget *widget =
            new QWidget;

    QHBoxLayout *layout =
            new QHBoxLayout(widget);

    QLabel *colorLabel =
            new QLabel;

    colorLabel->setFixedSize(
                14, 14);

    colorLabel->setStyleSheet(
        "background-color: "
        + color.name()
        + "; border-radius: 7px;");

    QLabel *textLabel =
            new QLabel(text);

    layout->setContentsMargins(
                0, 0, 0, 0);

    layout->addWidget(colorLabel);
    layout->addWidget(textLabel);

    return widget;
}

QChartView* ChartsManager::uavChart() const
{
    return m_uavChart;
}

QChartView* ChartsManager::heliChart() const
{
    return m_heliChart;
}

QChartView* ChartsManager::planeChart() const
{
    return m_planeChart;
}

QChartView* ChartsManager::costChart() const
{
    return m_costChart;
}
