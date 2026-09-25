#include "oilsimulation.h"

#include <QtMath>
#include <cmath>

OilSimulation::OilSimulation()
{
}

void OilSimulation::initialize(
        double oilVolume,
        double initialThickness,
        double minThickness,
        double kSpread,
        double driftVelocity,
        double driftAngle)
{
    m_oilVolume = oilVolume;

    m_minThickness = minThickness;

    m_currentArea =
            m_oilVolume /
            (initialThickness * 1e-3);

    m_maxArea =
            m_oilVolume /
            (m_minThickness * 1e-3);

    m_oilThickness =
            initialThickness;

    m_kSpread = kSpread;

    m_driftVelocity = driftVelocity;
    m_driftAngle = driftAngle;

    m_worldX = 0.0;
    m_worldY = 0.0;

    m_spreadingFinished = false;

    m_perimeter =
            2.0 *
            M_PI *
            sqrt(
                m_currentArea /
                M_PI);

    m_particles.clear();

    int particleCount =
            qBound(
                300,
                int(m_oilVolume * 4),
                3000);

    for(int i = 0; i < particleCount; i++)
    {
        double angle =
                degToRad(
                    qrand() % 360);

        double radius =
                qSqrt(
                    qrand() % 1000)
                * 1.8;

        OilParticle p;

        p.x =
                cos(angle) *
                radius;

        p.y =
                sin(angle) *
                radius;

        p.vx = 0.0;
        p.vy = 0.0;

        m_particles.push_back(p);
    }
}

void OilSimulation::update(
        double simulationSpeed)
{
    if(!m_spreadingFinished)
    {
        double dt =
                0.05 *
                simulationSpeed;

        m_currentArea +=
                M_PI *
                m_kSpread *
                1000000.0 *
                dt;

        if(m_currentArea >= m_maxArea)
        {
            m_currentArea =
                    m_maxArea;

            m_spreadingFinished =
                    true;
        }

        m_oilThickness =
                m_oilVolume /
                (m_currentArea * 1e-3);

        if(m_oilThickness <
                m_minThickness)
        {
            m_oilThickness =
                    m_minThickness;
        }

        m_perimeter =
                2.0 *
                M_PI *
                sqrt(
                    m_currentArea /
                    M_PI);
    }

    m_worldX +=
            cos(m_driftAngle)
            *
            m_driftVelocity
            *
            1.15
            *
            simulationSpeed;

    m_worldY +=
            sin(m_driftAngle)
            *
            m_driftVelocity
            *
            1.15
            *
            simulationSpeed;

    for(int i = 0;
        i < m_particles.size();
        i++)
    {
        OilParticle &p =
                m_particles[i];

        if(!m_spreadingFinished)
        {
            double ang =
                    degToRad(
                        qrand() % 360);

            double spread =
                    0.08
                    +
                    (qrand() % 100)
                    / 900.0;

            p.vx +=
                    cos(ang)
                    *
                    spread;

            p.vy +=
                    sin(ang)
                    *
                    spread;

            p.vx +=
                    cos(m_driftAngle)
                    *
                    0.05;

            p.vy +=
                    sin(m_driftAngle)
                    *
                    0.05;
        }
        else
        {
            p.vx +=
                    cos(m_driftAngle)
                    *
                    0.03;

            p.vy +=
                    sin(m_driftAngle)
                    *
                    0.03;
        }

        p.vx *= 0.988;
        p.vy *= 0.988;

        p.x += p.vx;
        p.y += p.vy;

        p.x +=
                cos(m_driftAngle)
                *
                m_driftVelocity
                *
                0.65
                *
                simulationSpeed;

        p.y +=
                sin(m_driftAngle)
                *
                m_driftVelocity
                *
                0.65
                *
                simulationSpeed;
    }
}

const QVector<OilParticle>&
OilSimulation::particles() const
{
    return m_particles;
}

double OilSimulation::currentArea() const
{
    return m_currentArea;
}

double OilSimulation::maxArea() const
{
    return m_maxArea;
}

double OilSimulation::oilThickness() const
{
    return m_oilThickness;
}

double OilSimulation::perimeter() const
{
    return m_perimeter;
}

double OilSimulation::worldX() const
{
    return m_worldX;
}

double OilSimulation::worldY() const
{
    return m_worldY;
}

bool OilSimulation::spreadingFinished() const
{
    return m_spreadingFinished;
}

double OilSimulation::degToRad(double deg)
{
    return deg * M_PI / 180.0;
}
