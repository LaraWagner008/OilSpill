#include "databasemanager.h"

#include <QSqlError>
#include <QDebug>

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
