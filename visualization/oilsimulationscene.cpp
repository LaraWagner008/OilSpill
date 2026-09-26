#include "oilsimulationscene.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QtMath>

OilSimulationScene::OilSimulationScene(
        QGraphicsScene *scene,
        OilSimulation *simulation)
    : m_scene(scene),
      m_simulation(simulation)
{
}

void OilSimulationScene::drawWater()
{
    m_scene->clear();

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

    m_scene->addRect(
                m_scene->sceneRect(),
                QPen(Qt::NoPen),
                QBrush(water));

    // =====================================================
    // GRID
    // =====================================================

    QPen grid(
                QColor(0,255,255,23));

    for(int x=-200000; x<200000; x+=250)
    {
        m_scene->addLine(
                    x,-200000,
                    x,200000,
                    grid);
    }

    for(int y=-200000; y<200000; y+=250)
    {
        m_scene->addLine(
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
        m_scene->addLine(
                    i,
                    -200000,
                    i+80,
                    -199920,
                    corner);
    }
}

void OilSimulationScene::drawOil()
{
    // =================================================
    // MAIN OIL SILHOUETTE
    // =================================================

    const QVector<OilParticle>& particles =
            m_simulation->particles();

    for(int i = 0;
        i < particles.size();
        i++)
    {
        const OilParticle &p =
                particles[i];

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

        m_scene->addEllipse(
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

        m_scene->addEllipse(
                    p.x-75,
                    p.y-75,
                    150,
                    150,
                    QPen(Qt::NoPen),
                    QBrush(oil));
    }
}

void OilSimulationScene::updateCamera(
        QGraphicsView *view,
        double driftAngle)
{
    double minX = 999999;
    double maxX = -999999;

    double minY = 999999;
    double maxY = -999999;

    const QVector<OilParticle>& particles =
            m_simulation->particles();

    for(int i = 0;
        i < particles.size();
        i++)
    {
        const OilParticle &p =
                particles[i];

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
            (minX + maxX) / 2.0;

    double oilCenterY =
            (minY + maxY) / 2.0;

    // LOOK AHEAD

    double lookAhead = 340;

    double targetX =
            oilCenterX
            +
            cos(driftAngle)
            *
            lookAhead;

    double targetY =
            oilCenterY
            +
            sin(driftAngle)
            *
            lookAhead;

    // SOFT CAMERA

    m_cameraX +=
            (targetX - m_cameraX)
            *
            0.03;

    m_cameraY +=
            (targetY - m_cameraY)
            *
            0.03;

    // KEEP OIL IN SCREEN

    view->fitInView(
                QRectF(
                    minX - 700,
                    minY - 700,
                    oilWidth + 1400,
                    oilHeight + 1400),
                Qt::KeepAspectRatio);

    view->centerOn(
            m_cameraX,
            m_cameraY);
}

double OilSimulationScene::cameraX() const
{
    return m_cameraX;
}

double OilSimulationScene::cameraY() const
{
    return m_cameraY;
}
