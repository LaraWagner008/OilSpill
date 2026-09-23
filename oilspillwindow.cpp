#include "oilspillwindow.h"

#include <QGraphicsEllipseItem>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QtMath>
#include <QPainter>
#include <QtCharts>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>

#include <QLineSeries>
#include <QScatterSeries>

#include <QDebug>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QWidget>

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>




OilSpillWindow::OilSpillWindow(QString waterType,
                               double timeLimit,
                               double oilVolume,
                               QWidget *parent)
    : QGraphicsView(parent)
{
    this->waterType = waterType;
    this->timeLimit = timeLimit;
    this->oilVolume = oilVolume;
    bestCost = 1e18;
    // ЮЫЛО connectDatabase();
    databaseManager.connect(); //СТАЛО

    loadConditions();
    bestUAVCount = 0;
    bestHeliCount = 0;
    bestPlaneCount = 0;

    bestUAVId = -1;
    bestHeliId = -1;
    bestPlaneId = -1;

    bestUAVOperationCost = 0;
    bestHeliOperationCost = 0;
    bestPlaneOperationCost = 0;

    bestUAVFuelCost = 0;
    bestHeliFuelCost = 0;
    bestPlaneFuelCost = 0;

    operationCost = 0;
    fuelCost = 0;
    materialCost = 0;

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);

    setWindowTitle("Мониторинг нефтяного разлива");

    setFixedSize(1920,1080);

    setFrameShape(QFrame::NoFrame);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setStyleSheet(
                "background-color: rgb(2,6,14);"
                "border:none;"
                );

    // =====================================================
    // ПАРАМЕТРЫ
    // =====================================================



    minThickness = 0.001;

    // =====================================================
    // DRIFT
    // =====================================================

    double sx =
            vCurrent * sin(degToRad(dCurrent))
            +
            kWind * vWind * sin(degToRad(dWind));

    double sy =
            vCurrent * cos(degToRad(dCurrent))
            +
            kWind * vWind * cos(degToRad(dWind));

    driftVelocity =
            sqrt(sx*sx + sy*sy);

    driftAngle =
            atan2(sy,sx);



    // =====================================================
    // AREA
    // =====================================================


    currentArea =
            oilVolume /
            (initialThickness * 1e-3);

    // Smax = V / hmin

    maxArea =
            oilVolume /
            (minThickness * 1e-3);

    oilThickness =
            initialThickness;

    // perimeter

    perimeter =
            2 * M_PI *
            sqrt(currentArea / M_PI);

    // =====================================================
    // SCENE
    // =====================================================

    scene = new QGraphicsScene(this);

    setScene(scene);

    scene->setSceneRect(
                -200000,
                -200000,
                400000,
                400000);

    worldX = 0;
    worldY = 0;

    cameraX = 0;
    cameraY = 0;

    spreadingFinished = false;
    bestCost = 1e100;

    bestUAVCost = 1e100;
    bestHeliCost = 1e100;
    bestPlaneCost = 1e100;
    // =====================================================
    // PARTICLES
    // =====================================================


    int particleCount =
            qBound(
                300,
                int(oilVolume * 4),
                3000);

    //for(int i=0; i<950; i++)
    for(int i=0; i<particleCount; i++)
    {
        double angle =
                degToRad(qrand()%360);

        // меньше радиус -> компактное круглое пятно

        double radius =
                qSqrt(qrand()%1000) * 1.8;

        OilParticle p;

        p.x = cos(angle) * radius;
        p.y = sin(angle) * radius;

        p.vx = 0;
        p.vy = 0;

        particles.push_back(p);
    }

    // =====================================================
    // HUD PANEL
    // =====================================================

    hud = new QLabel(this);

    hud->setGeometry(
                1520,
                0,
                390,
                500);

    hud->setStyleSheet(

                "QLabel{"

                "background-color: rgba(4,14,28,120);"

                "border: 1px solid rgba(0,255,255,120);"

                "border-radius: 22px;"

                "padding:22px;"

                "color: rgb(160,255,255);"

                "font-family:'DejaVu Sans';"

                "font-size:15px;"

                "}"

                );
    //--------------------------------------------------
    // РЕЗУЛЬТАТЫ
    //--------------------------------------------------

    resultPanel = new QLabel(this);

    resultPanel->setStyleSheet(

                "QLabel{"
                "background-color:rgba(4,14,28,120);"
                "border:1px solid rgba(0,255,255,120);"
                "border-radius:18px;"
                "padding:15px;"
                "color:white;"
                "font-size:16px;"
                "}"

                );

    resultPanel->setText(
                "Расчет не выполнен");





    // =====================================================
    // TIMER
    // =====================================================

    timer = new QTimer(this);

    connect(timer,
            &QTimer::timeout,
            this,
            &OilSpillWindow::updateSimulation);

    timer->start(33);
    bestCost = 1e30;

    calculateOperation();

    createCharts();
}

double OilSpillWindow::degToRad(double deg)
{
    return deg * M_PI / 180.0;
}

void OilSpillWindow::drawWater()
{
    scene->clear();

    // =====================================================
    // WATER
    // =====================================================

    QLinearGradient water(
                -10000,
                -10000,
                10000,
                10000);

    water.setColorAt(
                0.0,
                QColor(1,18,38));

    water.setColorAt(
                0.45,
                QColor(5,48,88));

    water.setColorAt(
                1.0,
                QColor(1,10,24));

    scene->addRect(
                scene->sceneRect(),
                QPen(Qt::NoPen),
                QBrush(water));

    // =====================================================
    // GRID
    // =====================================================

    QPen grid(
                QColor(0,255,255,23));

    for(int x=-200000; x<200000; x+=250)
    {
        scene->addLine(
                    x,-200000,
                    x,200000,
                    grid);
    }

    for(int y=-200000; y<200000; y+=250)
    {
        scene->addLine(
                    -200000,y,
                    200000,y,
                    grid);
    }

    // =====================================================
    // HUD CORNERS
    // =====================================================

    QPen corner(
                QColor(0,255,255,26));

    corner.setWidth(2);

    for(int i=-200000; i<200000; i+=1500)
    {
        scene->addLine(
                    i,
                    -200000,
                    i+80,
                    -199920,
                    corner);
    }
}

void OilSpillWindow::drawOil()
{
    // =================================================
    // MAIN OIL SILHOUETTE
    // =================================================

    for(int i=0; i<particles.size(); i++)
    {
        OilParticle &p = particles[i];

        // OUTER CYAN GLOW

        QRadialGradient glow(
                    p.x,
                    p.y,
                    110);

        glow.setColorAt(
                    0.0,
                    QColor(0,255,220,10));

        glow.setColorAt(
                    0.5,
                    QColor(0,255,220,5));

        glow.setColorAt(
                    1.0,
                    QColor(0,0,0,0));

        scene->addEllipse(
                    p.x-110,
                    p.y-110,
                    220,
                    220,
                    QPen(Qt::NoPen),
                    QBrush(glow));

        // MAIN OIL BODY

        QRadialGradient oil(
                    p.x,
                    p.y,
                    75);

        oil.setColorAt(
                    0.0,
                    QColor(6,8,8,20));

        oil.setColorAt(
                    0.55,
                    QColor(12,18,16,10));

        oil.setColorAt(
                    0.82,
                    QColor(0,255,140,28));

        oil.setColorAt(
                    1.0,
                    QColor(0,0,0,0));

        scene->addEllipse(
                    p.x-75,
                    p.y-75,
                    150,
                    150,
                    QPen(Qt::NoPen),
                    QBrush(oil));
    }
}

void OilSpillWindow::drawHUD()
{
    QString text;

    text +=
            "<div align='center'>"
            "<span style='font-size:22px;"
            "font-weight:700;"
            "color:#7ffeff;'>"
            "ПАРАМЕТРЫ РАЗЛИВА"
            "</span>"
            "</div><br>";

    text +=
            "≋ Скорость дрейфа<br>"
            "<b>"
            + QString::number(
                driftVelocity,
                'f',
                2)
            + " км/ч</b><br><br>";

    text +=
            "➤ Направление<br>"
            "<b>"
            + QString::number(
                driftAngle*180/M_PI,
                'f',
                1)
            + "°</b><br><br>";

    text +=
            "⌖ Координаты<br>"
            "<b>X: "
            + QString::number(worldX,'f',1)
            + "<br>Y: "
            + QString::number(worldY,'f',1)
            + "</b><br><br>";

    text +=
            "⬒ Площадь<br>"
            "<b>"
            + QString::number(
                currentArea/1000000.0,
                'f',
                2)
            + " км²</b><br><br>";

    text +=
            "◌ Толщина плёнки<br>"
            "<b>"
            + QString::number(
                oilThickness,
                'f',
                4)
            + " мм</b><br><br>";

    text +=
            "◎ Периметр<br>"
            "<b>"
            + QString::number(
                perimeter/1000.0,
                'f',
                2)
            + " км</b>";

    hud->setText(text);
}

