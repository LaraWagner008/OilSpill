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
#include "models/environmentdata.h"
#include "models/aircraftdata.h"
#include "models/operationresult.h"
#include "calculations/uavcalculator.h"
#include "calculations/helicoptercalculator.h"
#include "calculations/airplanecalculator.h"
#include "calculations/operationcalculator.h"

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

    EnvironmentData environment;

    double oilThickness;
    double minThickness;

    double currentArea;
    double maxArea;

    double perimeter;

    double kSpread;
    double initialThickness;

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
    AircraftData aircraft;

    OperationResult operationResult;

    UAVCalculator uavCalculator;
    HelicopterCalculator helicopterCalculator;
    AirplaneCalculator airplaneCalculator;
    OperationCalculator operationCalculator;

    QVector<OilParticle> particles;
};

#endif
