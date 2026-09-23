#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager
{
public:
    DatabaseManager();

    bool connect();
    QSqlDatabase database() const;

private:
    QSqlDatabase m_database;
};

#endif // DATABASEMANAGER_H