void OilSpillWindow::drawScene()
{
    drawWater();

    drawOil();

    drawHUD();

    // =====================================================
    // SMART CAMERA
    // =====================================================

    double minX = 999999;
    double maxX = -999999;

    double minY = 999999;
    double maxY = -999999;

    for(int i=0; i<particles.size(); i++)
    {
        OilParticle &p = particles[i];

        if(p.x < minX) minX = p.x;
        if(p.x > maxX) maxX = p.x;

        if(p.y < minY) minY = p.y;
        if(p.y > maxY) maxY = p.y;
    }

    double oilWidth =
            maxX - minX;

    double oilHeight =
            maxY - minY;

    double oilCenterX =
            (minX + maxX)/2.0;

    double oilCenterY =
            (minY + maxY)/2.0;

    // LOOK AHEAD

    double lookAhead = 340;

    double targetX =
            oilCenterX
            +
            cos(driftAngle)
            * lookAhead;

    double targetY =
            oilCenterY
            +
            sin(driftAngle)
            * lookAhead;

    // SOFT CAMERA

    cameraX +=
            (targetX - cameraX)
            * 0.03;

    cameraY +=
            (targetY - cameraY)
            * 0.03;

    // KEEP OIL IN SCREEN

    fitInView(
                QRectF(
                    minX-700,
                    minY-700,
                    oilWidth+1400,
                    oilHeight+1400),
                Qt::KeepAspectRatio);

    centerOn(cameraX,cameraY);

    viewport()->update();
}

void OilSpillWindow::updateSimulation()
{
    double simulationSpeed = 5.0;
    // =====================================================
    // AREA GROWTH
    // =====================================================

    if(!spreadingFinished)
    {
        // =====================================================
        // MODEL TIME
        // =====================================================

        // шаг модельного времени (часы)

        double dt = 0.05 * simulationSpeed;

        // =====================================================
        // S(t)=S0+PI*k*t
        // kSpread -> км²/ч
        // currentArea -> м²
        // =====================================================

        currentArea +=
                M_PI *
                kSpread *
                1000000.0 *
                dt;

        if(currentArea >= maxArea)
        {
            currentArea = maxArea;

            spreadingFinished = true;
        }

        // h(t)=V/S(t)

        oilThickness =
                oilVolume /
                (currentArea * 1e-3);

        if(oilThickness < minThickness)
            oilThickness = minThickness;

        perimeter =
                2 * M_PI *
                sqrt(currentArea / M_PI);
    }

    // =====================================================
    // WORLD DRIFT
    // =====================================================

    worldX +=
            cos(driftAngle)
            * driftVelocity
            * 1.15 * simulationSpeed;

    worldY +=
            sin(driftAngle)
            * driftVelocity
            * 1.15 * simulationSpeed;

    // =====================================================
    // PARTICLES
    // =====================================================

    for(int i=0; i<particles.size(); i++)
    {
        OilParticle &p = particles[i];

        if(!spreadingFinished)
        {
            double ang =
                    degToRad(
                        qrand()%360);

            // более мягкое растекание

            double spread =
                    0.08 +
                    (qrand()%100)/900.0;

            p.vx +=
                    cos(ang)
                    * spread;

            p.vy +=
                    sin(ang)
                    * spread;

            // влияние дрейфа

            p.vx +=
                    cos(driftAngle)
                    * 0.05;

            p.vy +=
                    sin(driftAngle)
                    * 0.05;
        }
        else
        {
            // после достижения min thickness
            // пятно в основном дрейфует

            p.vx +=
                    cos(driftAngle)
                    * 0.03;

            p.vy +=
                    sin(driftAngle)
                    * 0.03;
        }

        p.vx *= 0.988;
        p.vy *= 0.988;

        p.x += p.vx;
        p.y += p.vy;

        p.x +=
                cos(driftAngle)
                * driftVelocity
                * 0.65 * simulationSpeed;

        p.y +=
                sin(driftAngle)
                * driftVelocity
                * 0.65 * simulationSpeed;
    }

    drawScene();
}

void OilSpillWindow::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);

    // =====================================================
    // FIXED HUD RETICLE
    // =====================================================
    QPainter p(viewport());

    p.setRenderHint(QPainter::Antialiasing);

    int cx = width()/2;
    int cy = height()/2;

    QColor neon(0,255,255);

    // glow

    QPen glow(
                QColor(0,255,255,50));

    glow.setWidth(6);

    p.setPen(glow);

    p.drawEllipse(
                QPoint(cx,cy),
                34,
                34);

    // main reticle

    QPen pen(neon);

    pen.setWidth(1);

    p.setPen(pen);

    p.drawEllipse(
                QPoint(cx,cy),
                26,
                26);

    p.drawEllipse(
                QPoint(cx,cy),
                8,
                8);

    // lines

    p.drawLine(cx-50,cy,cx-18,cy);
    p.drawLine(cx+18,cy,cx+50,cy);

    p.drawLine(cx,cy-50,cx,cy-18);
    p.drawLine(cx,cy+18,cx,cy+50);

    // compass marks

    for(int i=0;i<360;i+=30)
    {
        double a = degToRad(i);

        int x1 =
                cx + cos(a)*32;

        int y1 =
                cy + sin(a)*32;

        int x2 =
                cx + cos(a)*38;

        int y2 =
                cy + sin(a)*38;

        p.drawLine(x1,y1,x2,y2);
    }

    // direction marker

    double dir =
            driftAngle;

    QPointF arrow1(
                cx + cos(dir)*52,
                cy + sin(dir)*52);

    QPointF arrow2(
                cx + cos(dir+0.15)*42,
                cy + sin(dir+0.15)*42);

    QPointF arrow3(
                cx + cos(dir-0.15)*42,
                cy + sin(dir-0.15)*42);

    QPolygonF arrow;

    arrow << arrow1
          << arrow2
          << arrow3;

    p.setBrush(neon);

    p.drawPolygon(arrow);

    // center point

    p.drawEllipse(
                QPoint(cx,cy),
                2,
                2);
}
//=====================================================
// SQLITE
//=====================================================



void OilSpillWindow::loadConditions()
{
    if(!databaseManager.database().isOpen())
        return;

    int waterId = 1;

    if(waterType == "Море")
        waterId = 2;

    if(waterType == "Река")
        waterId = 3;


    QSqlQuery q(databaseManager.database());
/*
    q.prepare(
        "SELECT * "
        "FROM Conditions "
        "WHERE ID_Water=?");


    */

    q.prepare(
        "SELECT "
        "Flow_speed, "
        "Flow_direction, "
        "Wind_speed, "
        "Wind_direction, "
        "Wind_coefficient, "
        "Thickness, "
        "Surface_coefficient, "
        "Distance_shore, "
        "Distance_base, "
        "Vulnerability_coefficient "
        "FROM Conditions "
        "WHERE ID_Water = ?");

    q.addBindValue(waterId);

    if(!q.exec())
    {
        qDebug()
                << q.lastError().text();

        return;
    }

    if(!q.next())
    {
        qDebug()
                << "Conditions EMPTY";

        return;
    }
/*
    vCurrent =
            q.value(2).toDouble();

    dCurrent =
            q.value(3).toDouble();

    vWind =
            q.value(4).toDouble();

    dWind =
            q.value(5).toDouble();

    kWind =
            q.value(6).toDouble();

    initialThickness =
            q.value(7).toDouble();
    qDebug()
            << "Thickness from DB ="
            << initialThickness;

    kSpread =
            q.value(8).toDouble();

    distanceShore =
            q.value(9).toDouble();

    distanceBase =
            q.value(10).toDouble();

    vulnerabilityCoefficient =
            q.value(11).toDouble();

    qDebug()
            << "Conditions loaded:"
            << waterType
            << vCurrent
            << vWind
            << initialThickness;
*/

    QSqlRecord rec = q.record();

    vCurrent =
            q.value(rec.indexOf("Flow_speed")).toDouble();

    dCurrent =
            q.value(rec.indexOf("Flow_direction")).toDouble();

    vWind =
            q.value(rec.indexOf("Wind_speed")).toDouble();

    dWind =
            q.value(rec.indexOf("Wind_direction")).toDouble();

    kWind =
            q.value(rec.indexOf("Wind_coefficient")).toDouble();

    initialThickness =
            q.value(rec.indexOf("Thickness")).toDouble();

    kSpread =
            q.value(rec.indexOf("Surface_coefficient")).toDouble();

    distanceShore =
            q.value(rec.indexOf("Distance_shore")).toDouble();

    distanceBase =
            q.value(rec.indexOf("Distance_base")).toDouble();

    vulnerabilityCoefficient =
            q.value(rec.indexOf("Vulnerability_coefficient")).toDouble();

    double sx =
            vCurrent * sin(degToRad(dCurrent))
            +
            kWind * vWind * sin(degToRad(dWind));

    double sy =
            vCurrent * cos(degToRad(dCurrent))
            +
            kWind * vWind * cos(degToRad(dWind));

    driftVelocity =
            sqrt(sx * sx + sy * sy);

    driftAngle =
            atan2(sy, sx);

}



