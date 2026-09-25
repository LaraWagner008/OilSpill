#ifndef OILSIMULATION_H
#define OILSIMULATION_H

#include <QVector>

struct OilParticle
{
    double x = 0.0;
    double y = 0.0;

    double vx = 0.0;
    double vy = 0.0;
};

class OilSimulation
{
public:
    OilSimulation();

    void initialize(
            double oilVolume,
            double initialThickness,
            double minThickness,
            double kSpread,
            double driftVelocity,
            double driftAngle);

    void update(double simulationSpeed);

    const QVector<OilParticle>& particles() const;

    double currentArea() const;
    double maxArea() const;
    double oilThickness() const;
    double perimeter() const;

    double worldX() const;
    double worldY() const;

    bool spreadingFinished() const;

private:
    QVector<OilParticle> m_particles;

    double m_oilVolume = 0.0;

    double m_minThickness = 0.0;

    double m_currentArea = 0.0;
    double m_maxArea = 0.0;

    double m_oilThickness = 0.0;
    double m_perimeter = 0.0;

    double m_kSpread = 0.0;

    double m_driftVelocity = 0.0;
    double m_driftAngle = 0.0;

    double m_worldX = 0.0;
    double m_worldY = 0.0;

    bool m_spreadingFinished = false;

    double degToRad(double deg);
};

#endif // OILSIMULATION_H
