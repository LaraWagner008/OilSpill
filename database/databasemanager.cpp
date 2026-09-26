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