double OilSpillWindow::calculateSearchArea()
{
    return currentArea * 1.25;
}
QVector<double> riskCosts;
QVector<QString> riskColors;
void OilSpillWindow::calculateOperation()
{

    double maxPhysicalTime =
            distanceShore /
            driftVelocity;

    if(timeLimit > maxPhysicalTime)
    {
        QMessageBox::warning(
                    nullptr,
                    "Ошибка",
                    "Для выбранных условий максимально допустимое время операции составляет "
                    + QString::number(maxPhysicalTime-0.1,'f',1)
                    + " ч.");

        QTimer::singleShot(
                    0,
                    this,
                    SLOT(close()));

        return;
    }

    /*
// надолго так не оставлять, пока открыто для рисков
    double tDetect =
            0.3 * timeLimit;

    double tLiquid =
            0.7 * timeLimit;

*/
    if(!databaseManager.database().isOpen())
        return;

    searchArea =
            calculateSearchArea();

    //--------------------------------------------------
    // Константы
    //--------------------------------------------------

    QSqlQuery c(databaseManager.database());

    c.exec("SELECT * FROM ConstData");
    qDebug() << "ConstData exec =" << c.lastError().text();

    if(!c.next())
    {
        qDebug() << "ConstData EMPTY";
        return;
    }

    double viewingAngle =
            c.value(1).toDouble();

    double flightHeight =
            c.value(2).toDouble();

    double loadingTime =
            c.value(3).toDouble();

    double weightBooms =
            c.value(4).toDouble();

    double sprayRate =
            c.value(5).toDouble();

    double density =
            c.value(6).toDouble();

    double boomSpeed =
            c.value(7).toDouble();

    /*
    double fuelPrice =
            c.value(8).toDouble();*/

    double boomPrice =
            c.value(8).toDouble();

    double dispersantPrice =
            c.value(9).toDouble();

    //--------------------------------------------------
    // масса нефти
    //--------------------------------------------------

    oilMass =
            oilVolume *
            density;

    boomsMass =
            perimeter / 1000.0 *
            weightBooms;

    uavNames.clear();
    uavCosts.clear();
    uavCounts.clear();

    heliNames.clear();
    heliCosts.clear();
    heliCounts.clear();

    planeNames.clear();
    planeCosts.clear();
    planeCounts.clear();

    //--------------------------------------------------
    // перебор всех вариантов
    //--------------------------------------------------

    QSqlQuery uavs(databaseManager.database());

/*
    uavNames.clear();
    uavCosts.clear();
    uavCounts.clear();

    heliNames.clear();
    heliCosts.clear();
    heliCounts.clear();

    planeNames.clear();
    planeCosts.clear();
    planeCounts.clear();
*/
    bestCost = 1e100;

/*
    QString bestUAV;
    QString bestHeli;
    QString bestPlane;

    double bestCountUAV = 0;
    double bestCountHeli = 0;
    double bestCountPlane = 0;
*/
    graphTimes.clear();
    graphCosts.clear();
    graphUAVCounts.clear();
    graphHeliCounts.clear();
    graphPlaneCounts.clear();

    graphOperationTimes.clear();

    for(int t = 1; t <= ceil(timeLimit); t++)
    {

        uavs.exec(
                    "SELECT * FROM UAVS");

        double localBestCost = 1e100;

        double localBestOperationTime = 0;

        int localBestUavCount = 0;
        int localBestHeliCount = 0;
        int localBestPlaneCount = 0;
        QString localBestUavName;
        QString localBestHeliName;
        QString localBestPlaneName;

        double bestLocalCostForTime = 1e100;

        double bestLocalEpsilon = 0.0;
        double bestLocalBeta = 0.0;
        //double beta = 0.0;

        /*
        double tDetect =
                0.3 * t;

        double tLiquid =
                0.7 * t;
        */
        for(double epsilon = 0.1;
            epsilon <= 0.9;
            epsilon += 0.1)
        {
            double tDetect =
                    epsilon *
                    t;

            double tLiquid =
                    (1.0 - epsilon) *
                    t;

            for(double beta = 0.1;
                beta <= 0.9;
                beta += 0.1)
            {

                qDebug()
                        << "EPSILON"
                        << epsilon;

                qDebug()
                        << "BETA"
                        << beta;

    while(uavs.next())
    {
        int idUAV =
                uavs.value(0).toInt();

        QString uavName =
                uavs.value(1).toString();

        double vUAV =
                uavs.value(2).toDouble();

        double rangeUAV =
                uavs.value(3).toDouble();

        double fuelWeightUAV =
                uavs.value(4).toDouble();

        double fuelConsumptionUAV =
                uavs.value(5).toDouble();

        double costUAV =
                uavs.value(6).toDouble();

        double fuelPriceUAV =
                c.value(7).toDouble();

        //--------------------------------------------------
        // производительность БПЛА
        //--------------------------------------------------

       /* double stripWidth =
                2.0 *
                (flightHeight * 1000.0) *
                tan(
                    degToRad(
                        viewingAngle/2.0));

        double qUAV =
                vUAV *
                stripWidth;*/


        double tFlightUAV =
                fuelWeightUAV /
                fuelConsumptionUAV;

        double searchRadius =
                driftVelocity *
                tDetect;

        double stripWidth =
                2.0 *
                flightHeight *
                tan(
                    degToRad(
                        viewingAngle / 2.0));

        double thetaSpiral =
                (2.0 * M_PI * searchRadius)
                /
                stripWidth;

        double totalSearchLength =
                (stripWidth /
                 (4.0 * M_PI))
                *
                (
                    thetaSpiral *
                    sqrt(
                        1.0 +
                        thetaSpiral *
                        thetaSpiral)
                    +
                    log(
                        thetaSpiral +
                        sqrt(
                            1.0 +
                            thetaSpiral *
                            thetaSpiral))
                );

        double searchLengthOneUAV =
                rangeUAV
                -
                2.0 *
                distanceBase;

        double searchAreaKm2 =
                searchArea / 1000000.0;


        double nFlightsUAV =
                ceil(
                    totalSearchLength
                    /
                    searchLengthOneUAV);


       /* double nFlightsUAV =
                searchArea /
                (qUAV * tFlightUAV);
        double tFlightMission =
                2.0*distanceBase /vUAV +
                searchArea/qUAV;*/


        double tFlightMission =
                (
                    searchLengthOneUAV
                    +
                    2.0 *
                    distanceBase
                )
                /
                vUAV;




        double countUAV =
                ceil(
                    nFlightsUAV *
                    tFlightMission /
                    tDetect);

        double nFlightsPerUAV =
        ceil(nFlightsUAV / countUAV);

        int tRealUAV =
        nFlightsPerUAV *
        tFlightMission;

        qDebug()
        << "ТУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУУТ"
        << "searchRadius="
        << searchRadius
        << "totalSearchLength="
        << totalSearchLength
        << "searchLengthOneUAV="
        << searchLengthOneUAV
        << "thetaSpiral="
        << thetaSpiral
           ;

        qDebug()
            << "distanceBase =" << distanceBase
            << "rangeUAV =" << rangeUAV
            << "vDrift =" << driftVelocity;

        qDebug()
            << "UAV CHECK основной расчет"
            << uavName
         //   << "qUAV=" << qUAV
            << "tFlightUAV=" << tFlightUAV
            << "nFlightsUAV=" << nFlightsUAV
            << "tFlightMission=" << tFlightMission
            << "countUAV=" << countUAV;

        //--------------------------------------------------
        // ограничение по дальности
        //--------------------------------------------------
        qDebug() << "CHECK UAV";
       /* if(rangeUAV < 240)
        {
            qDebug() << "UAV range fail:"
                     << uavName
                     << rangeUAV;

            continue;
        }*/
        if(searchLengthOneUAV <= 0.0)
        {
            continue;
        }
        //--------------------------------------------------
        // ВЕРТОЛЕТЫ
        //--------------------------------------------------

        QSqlQuery helis(databaseManager.database());

        helis.exec(
                    "SELECT * FROM Helicopters");

        while(helis.next())
        {
            int idHeli =
                    helis.value(0).toInt();

            QString heliName =
                    helis.value(1).toString();

            double vHeli =
                    helis.value(2).toDouble();

            double rangeHeli =
                    helis.value(3).toDouble();

            double capacityHeli =
                    helis.value(4).toDouble();

            double fuelWeightHeli =
                    helis.value(5).toDouble();

            double fuelConsumptionHeli =
                    helis.value(6).toDouble();

            double costHeli =
                    helis.value(7).toDouble();

            double fuelPriceHeli =
                    c.value(8).toDouble();

            //--------------------------------------------------
            // ограничение по дальности
            //--------------------------------------------------

            if(rangeHeli < 240)
            {
                qDebug() << "HELI range fail:"
                         << heliName
                         << rangeHeli;

                continue;
            }
            //--------------------------------------------------
            // ВК
            //--------------------------------------------------

            double nFlightsHeli =
                    ceil(
                        boomsMass /
                        capacityHeli);

            double tBooms =
                    (perimeter/1000.0) /
                    boomSpeed;

            double tHeliFlight =
                    2.0 * distanceBase  /
                    vHeli;

            double tHeliMission =
                    tHeliFlight
                    +
                    tBooms
                    +
                    loadingTime;

            double countHeli =
                    ceil(
                        nFlightsHeli *
                        tHeliMission /
                        (beta * tLiquid));

            double nFlightsPerHeli =
            ceil(nFlightsHeli / countHeli);

            int tRealHeli =
            nFlightsPerHeli *
            tHeliMission;

            //--------------------------------------------------
            // САМОЛЕТЫ
            //--------------------------------------------------

            QSqlQuery planes(databaseManager.database());

            planes.exec(
                        "SELECT * FROM Airplanes");

            while(planes.next())
            {
                int idPlane =
                        planes.value(0).toInt();

                QString planeName =
                        planes.value(1).toString();

                double vPlane =
                        planes.value(2).toDouble();

                double rangePlane =
                        planes.value(3).toDouble();

                double capacityPlane =
                        planes.value(4).toDouble();

                double sprayWidth =
                        planes.value(5).toDouble();

                double fuelWeightPlane =
                        planes.value(6).toDouble();

                double fuelConsumptionPlane =
                        planes.value(7).toDouble();

                double costPlane =
                        planes.value(8).toDouble();

                double fuelPricePlane =
                        c.value(8).toDouble();

                //------------------------------------------
                // ограничение по дальности
                //------------------------------------------

                if(rangePlane < 240)
                {
                    qDebug() << "PLANE range fail:"
                             << planeName
                             << rangePlane;

                    continue;
                }

                //------------------------------------------
                // АК
                //------------------------------------------


                double areaAtArrival =
                        currentArea
                        +
                        M_PI *
                        kSpread *
                        1000000.0 *
                        (
                            tDetect
                            +
                            beta * tLiquid
                        );
                double areaAtArrivalKm2 =
                        areaAtArrival /
                        1000000.0;

                double dispersantVolume =
                        areaAtArrivalKm2 *
                        sprayRate *
                        100.0;
                double dispersantMassLocal =
                        dispersantVolume *
                        density /
                        1000.0;

                double qSpray =
                        vPlane *
                        sprayWidth *
                        sprayRate *
                        100.0;

                double tSpray =
                        areaAtArrival /
                        qSpray;

                double nFlightsPlane =
                        ceil(
                            dispersantMassLocal /
                            capacityPlane);

                double tPlaneFlight =
                        2.0 * distanceBase  /
                        vPlane;

                double tPlaneMission =
                        tPlaneFlight
                        +
                        tSpray
                        +
                        loadingTime;

                double countPlane =
                        ceil(
                            nFlightsPlane *
                            tPlaneMission /
                            tLiquid);

                double nFlightsPerPlane =
                ceil(nFlightsPlane / countPlane);

                int tRealPlane =
                nFlightsPerPlane *
                tPlaneMission;

                qDebug()
                    << "PLANE CHECK"
                    << planeName
                    << "dispersantMass=" << dispersantMassLocal
                    << "capacity=" << capacityPlane
                    << "nFlightsPlane=" << nFlightsPlane
                    << "tPlaneMission=" << tPlaneMission
                    << "countPlane=" << countPlane;


                //--------------------------------------------------
                // ЗАТРАТЫ ЭКСПЛУАТАЦИИ
                //--------------------------------------------------
/*
                double CrUAV =
                        countUAV *
                        costUAV *
                        tDetect;

                double CrHeli =
                        countHeli *
                        costHeli *
                        0.5 *
                        tLiquid;

                double CrPlane =
                        countPlane *
                        costPlane *
                        tLiquid;


                //риск скопирован в основной расчет
                double CrUAV =
                        costUAV *
                        nFlightsUAV *
                        tFlightMission;


                double CrHeli =
                        costHeli *
                        nFlightsHeli *
                        tHeliMission;


                double CrPlane =
                    costPlane *
                    nFlightsPlane *
                    tPlaneMission;
              */


//было 26 июня:
/*
                double CrUAV =
                    costUAV *
                    countUAV ;



                double CrHeli =
                    costHeli *
                    countHeli ;

                double CrPlane =
                    costPlane *
                    countPlane ;

                double CrUAV =
                    costUAV *
                    nFlightsUAV *
                    tFlightMission;
                double CrHeli =
                    costHeli *
                    nFlightsHeli *
                    tHeliMission;
                double CrPlane =
                    costPlane *
                    nFlightsPlane *
                    tPlaneMission;*/

                double CrUAV =
                    countUAV * tRealUAV * costUAV;
                double CrHeli =
                    countHeli * tRealHeli * costHeli;
                double CrPlane =
                    countPlane * tRealPlane * costPlane;

                double Cr =
                        CrUAV +
                        CrHeli +
                        CrPlane;

                //--------------------------------------------------
                // ЗАТРАТЫ НА ТОПЛИВО
                //--------------------------------------------------
/*
                double CfUAV =
                        countUAV *
                        fuelConsumptionUAV *
                        tDetect *
                        fuelPrice;

                double CfHeli =
                        countHeli *
                        fuelConsumptionHeli *
                        0.5 *
                        tLiquid *
                        fuelPrice;

                double CfPlane =
                        countPlane *
                        fuelConsumptionPlane *
                        tLiquid *
                        fuelPrice;


                //риск скопирован в основной расчет

                double CfUAV =
                            fuelConsumptionUAV *
                            nFlightsUAV * tFlightMission *
                            fuelPrice;

                double CfHeli =
                        fuelConsumptionHeli *
                        nFlightsHeli * tHeliMission *
                        fuelPrice;

                double CfPlane =
                        fuelConsumptionPlane *
                        nFlightsPlane *
                        tPlaneMission *
                        fuelPrice;
*/



//было 26 июня топливо
/*
                double CfUAV =
                        fuelConsumptionUAV *
                        countUAV *

                        fuelPrice;

                double CfHeli =
                        fuelConsumptionHeli *
                        countHeli *

                        fuelPrice;

                double CfPlane =
                        fuelConsumptionPlane *
                        countPlane *

                        fuelPrice;

                double CfUAV =
                    nFlightsUAV *
                    tFlightMission *
                    fuelConsumptionUAV *
                    fuelPrice;
                double CfHeli =
                    nFlightsHeli *
                    tHeliMission *
                    fuelConsumptionHeli *
                    fuelPrice;
                double CfPlane =
                    nFlightsPlane *
                    tPlaneMission *
                    fuelConsumptionPlane *
                    fuelPrice;*/

                double CfUAV =
                    countUAV *
                    tRealUAV *
                    fuelConsumptionUAV *
                    fuelPriceUAV;
                double CfHeli =
                        countHeli *
                        tRealHeli *
                    fuelConsumptionHeli *
                    fuelPriceHeli;
                double CfPlane =
                        countPlane *
                        tRealPlane *
                    fuelConsumptionPlane *
                    fuelPricePlane;


                double Cf =
                        CfUAV +
                        CfHeli +
                        CfPlane;

                double totalUAVCost =
                        CrUAV +
                        CfUAV;
/*
                if(!uavNames.contains(uavName))
                {
                    uavNames.push_back(uavName);
                    uavCosts.push_back(totalUAVCost);
                    uavCounts.push_back(countUAV);
                }
                */

/*
                uavNames.push_back(
                            uavName);

                uavCosts.push_back(
                            totalUAVCost);

                uavCounts.push_back(
                            countUAV);
*/



                if(totalUAVCost < bestUAVCost)
                {
                    bestUAVCost =
                            totalUAVCost;

                    bestUAV =
                            uavName;

                    bestUAVCount =
                            countUAV;

                    bestUAVId =
                            idUAV;

                    bestUAVOperationCost =
                            CrUAV;

                    bestUAVFuelCost =
                            CfUAV;
                }

                double totalHeliCost =
                        CrHeli +
                        CfHeli;

                /*
                if(!heliNames.contains(heliName))
                {
                    heliNames.push_back(heliName);
                    heliCosts.push_back(totalHeliCost);
                    heliCounts.push_back(countHeli);
                }
                */

/*
                heliNames.push_back(
                            heliName);

                heliCosts.push_back(
                            totalHeliCost);

                heliCounts.push_back(
                            countHeli);
*/
                if(totalHeliCost < bestHeliCost)
                {
                    bestHeliCost =
                            totalHeliCost;

                    bestHeli =
                            heliName;

                    bestHeliCount =
                            countHeli;

                    bestHeliId =
                            idHeli;

                    bestHeliOperationCost =
                            CrHeli;

                    bestHeliFuelCost =
                            CfHeli;
                }

                double totalPlaneCost =
                        CrPlane +
                        CfPlane;

                /*
                if(!planeNames.contains(planeName))
                {
                    planeNames.push_back(planeName);
                    planeCosts.push_back(totalPlaneCost);
                    planeCounts.push_back(countPlane);
                }
                */

/*
                planeNames.push_back(
                            planeName);

                planeCosts.push_back(
                            totalPlaneCost);

                planeCounts.push_back(
                            countPlane);
*/
                if(totalPlaneCost < bestPlaneCost)
                {
                    bestPlaneCost =
                            totalPlaneCost;

                    bestPlane =
                            planeName;

                    bestPlaneCount =
                            countPlane;

                    bestPlaneId =
                            idPlane;

                    dispersantMass =
                            dispersantMassLocal;

                    bestPlaneOperationCost =
                            CrPlane;

                    bestPlaneFuelCost =
                            CfPlane;
                }

                //--------------------------------------------------
                // ЗАТРАТЫ НА МАТЕРИАЛЫ
                //--------------------------------------------------

                double CmDisp =
                        dispersantMassLocal *
                        dispersantPrice;

                double CmBooms =
                        boomsMass *
                        boomPrice;

                double Cm =
                        CmDisp +
                        CmBooms;

                //--------------------------------------------------
                // ОБЩИЕ ЗАТРАТЫ
                //--------------------------------------------------

                qDebug()
                    << "MAIN COST DETAILS"
                    << uavName
                    << heliName
                    << planeName
                    << "CrUAV=" << CrUAV
                    << "CrHeli=" << CrHeli
                    << "CrPlane=" << CrPlane
                    << "CfUAV=" << CfUAV
                    << "CfHeli=" << CfHeli
                    << "CfPlane=" << CfPlane
                    << "Cm=" << Cm;

                double totalCost =
                        Cr +
                        Cf +
                        Cm;

                qDebug()
                        << "EPSILON"
                        << epsilon
                        << "TOTAL"
                        << totalCost;
                qDebug()
                        << "BETA"
                        << beta
                        << "TOTAL"
                        << totalCost;

                if(totalCost < bestLocalCostForTime)
                {
                    bestLocalCostForTime =
                            totalCost;

                    bestLocalEpsilon =
                            epsilon;

                    bestLocalBeta =
                            beta;
                }

                qDebug()
                        << "MAIN TOTAL COST"
                        << uavName
                        << heliName
                        << planeName
                        << totalCost;

                /*
                 * if(bestOperationTime  <= t)
                {
                if(totalCost < bestCost)
                {
                    bestCost = totalCost;

                    bestUAV = uavName;
                    bestHeli = heliName;
                    bestPlane = planeName;

                    bestUAVCount = countUAV;
                    bestHeliCount = countHeli;
                    bestPlaneCount = countPlane;
                }
                }
                */
                /*
                double operationTime = t;

                if(operationTime <= timeLimit)
                {
                    if(totalCost < localBestCost)
                    {
                        localBestCost = totalCost;
                        localBestTime = bestOperationTime;

                        localBestUav = u;
                        localBestHeli = h;
                        localBestPlane = p;
                    }
                }*/

                /*
                if(totalCost < localBestCost)
                {
                    localBestCost = totalCost;
                }
                */

                double operationTime = tDetect + tLiquid;


                qDebug()
                    << "СЕЙЧАССЕЙЧАССЕЙЧАССЕЙЧАССЕЙЧАССЕЙЧАССЕЙЧАССЕЙЧАССЕЙЧАССЕЙЧАССЕЙЧАССЕЙЧАССЕЙЧАС"
                    << "t =" << t
                    << "operationTime =" << operationTime
                    << "countUAV =" << countUAV
                    << "countHeli =" << countHeli
                    << "countPlane =" << countPlane
                    << "Cr =" << Cr
                    << "Cf =" << Cf
                    << "Cm =" << Cm
                    << "total =" << totalCost;
                if(operationTime <= t)
                {
                    if(totalCost < localBestCost)
                    {
                        localBestCost = totalCost;

                        localBestOperationTime = operationTime;

                        localBestUavCount = countUAV;
                        localBestHeliCount = countHeli;
                        localBestPlaneCount = countPlane;

                        localBestUavName = uavName;
                        localBestHeliName = heliName;
                        localBestPlaneName = planeName;
                    }
                }

                //--------------------------------------------------
                // ОГРАНИЧЕНИЕ ПО ТОПЛИВУ
                //--------------------------------------------------

                double enduranceUAV =
                        fuelWeightUAV /
                        fuelConsumptionUAV;

                double tOneUAVFlight =
                        2.0 * distanceBase  / vUAV +
                        tFlightUAV;

                double enduranceHeli =
                        fuelWeightHeli /
                        fuelConsumptionHeli;

                double endurancePlane =
                        fuelWeightPlane /
                        fuelConsumptionPlane;
/*
                if(tOneUAVFlight > enduranceUAV)
                {
                    qDebug()
                            << "UAV endurance fail"
                            << uavName
                            << "mission="
                            << tOneUAVFlight
                            << "endurance="
                            << enduranceUAV;

                    continue;
                }

                if(tHeliMission > enduranceHeli)
                {
                    qDebug()
                            << "HELI endurance fail"
                            << heliName
                            << "mission="
                            << tHeliMission
                            << "endurance="
                            << enduranceHeli;

                    continue;
                }

                if(tPlaneMission > endurancePlane)
                {
                    qDebug()
                            << "PLANE endurance fail"
                            << planeName
                            << "mission="
                            << tPlaneMission
                            << "endurance="
                            << endurancePlane;

                    continue;
                }
*/


            }
        }
    }
}//для beta
} //для epsilon
/*
    if(localBestCost < 1e100)
    {
        graphTimes.append(t);

        graphCosts.append(
            localBestCost / 1000000.0);
    }
    qDebug()
        << "GRAPH:"
        << t
        << localBestCost;
        */
    if(localBestCost < 1e100)
    {
        graphTimes.append(
            localBestOperationTime);

        graphCosts.append(
            localBestCost / 1000000.0);

        graphOperationTimes.append(
            localBestOperationTime);

        graphUAVCounts.append(
            localBestUavCount);

        graphHeliCounts.append(
            localBestHeliCount);

        graphPlaneCounts.append(
            localBestPlaneCount);

        graphUAVNames.append(
            localBestUavName);

        graphHeliNames.append(
            localBestHeliName);

        graphPlaneNames.append(
            localBestPlaneName);

        graphEpsilons.append(
            bestLocalEpsilon);

        graphBetas.append(
                bestLocalBeta);
    }
}


    int bestIndex = -1;
    double minCost = 1e100;


    for(int i = 0; i < graphCosts.size(); i++)
    {
        if(graphCosts[i] < minCost)
        {
            minCost = graphCosts[i];
            bestIndex = i;
        }
    }

    if(bestIndex >= 0)
    {
        bestCost =
                graphCosts[bestIndex] * 1000000.0;

        bestOperationTime =
                graphOperationTimes[bestIndex];

        bestUAVCount =
                graphUAVCounts[bestIndex];

        bestHeliCount =
                graphHeliCounts[bestIndex];

        bestPlaneCount =
                graphPlaneCounts[bestIndex];

        bestUAV =
                graphUAVNames[bestIndex];

        bestHeli =
                graphHeliNames[bestIndex];

        bestPlane =
                graphPlaneNames[bestIndex];

        bestEpsilon =
                graphEpsilons[bestIndex];

        bestBeta =
                graphBetas[bestIndex];
    }



    uavNames.clear();
    uavCosts.clear();
    uavCounts.clear();

    heliNames.clear();
    heliCosts.clear();
    heliCounts.clear();

    planeNames.clear();
    planeCosts.clear();
    planeCounts.clear();
/*
    uavNames.push_back(bestUAV);
    uavCosts.push_back(bestUAVCost);
    uavCounts.push_back(bestUAVCount);

    heliNames.push_back(bestHeli);
    heliCosts.push_back(bestHeliCost);
    heliCounts.push_back(bestHeliCount);

    planeNames.push_back(bestPlane);
    planeCosts.push_back(bestPlaneCost);
    planeCounts.push_back(bestPlaneCount);
*/

    double tDetectBest =
            bestEpsilon * bestOperationTime;

    double tLiquidBest =
            (1-bestEpsilon) * bestOperationTime;

    for(int i = 0; i < graphOperationTimes.size(); i++)
    {
        if(qFuzzyCompare(
                graphOperationTimes[i] + 1.0,
                bestOperationTime + 1.0))
        {
            continue;
        }
    }

    QSqlQuery uavsChart(databaseManager.database());

    uavsChart.exec("SELECT * FROM UAVs");

    while(uavsChart.next())
    {
        QString uavName =
                uavsChart.value(1).toString();

        double vUAV =
                uavsChart.value(2).toDouble();

        double fuelConsumptionUAV =
                uavsChart.value(5).toDouble();

        double costUAV =
                uavsChart.value(6).toDouble();

        double fuelPriceUAV =
                c.value(7).toDouble();

        double stripWidth =
                2.0 *
                (flightHeight * 1000.0) *
                tan(
                    degToRad(
                        viewingAngle / 2.0));
        /*

        double qUAV =
                vUAV *
                stripWidth;

        double tFlightUAV =
                uavsChart.value(4).toDouble() /
                fuelConsumptionUAV;

        double nFlightsUAV =
                searchArea /
                (qUAV * tFlightUAV);

        double tFlightMission =
                2.0 * distanceBase / vUAV +
                searchArea / qUAV;

        double countUAV =
                ceil(
                    nFlightsUAV *
                    tFlightMission /
                    tDetectBest);
                    */

        double searchRadius =
                driftVelocity *
                tDetectBest;
        /*
        double stripWidth =
                2.0 *
                flightHeight *
                tan(
                    degToRad(
                        viewingAngle / 2.0));
        */
        double thetaSpiral =
                (2.0 * M_PI * searchRadius)
                /
                stripWidth;

        double totalSearchLength =
                (stripWidth /
                 (4.0 * M_PI))
                *
                (
                    thetaSpiral *
                    sqrt(
                        1.0 +
                        thetaSpiral *
                        thetaSpiral)
                    +
                    log(
                        thetaSpiral +
                        sqrt(
                            1.0 +
                            thetaSpiral *
                            thetaSpiral))
                );

        double searchLengthOneUAV =
                uavsChart.value(3).toDouble()
                -
                2.0 *
                distanceBase;

        if(searchLengthOneUAV <= 0.0)
        {
            continue;
        }

        double nFlightsUAV =
                ceil(
                    totalSearchLength
                    /
                    searchLengthOneUAV);

        double tFlightMission =
                (
                    searchLengthOneUAV
                    +
                    2.0 *
                    distanceBase
                )
                /
                vUAV;

        double countUAV =
                ceil(
                    nFlightsUAV *
                    tFlightMission /
                    tDetectBest);

        double nFlightsPerUAV =
        ceil(nFlightsUAV / countUAV);

        int tRealUAV =
        nFlightsPerUAV *
        tFlightMission;

        /*
        double CrUAV =
                costUAV *
                countUAV;

        double CrUAV =
            costUAV *
            nFlightsUAV *
            tFlightMission;*/
        double CrUAV =
            countUAV * tRealUAV * costUAV;


/*
        double CfUAV =
                fuelConsumptionUAV *
                countUAV *
                fuelPrice;

        double CfUAV =
            nFlightsUAV *
            tFlightMission *
            fuelConsumptionUAV *
            fuelPrice;*/

        double CfUAV =
            countUAV *
            tRealUAV *
            fuelConsumptionUAV *
            fuelPriceUAV;

        double totalUAVCost =
                CrUAV +
                CfUAV;

        uavNames.push_back(uavName);
        uavCosts.push_back(totalUAVCost);
        uavCounts.push_back(countUAV);
    }

    QSqlQuery helisChart(databaseManager.database());

    helisChart.exec("SELECT * FROM Helicopters");

    while(helisChart.next())
    {
        QString heliName =
                helisChart.value(1).toString();

        double vHeli =
                helisChart.value(2).toDouble();

        double capacityHeli =
                helisChart.value(4).toDouble();

        double fuelConsumptionHeli =
                helisChart.value(6).toDouble();

        double costHeli =
                helisChart.value(7).toDouble();

        double fuelPriceHeli =
                c.value(8).toDouble();

        double nFlightsHeli =
                ceil(
                    boomsMass /
                    capacityHeli);

        double tBooms =
                (perimeter / 1000.0) /
                boomSpeed;

        double tHeliFlight =
                2.0 * distanceBase /
                vHeli;

        double tHeliMission =
                tHeliFlight +
                tBooms +
                loadingTime;

        double countHeli =
                ceil(
                    nFlightsHeli *
                    tHeliMission /
                    (bestBeta * tLiquidBest));

        double nFlightsPerHeli =
        ceil(nFlightsHeli / countHeli);

        int tRealHeli =
        nFlightsPerHeli *
        tHeliMission;

        /*
        double CrHeli =
                costHeli *
                countHeli;

        double CrHeli =
            costHeli *
            nFlightsHeli *
            tHeliMission; */

        double CrHeli =
            countHeli * tRealHeli * costHeli;


/*
        double CfHeli =
                fuelConsumptionHeli *
                countHeli *
                fuelPrice;

        double CfHeli =
            nFlightsHeli *
            tHeliMission *
            fuelConsumptionHeli *
            fuelPrice;*/

        double CfHeli =
                countHeli *
                tRealHeli *
            fuelConsumptionHeli *
            fuelPriceHeli;

        double totalHeliCost =
                CrHeli +
                CfHeli;

        heliNames.push_back(heliName);
        heliCosts.push_back(totalHeliCost);
        heliCounts.push_back(countHeli);
    }

    QSqlQuery planesChart(databaseManager.database());

    planesChart.exec("SELECT * FROM Airplanes");

    while(planesChart.next())
    {
        QString planeName =
                planesChart.value(1).toString();

        double vPlane =
                planesChart.value(2).toDouble();

        double capacityPlane =
                planesChart.value(4).toDouble();

        double sprayWidth =
                planesChart.value(5).toDouble();

        double fuelConsumptionPlane =
                planesChart.value(7).toDouble();

        double costPlane =
                planesChart.value(8).toDouble();

        double fuelPricePlane =
                c.value(8).toDouble();

        double areaAtArrival =
                currentArea +
                M_PI *
                kSpread *
                1000000.0 *
                (
                    tDetectBest +
                    bestBeta * tLiquidBest
                );

        double dispersantVolume =
                areaAtArrival /
                1000000.0 *
                sprayRate *
                100.0;

        double dispersantMassLocal =
                dispersantVolume *
                density /
                1000.0;

        double qSpray =
                vPlane *
                sprayWidth *
                sprayRate *
                100.0;

        double tSpray =
                areaAtArrival /
                qSpray;

        double nFlightsPlane =
                ceil(
                    dispersantMassLocal /
                    capacityPlane);

        double tPlaneFlight =
                2.0 * distanceBase /
                vPlane;

        double tPlaneMission =
                tPlaneFlight +
                tSpray +
                loadingTime;

        double countPlane =
                ceil(
                    nFlightsPlane *
                    tPlaneMission /
                    tLiquidBest);

        double nFlightsPerPlane =
        ceil(nFlightsPlane / countPlane);

        int tRealPlane =
        nFlightsPerPlane *
        tPlaneMission;
/*
        double CrPlane =
                costPlane *
                countPlane;

        double CrPlane =
            costPlane *
            nFlightsPlane *
            tPlaneMission;*/

        double CrPlane =
            countPlane * tRealPlane * costPlane;

        /*
        double CfPlane =
                fuelConsumptionPlane *
                countPlane *
                fuelPrice;

        double CfPlane =
            nFlightsPlane *
            tPlaneMission *
            fuelConsumptionPlane *
            fuelPrice;*/

        double CfPlane =
                countPlane *
                tRealPlane *
            fuelConsumptionPlane *
            fuelPricePlane;

        double totalPlaneCost =
                CrPlane +
                CfPlane;

        planeNames.push_back(planeName);
        planeCosts.push_back(totalPlaneCost);
        planeCounts.push_back(countPlane);
    }




    riskTimes.clear();
    riskValues.clear();

    for(int i = 0; i < graphOperationTimes.size(); i++)
    {
        double t = graphOperationTimes[i];

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
                qBound(0.0, shoreFactor, 1.0);

        double risk =
                (oilVolume / 1000.0)
                *
                shoreFactor
                *
                (driftVelocity / 10.0)
                *
                vulnerabilityCoefficient
                ;

        risk =
                qBound(0.0, risk, 1.0);

        riskTimes.push_back(t);
        riskValues.push_back(risk);
    }

    scenarios.clear();

    QVector<double> scenarioFractions;

    scenarioFractions
            << 0.25
            << 0.50
            << 0.75
            << 1.00;

    qDebug()
            << "MAIN COST"
            << bestCost;


    operationCost =
            bestUAVOperationCost
            +
            bestHeliOperationCost
            +
            bestPlaneOperationCost;

    fuelCost =
            bestUAVFuelCost
            +
            bestHeliFuelCost
            +
            bestPlaneFuelCost;

    materialCost =
            dispersantMass *
            dispersantPrice
            +
            boomsMass *
            boomPrice;

    bestCost =
            operationCost
            +
            fuelCost
            +
            materialCost;

/*
   bestOperationTime =
           timeLimit;

*/


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

    riskValue =
            (oilVolume / 1000.0)
            *
            shoreFactor
            *
            (driftVelocity / 10.0)
            *
            vulnerabilityCoefficient
           ;

    if(riskValue > 1)
        riskValue = 1;

    if(riskValue < 0)
        riskValue = 0;

    if(riskValue <= 0.25)
    {
        riskLevel = "Низкий";
        riskColor = "#00ff55";
    }
    else if(riskValue <= 0.35)
    {
        riskLevel = "Средний";
        riskColor = "#ffff00";
    }
    else if(riskValue <= 0.6)
    {
        riskLevel = "Высокий";
        riskColor = "#ff8800";
    }
    else
    {
        riskLevel = "Критический";
        riskColor = "#ff0000";
    }

    //Risk for all scenarios
  /*
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

    QVector<RiskScenario> scenarios;
*/
    scenarios.clear();


    QVector<double> scenarioTimes;



    for(double fraction : scenarioFractions)
    {
        double targetTime =
                bestOperationTime * fraction;

        int bestScenarioIndex = 0;

        double minDelta = 1e100;

        for(int i = 0; i < graphOperationTimes.size(); i++)
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
                graphCosts[bestScenarioIndex] * 1000000.0;

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
                riskValues[bestScenarioIndex];
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

        scenarios.push_back(s);
    }

    for(int i = 0; i < scenarioTimes.size(); i++)
    {
        double scenarioTime =
                scenarioTimes[i];

        double scenarioDetect =
                bestEpsilon * scenarioTime;

        double scenarioLiquid =
                (1-bestEpsilon) * scenarioTime;

        double scenarioBestCost =
                1e100;

        double bestUAVCostScenario = 1e100;
        double bestHeliCostScenario = 1e100;
        double bestPlaneCostScenario = 1e100;

        QString scenarioBestUAV;
        QString scenarioBestHeli;
        QString scenarioBestPlane;

        int scenarioBestUAVCount = 0;
        int scenarioBestHeliCount = 0;
        int scenarioBestPlaneCount = 0;



        double shoreDistance =
                qMax(
                    0.0,
                    distanceShore -
                    driftVelocity *
                    scenarioTime);

        double shoreFactor =
                1.0 -
                shoreDistance /
                distanceShore;

        if(shoreFactor < 0.0)
            shoreFactor = 0.0;

        if(shoreFactor > 1.0)
            shoreFactor = 1.0;

        double scenarioRisk =
                (oilVolume / 1000.0)
                *
                shoreFactor
                *
                (driftVelocity / 10.0)
                *
                vulnerabilityCoefficient
                ;
    };


    QString riskHtml;

    riskHtml =
            "<table width='100%'>"
            "<tr>";

    riskHtml =
            "<div style='font-size:20px;"
            "font-weight:bold;"
            "color:#7ffeff;"
            "text-align:center;"
            "margin-bottom:10px;'>"
            "Альтернативные сценарии"
            "</div>"
            "<br>"
            "Экологические риски"
            "<table width='100%'>"
            "<tr>";

    for(int i = 0; i < scenarios.size(); i++)
    {
        RiskScenario s =
                scenarios[i];

        riskHtml +=
                "<td valign='top' width='25%'>";

        riskHtml +=
                "<span style='color:"
                + s.color
                + "; font-size:22px;'>●</span> ";

        riskHtml +=
                "<b>"
                + s.level
                + "</b> ";

        riskHtml +=
                QString::number(
                    s.risk,
                    'f',
                    1)
                ;

        riskHtml +=
                "<br><br>";

        riskHtml +=
                QString::number(
                    s.time,
                    'f',
                    1)
                + " ч, ";

        riskHtml +=
                QString::number(
                    s.cost / 1000000.0,
                    'f',
                    2)
                + " млн руб.";

        riskHtml +=
                "<br><br>";

        riskHtml +=
                s.uav
                + " ("
                + QString::number(
                    s.uavCount)
                + ")";

        riskHtml +=
                "<br>";

        riskHtml +=
                s.heli
                + " ("
                + QString::number(
                    s.heliCount)
                + ")";

        riskHtml +=
                "<br>";

        riskHtml +=
                s.plane
                + " ("
                + QString::number(
                    s.planeCount)
                + ")";

        riskHtml += "</td>";
    }

    riskHtml +=
            "</tr>"
            "</table>";

    //Окно "Экологические риски"
        QLabel *riskPanel =
                new QLabel();
        riskPanel = new QLabel(this);
        riskPanel->setText(
                    riskHtml);
        riskPanel->setGeometry(
                    615,
                    0,
                    885,
                    230);

        riskPanel->setStyleSheet(
                    "QLabel{"
                    "background-color:rgba(4,14,28,120);"
                    "border:1px solid rgba(0,255,255,120);"
                    "border-radius:15px;"
                    "padding:15px;"
                    "color:white;"
                    "font-size:16px;"
                    "}"
                    );



    //--------------------------------------------------
    // СОХРАНЕНИЕ В RESULT
    //--------------------------------------------------

    QSqlQuery save(databaseManager.database());
    QSqlQuery clear(databaseManager.database());
    clear.exec("DELETE FROM Result");
    save.prepare(

                "INSERT INTO Result "
                "("
                "ID_Water,"
                "Oil_volume,"
                "Time,"
                "ID_UAVS,"
                "ID_Helicopters,"
                "ID_Airplanes,"
                "Count_UAVS,"
                "Count_Helicopters,"
                "Count_Airplanes,"
                "Costs,"
                "Risk"
                ") "
                "VALUES "
                "(?,?,?,?,?,?,?,?,?,?,?)"

                );

    int waterId = 1;

    if(waterType == "Море")
        waterId = 2;

    if(waterType == "Река")
        waterId = 3;


    save.addBindValue(waterId);
    save.addBindValue(oilVolume);
    save.addBindValue(timeLimit);

    save.addBindValue(bestUAVId);
    save.addBindValue(bestHeliId);
    save.addBindValue(bestPlaneId);

    save.addBindValue(bestUAVCount);
    save.addBindValue(bestHeliCount);
    save.addBindValue(bestPlaneCount);

    save.addBindValue(bestCost);

    save.addBindValue(riskValue);

    save.exec();

    QSqlQuery q(databaseManager.database());

    q.exec("SELECT * FROM Result LIMIT 1");

    if(q.next())
    {
        resultPanel->setText(
            QString(
            "ID_Water: %1\n"
            "Oil_volume: %2\n"
            "Time: %3\n"
            "ID_UAVS: %4\n"
            "ID_Helicopters: %5\n"
            "ID_Airplanes: %6\n"
            "Count_UAVS: %7\n"
            "Count_Helicopters: %8\n"
            "Count_Airplanes: %9\n"
            "Costs: %10\n"
            "Risk: %11")
            .arg(q.value(0).toString())
            .arg(q.value(1).toString())
            .arg(q.value(2).toString())
            .arg(q.value(3).toString())
            .arg(q.value(4).toString())
            .arg(q.value(5).toString())
            .arg(q.value(6).toString())
            .arg(q.value(7).toString())
            .arg(q.value(8).toString())
            .arg(q.value(9).toString())
            .arg(q.value(10).toString())
            );
    }
}

