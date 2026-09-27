#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include "../models/operationconstants.h"
#include "../models/aircraftdatabase.h"

class DatabaseManager
{
public:
    DatabaseManager();

    bool connect();
    QSqlDatabase database() const;
    OperationConstants loadOperationConstants() const;
    AircraftDatabase loadAircraftDatabase() const;

private:
    QSqlDatabase m_database;
};

#endif // DATABASEMANAGER_H
