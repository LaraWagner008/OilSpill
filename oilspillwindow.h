#ifndef OILSPILLWINDOW_H
#define OILSPILLWINDOW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QLabel>

#include <QtCharts>

#include <QSqlDatabase>
#include <QSqlQuery>

#include "database/databasemanager.h"

QT_CHARTS_USE_NAMESPACE

struct OilParticle
{
    double x;
    double y;

    double vx;
    double vy;
};

struct RiskScenario
{
    QString level;
    QString color;

    double risk;
    double time;
    double cost;

    QString uav;
    QString heli;
    QString plane;

    int uavCount;
    int heliCount;
    int planeCount;
};

class OilSpillWindow : public QGraphicsView
{
    Q_OBJECT

public:
    explicit OilSpillWindow(QString waterType,
                            double timeLimit,
                            double oilVolume,
                            QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void updateSimulation();

private:
    QVector<RiskScenario> scenarios;
    QWidget* createLegendItem(
            const QColor &color,
            const QString &text);
    //----------------------------------
    // существующая модель
    //----------------------------------
    QVector<double> graphTimes;
    QVector<double> graphCosts;
    QVector<double> graphEpsilons;
    QVector<double> graphBetas;

    QVector<int> graphUAVCounts;
    QVector<int> graphHeliCounts;
    QVector<int> graphPlaneCounts;
    QVector<QString> graphUAVNames;
    QVector<QString> graphHeliNames;
    QVector<QString> graphPlaneNames;
    QVector<double> riskTimes;
    QVector<double> riskValues;

    QVector<double> graphOperationTimes;

    void drawScene();
    void drawWater();
    void drawOil();
    void drawHUD();

    double degToRad(double deg);

    //----------------------------------
    // БД
    //----------------------------------

    //void connectDatabase();
    void loadConditions();
    void calculateOperation();

    //----------------------------------
    // графики
    //----------------------------------

    void createCharts();

    //----------------------------------
    // расчеты
    //----------------------------------

    double calculateSearchArea();

    double calculateUAVFlights(
            double searchArea,
            double productivity,
            double flightTime);

    double calculateUAVTime(
            double searchArea,
            double productivity,
            double speed);

    //----------------------------------
    // сцена
    //----------------------------------

    QGraphicsScene *scene;
    QTimer *timer;

    QLabel *hud;

    //----------------------------------
    // окна результатов
    //----------------------------------

    QLabel *resultPanel;

    QChartView *uavChart;
    QChartView *heliChart;
    QChartView *planeChart;

    QChartView *costChart;

    //----------------------------------
    // БД
    //----------------------------------


    // БЫЛО QSqlDatabase db;
    DatabaseManager databaseManager; //СТАЛО

    //----------------------------------
    // входные данные
    //----------------------------------

    QString waterType;

    double timeLimit;
    double oilVolume;

    //----------------------------------
    // параметры среды
    //----------------------------------

    double vCurrent;
    double dCurrent;

    double vWind;
    double dWind;

    double kWind;

    double driftVelocity;
    double driftAngle;

    double oilThickness;
    double minThickness;

    double currentArea;
    double maxArea;

    double perimeter;

    double kSpread;
    double initialThickness;
    double distanceShore;
    double distanceBase;
    double vulnerabilityCoefficient;
    bool spreadingFinished;

    //----------------------------------
    // координаты
    //----------------------------------

    double worldX;
    double worldY;

    double cameraX;
    double cameraY;

    //----------------------------------
    // результаты расчета
    //----------------------------------

    QString bestUAV;
    QString bestHeli;
    QString bestPlane;

    double bestUAVCost;
    double bestHeliCost;
    double bestPlaneCost;

    double bestUAVOperationCost;
    double bestHeliOperationCost;
    double bestPlaneOperationCost;

    double bestUAVFuelCost;
    double bestHeliFuelCost;
    double bestPlaneFuelCost;

    QStringList uavNames;
    QStringList heliNames;
    QStringList planeNames;

    QVector<double> uavCosts;
    QVector<double> heliCosts;
    QVector<double> planeCosts;

    QVector<int> uavCounts;
    QVector<int> heliCounts;
    QVector<int> planeCounts;

    int bestUAVCount;
    int bestHeliCount;
    int bestPlaneCount;

    double bestEpsilon = 0.0;
    double bestBeta = 0.0;



    double bestCost;

    //----------------------------------
    // части затрат
    //----------------------------------

    double operationCost;
    double fuelCost;
    double materialCost;

    double riskValue;
    QString riskLevel;
    QString riskColor;

    // данные лучшей комбинации

    int bestUAVId;
    int bestHeliId;
    int bestPlaneId;

    double bestOperationTime;

    double searchArea;

    double oilMass;
    double dispersantMass;
    double boomsMass;
    QVector<OilParticle> particles;
};

#endif
