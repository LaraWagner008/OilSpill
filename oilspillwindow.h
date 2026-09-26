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
#include "simulation/oilsimulation.h"
#include "visualization/oilsimulationscene.h"
#include "visualization/hudrenderer.h"
#include "visualization/chartsmanager.h"
#include "models/riskscenario.h"
#include "calculations/riskcalculator.h"

QT_CHARTS_USE_NAMESPACE

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
    void updateResultPanel();

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
    HUDRenderer *hudRenderer;

    //----------------------------------
    // окна результатов
    //----------------------------------

    QLabel *resultPanel;

    ChartsManager chartsManager;

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

    double minThickness;
    double kSpread;
    double initialThickness;
/*
    double oilThickness;
    double currentArea;
    double maxArea;
    double perimeter;
    bool spreadingFinished;
    double worldX;
    double worldY;
*/
    //----------------------------------
    // координаты
    //----------------------------------

//    double cameraX;
  //  double cameraY;

    //----------------------------------
    // результаты расчета
    //----------------------------------
    AircraftData aircraft;

    OperationResult operationResult;

    UAVCalculator uavCalculator;
    HelicopterCalculator helicopterCalculator;
    AirplaneCalculator airplaneCalculator;
    OperationCalculator operationCalculator;
    RiskCalculator riskCalculator;

    OilSimulation oilSimulation;
    OilSimulationScene *oilScene;

};

#endif