QWidget* OilSpillWindow::createLegendItem(
        const QColor &color,
        const QString &text)
{
    QWidget *item = new QWidget;

    QHBoxLayout *layout = new QHBoxLayout(item);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    QLabel *marker = new QLabel;
    marker->setFixedSize(14, 14);

    marker->setStyleSheet(QString(
        "background-color: %1;"
        "border-radius: 7px;"
        "border: 1px solid white;")
        .arg(color.name()));

    QLabel *label = new QLabel(text);

    label->setStyleSheet(
        "color: white;"
        "font-size: 12px;"
        "font-family: Bahnschrift;"
    );

    layout->addWidget(marker);
    layout->addWidget(label);

    return item;
}

void OilSpillWindow::createCharts()
{
    //--------------------------------------------------
    // БПЛА
    //--------------------------------------------------

    QBarSet *uavSet =
            new QBarSet("БПЛА");

    for(int i=0; i<uavCosts.size(); i++)
    {
        *uavSet << uavCosts[i] / 1000000.0;
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

    // Настройка фона и рамки в стиле HUD
    uav->setBackgroundPen(QPen(Qt::NoPen));  // Убираем собственную рамку
    uav->setBackgroundBrush(QBrush(QColor(255, 255, 255)));  // Белый фон
    uav->setPlotAreaBackgroundVisible(true);
    uav->setPlotAreaBackgroundBrush(QBrush(QColor(255, 255, 255)));

    QStringList cats1;

    for(int i=0; i<uavNames.size(); i++)
    {
        cats1 <<
            uavNames[i]
            + "\n("
            + QString::number(uavCounts[i])
            + ")";
    }

    QBarCategoryAxis *axis1 =
            new QBarCategoryAxis();

    axis1->append(cats1);
    uav->layout()->setContentsMargins(0, 0, 0, 0);
    uav->createDefaultAxes();

    uav->setAxisX(
                axis1,
                uavSeries);
    QFont axisFont;
    axisFont.setPointSize(5);

    axis1->setLabelsFont(axisFont);

    uav->legend()->hide();

    // Создаём QChartView без рамки
    uavChart = new QChartView(uav);
    uavChart->setStyleSheet("QChartView { border: none; background: transparent; }");

    // Создаём фрейм-контейнер с голубой рамкой
    QFrame *uavFrame = new QFrame(this);
    uavFrame->setGeometry(0, 750, 660, 250);
    uavFrame->setStyleSheet(
        "QFrame{"
        "    background-color: rgba(4,14,28,120);"
        "    border: 1px solid rgba(0,255,255,120);"
        "    border-radius: 15px;"
        "}"
    );

    // Используем layout для автоматического позиционирования
    QVBoxLayout *uavFrameLayout = new QVBoxLayout(uavFrame);
    uavFrameLayout->setContentsMargins(15, 15, 15, 15);  // Отступы внутри рамки
    uavFrameLayout->addWidget(uavChart);

    //--------------------------------------------------
    // ВК
    //--------------------------------------------------

    QBarSet *heliSet =
            new QBarSet("ВК");

    for(int i=0; i<heliCosts.size(); i++)
    {
        *heliSet << heliCosts[i] / 1000000.0;
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

    // Настройка фона и рамки в стиле HUD
    heli->setBackgroundPen(QPen(Qt::NoPen));  // Убираем собственную рамку
    heli->setBackgroundBrush(QBrush(QColor(255, 255, 255)));  // Белый фон
    heli->setPlotAreaBackgroundVisible(true);
    heli->setPlotAreaBackgroundBrush(QBrush(QColor(255, 255, 255)));

    QStringList cats2;

    for(int i=0; i<heliNames.size(); i++)
    {
        cats2 <<
            heliNames[i]
            + "\n("
            + QString::number(heliCounts[i])
            + ")";
    }

    QBarCategoryAxis *axis2 =
            new QBarCategoryAxis();

    axis2->append(cats2);
    heli->layout()->setContentsMargins(0, 0, 0, 0);
    heli->createDefaultAxes();

    heli->setAxisX(
                axis2,
                heliSeries);

    // axis2->setLabelsFont(axisFont);

    heli->legend()->hide();

    // Создаём QChartView без рамки
    heliChart = new QChartView(heli);
    heliChart->setStyleSheet("QChartView { border: none; background: transparent; }");

    // Создаём фрейм-контейнер с голубой рамкой
    QFrame *heliFrame = new QFrame(this);
    heliFrame->setGeometry(660, 750, 630, 250);
    heliFrame->setStyleSheet(
        "QFrame{"
        "    background-color: rgba(4,14,28,120);"
        "    border: 1px solid rgba(0,255,255,120);"
        "    border-radius: 15px;"
        "}"
    );

    // Используем layout для автоматического позиционирования
    QVBoxLayout *heliFrameLayout = new QVBoxLayout(heliFrame);
    heliFrameLayout->setContentsMargins(15, 15, 15, 15);  // Отступы внутри рамки
    heliFrameLayout->addWidget(heliChart);

    //--------------------------------------------------
    // АК
    //--------------------------------------------------

    QBarSet *planeSet =
            new QBarSet("АК");

    for(int i=0; i<planeCosts.size(); i++)
    {
        *planeSet << planeCosts[i] / 1000000.0;
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

    // Настройка фона и рамки в стиле HUD
    plane->setBackgroundPen(QPen(Qt::NoPen));  // Убираем собственную рамку
    plane->setBackgroundBrush(QBrush(QColor(255, 255, 255)));  // Белый фон
    plane->setPlotAreaBackgroundVisible(true);
    plane->setPlotAreaBackgroundBrush(QBrush(QColor(255, 255, 255)));

    QStringList cats3;

    for(int i=0; i<planeNames.size(); i++)
    {
        cats3 <<
            planeNames[i]
            + "\n("
            + QString::number(planeCounts[i])
            + ")";
    }

    QBarCategoryAxis *axis3 =
            new QBarCategoryAxis();

    axis3->append(cats3);
    plane->layout()->setContentsMargins(0, 0, 0, 0);
    plane->createDefaultAxes();

    plane->setAxisX(
                axis3,
                planeSeries);

    // axis3->setLabelsFont(axisFont);

    plane->legend()->hide();

    // Создаём QChartView без рамки
    planeChart = new QChartView(plane);
    planeChart->setStyleSheet("QChartView { border: none; background: transparent; }");

    // Создаём фрейм-контейнер с голубой рамкой
    QFrame *planeFrame = new QFrame(this);
    planeFrame->setGeometry(1290, 750, 620, 250);
    planeFrame->setStyleSheet(
        "QFrame{"
        "    background-color: rgba(4,14,28,120);"
        "    border: 1px solid rgba(0,255,255,120);"
        "    border-radius: 15px;"
        "}"
    );

    // Используем layout для автоматического позиционирования
    QVBoxLayout *planeFrameLayout = new QVBoxLayout(planeFrame);
    planeFrameLayout->setContentsMargins(15, 15, 15, 15);  // Отступы внутри рамки
    planeFrameLayout->addWidget(planeChart);

    //--------------------------------------------------
    // ОБЩИЙ ГРАФИК
    //--------------------------------------------------

    QLineSeries *series =
            new QLineSeries();

    double costMln =
            bestCost / 1000000.0; // для графика в млн руб а не просто руб



    for(int i = 0; i < graphTimes.size(); i++)
    {    qDebug() << "graphTimes=" << graphTimes;
        qDebug() << "graphCosts" << graphCosts;
        series->append(
            graphTimes[i],
            graphCosts[i]);
    }


    /*
    series->append(
            0,
            costMln * 1.2);

    series->append(
            bestOperationTime,
            costMln);

    series->append(
            bestOperationTime * 1.2,
            costMln * 1.1);
    */



    series->setColor(
                QColor(255,80,180));

    QChart *cost =
            new QChart();

    cost->addSeries(series);
/*
    for(int i = 0; i < riskCosts.size(); i++)
    {
        QLineSeries *riskLine =
                new QLineSeries();

        riskLine->append(
                    0,
                    riskCosts[i]);

        riskLine->append(
                    bestOperationTime * 1.2,
                    riskCosts[i]);

        QPen riskPen;
        riskPen.setColor(
                    QColor(
                        riskColors[i]));

        riskPen.setStyle(
                    Qt::DashLine);

        riskPen.setWidth(2);

        riskLine->setPen(
                    riskPen);

        cost->addSeries(
                    riskLine);
    }
    */

    for(int i = 0; i < scenarios.size(); i++)
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
                    scenarios[i].cost / 1000000.0);

        cost->addSeries(riskPoint);
    }







    //отдельная легенда для графика
    cost->legend()->hide();

    // Убираем внутренние отступы, чтобы график был вплотную к осям
    cost->layout()->setContentsMargins(0, 0, 0, 0);

    cost->createDefaultAxes();

    QValueAxis *axisX =
            qobject_cast<QValueAxis*>(cost->axisX());

    QValueAxis *axisY =
            qobject_cast<QValueAxis*>(cost->axisY());

    axisX->setRange(
                0,
                bestOperationTime * 1.1);




    /*
    double maxRiskCost = costMln;

    for(const auto &s : scenarios)
    {
        maxRiskCost = qMax(
                    maxRiskCost,
                    s.cost / 1000000.0);
    }
    axisY->setRange(
                0,
                maxRiskCost * 1.15);
*/
    double maxY = 0.0;

    for(double cost : graphCosts)
    {
        maxY = qMax(maxY, cost);
    }

    for(const auto &s : scenarios)
    {
        maxY = qMax(
                    maxY,
                    s.cost / 1000000.0);
    }

    axisY->setRange(
                0,
                maxY * 1.15);



    /*
    for(QAbstractSeries *s : cost->series())
    {
        s->attachAxis(
                    cost->axisX());

        s->attachAxis(
                    cost->axisY());
    }
    */

    cost->setTitle(
                "Затраты операции");

    QPen pen;
    pen.setColor(QColor(0, 105, 217));  // Розовый цвет
    pen.setWidth(3);                      // Толщина линии
    pen.setStyle(Qt::SolidLine);          // Сплошная линия
    series->setPen(pen);

    costChart = new QChartView(cost);
    costChart->setStyleSheet("QChartView { border: none; background: transparent; }");

    QFrame *costFrame = new QFrame(this);
    costFrame->setGeometry(0, 400, 600, 350);
    costFrame->setStyleSheet(
        "QFrame{"
        "    background-color: rgba(4,14,28,120);"
        "    border: 1px solid rgba(0,255,255,120);"
        "    border-radius: 15px;"
        "}"
    );

    // Используем layout для автоматического позиционирования
    QVBoxLayout *frameLayout = new QVBoxLayout(costFrame);

    QHBoxLayout *legendLayout = new QHBoxLayout;

    legendLayout->setSpacing(15);

   /* legendLayout->addWidget(
        createLegendItem(
            QColor(0, 105, 217),
            "Рациональный парк ВС"));
*/
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

    frameLayout->setContentsMargins(10, 10, 10, 10);

    frameLayout->addLayout(legendLayout);

    frameLayout->addWidget(costChart);


    //--------------------------------------------------
    // ТЕКСТ РЕЗУЛЬТАТОВ
    //--------------------------------------------------

    QString txt;

    txt += "<div align='center'>"
           "<span style='font-size:24px;"
           "font-weight:700;"
           "color:#7ffeff;'>"
           "Результат"
           "</span>"
           "</div>";

    txt += "Тип акватории: "
           + waterType
           + "<br>";

    txt += "Ограничение на время: "
           + QString::number(timeLimit)
           + " ч<br>";

    txt += "Объём выброса нефти: "
           + QString::number(oilVolume)
           + " м³<br>";

    txt += "Операция выполнена за: "
    + QString::number(bestOperationTime, 'f', 1)
    + " ч<br>";
/*
    txt +=
            "доля времени на обнаружение (ε): "
            + QString::number(
                bestEpsilon,
                'f',
                1)
            + "\n<br>";
    txt +=
            "доля времени на ликвидацию с ВК (β)= "
            + QString::number(
                bestBeta,
                'f',
                1)
            + "\n<br>";
*/
    txt += "<b>Рациональный парк</b><br>";

    txt += "БПЛА: "
           + bestUAV
           + " ("
           + QString::number(bestUAVCount)
           + ") — "
           + QString::number(
                bestUAVCost / 1000000.0,
                'f',
                2)
           + " млн руб.<br>";

    txt += "ВК: "
           + bestHeli
           + " ("
           + QString::number(bestHeliCount)
           + ") — "
           + QString::number(
                bestHeliCost / 1000000.0,
                'f',
                2)
           + " млн руб.<br>";

    txt += "АК: "
           + bestPlane
           + " ("
           + QString::number(bestPlaneCount)
           + ") — "
           + QString::number(
                bestPlaneCost / 1000000.0,
                'f',
                2)
           + " млн руб.<br>";

    txt += "<b>Затраты</b><br>";

    txt += "Эксплуатация: "
           + QString::number(
                operationCost / 1000000.0,
                'f',
                2)
           + " млн руб.<br>";

    txt += "Топливо: "
           + QString::number(
                fuelCost / 1000000.0,
                'f',
                2)
           + " млн руб.<br>";

    txt += "Материалы: "
           + QString::number(
                materialCost / 1000000.0,
                'f',
                2)
           + " млн руб.<br>";

  //  txt += "<br>";

    txt +=
            "Экологический риск: ";

    txt +=
            "<span style='color:"
            + riskColor
            + ";'>●</span> ";

    txt +=
            riskLevel
            + " ("
            + QString::number(
                riskValue,
                'f',
                1)
            + ")";

    txt += "<br><b>Итого: "
           + QString::number(
                bestCost / 1000000.0,
                'f',
                2)
           + " млн руб.</b>";

    resultPanel->setText(txt);

    resultPanel->setGeometry(
                0,
                0,
                600,
                390);
}
