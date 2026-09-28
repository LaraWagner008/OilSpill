#include "hudrenderer.h"

#include <QString>
#include <QtMath>

HUDRenderer::HUDRenderer(
        QLabel *hud,
        OilSimulation *simulation)
    : m_hud(hud),
      m_simulation(simulation)
{
}

void HUDRenderer::draw(
        double driftVelocity,
        double driftAngle)
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
                driftAngle * 180 / M_PI,
                'f',
                1)
            + "°</b><br><br>";

    text +=
            "⌖ Координаты<br>"
            "<b>X: "
            + QString::number(
                m_simulation->worldX(),
                'f',
                1)
            + "<br>Y: "
            + QString::number(
                m_simulation->worldY(),
                'f',
                1)
            + "</b><br><br>";

    text +=
            "⬒ Площадь<br>"
            "<b>"
            + QString::number(
                m_simulation->currentArea() / 1000000.0,
                'f',
                2)
            + " км²</b><br><br>";

    text +=
            "◌ Толщина плёнки<br>"
            "<b>"
            + QString::number(
                m_simulation->oilThickness(),
                'f',
                4)
            + " мм</b><br><br>";

    text +=
            "◎ Периметр<br>"
            "<b>"
            + QString::number(
                m_simulation->perimeter() / 1000.0,
                'f',
                2)
            + " км</b>";

    m_hud->setText(text);
}
