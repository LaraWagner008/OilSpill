#include "databasemanager.h"

#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

DatabaseManager::DatabaseManager()
{
}

bool DatabaseManager::connect()
{
    if(QSqlDatabase::contains("oil_connection"))
    {
        m_database =
                QSqlDatabase::database(
                    "oil_connection");
    }
    else
    {
        m_database =
                QSqlDatabase::addDatabase(
                    "QSQLITE",
                    "oil_connection");

        m_database.setDatabaseName(
                    "D:/OilProject/OilSpillSystem/database.db");
    }

    if(!m_database.open())
    {
        qDebug()
                << m_database.lastError().text();

        return false;
    }

    return true;
}

QSqlDatabase DatabaseManager::database() const
{
    return m_database;
}

OperationConstants DatabaseManager::loadOperationConstants() const
{
    OperationConstants constants;

    if(!m_database.isOpen())
        return constants;

    QSqlQuery query(m_database);

    query.exec("SELECT * FROM ConstData");

    qDebug()
            << "ConstData exec ="
            << query.lastError().text();

    if(!query.next())
    {
        qDebug()
                << "ConstData EMPTY";

        return constants;
    }

    constants.viewingAngle =
            query.value(1).toDouble();

    constants.flightHeight =
            query.value(2).toDouble();

    constants.loadingTime =
            query.value(3).toDouble();

    constants.weightBooms =
            query.value(4).toDouble();

    constants.sprayRate =
            query.value(5).toDouble();

    constants.density =
            query.value(6).toDouble();

    constants.boomSpeed =
            query.value(7).toDouble();

    constants.boomPrice =
            query.value(8).toDouble();

    constants.dispersantPrice =
            query.value(9).toDouble();


    constants.fuelPriceUAV =
            query.value(7).toDouble();

    constants.fuelPriceHeli =
            query.value(8).toDouble();

    constants.fuelPricePlane =
            query.value(8).toDouble();

    return constants;
}

AircraftDatabase DatabaseManager::loadAircraftDatabase() const
{
    AircraftDatabase data;

    if(!m_database.isOpen())
        return data;

    QSqlQuery uavQuery(m_database);

    uavQuery.exec(
                "SELECT * FROM UAVS");

    while(uavQuery.next())
    {
        UAVData uav;

        uav.id =
                uavQuery.value(0).toInt();

        uav.name =
                uavQuery.value(1).toString();

        uav.speed =
                uavQuery.value(2).toDouble();

        uav.range =
                uavQuery.value(3).toDouble();

        uav.fuelWeight =
                uavQuery.value(4).toDouble();

        uav.fuelConsumption =
                uavQuery.value(5).toDouble();

        uav.cost =
                uavQuery.value(6).toDouble();

        data.uavs.append(uav);
    }

    QSqlQuery heliQuery(m_database);

    heliQuery.exec(
                "SELECT * FROM Helicopters");

    while(heliQuery.next())
    {
        HelicopterData heli;

        heli.id =
                heliQuery.value(0).toInt();

        heli.name =
                heliQuery.value(1).toString();

        heli.speed =
                heliQuery.value(2).toDouble();

        heli.range =
                heliQuery.value(3).toDouble();

        heli.capacity =
                heliQuery.value(4).toDouble();

        heli.fuelWeight =
                heliQuery.value(5).toDouble();

        heli.fuelConsumption =
                heliQuery.value(6).toDouble();

        heli.cost =
                heliQuery.value(7).toDouble();

        data.helicopters.append(heli);
    }

    QSqlQuery planeQuery(m_database);

    planeQuery.exec(
                "SELECT * FROM Airplanes");

    while(planeQuery.next())
    {
        AirplaneData plane;

        plane.id =
                planeQuery.value(0).toInt();

        plane.name =
                planeQuery.value(1).toString();

        plane.speed =
                planeQuery.value(2).toDouble();

        plane.range =
                planeQuery.value(3).toDouble();

        plane.capacity =
                planeQuery.value(4).toDouble();

        plane.sprayWidth =
                planeQuery.value(5).toDouble();

        plane.fuelWeight =
                planeQuery.value(6).toDouble();

        plane.fuelConsumption =
                planeQuery.value(7).toDouble();

        plane.cost =
                planeQuery.value(8).toDouble();

        data.airplanes.append(plane);
    }

    return data;
}
