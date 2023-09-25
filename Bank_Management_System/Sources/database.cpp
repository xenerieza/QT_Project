#include "database.h"
QSqlDatabase Database::DB_Conn = QSqlDatabase::addDatabase("QSQLITE");

Database& Database::instance()
{
    static Database instance;
    return instance;
}

Database::Database(QObject *parent) : QObject(parent)
{
    DB_Conn.setDatabaseName("yourpath/Database.db");

    if(!DB_Conn.isValid())
    {
        qDebug() << "Database Connection failed" << DB_Conn.lastError().text();
    }
}

Database::~Database()
{
    if (DB_Conn.isOpen())
    {
        DB_Conn.close();
    }
}

QSqlDatabase& Database::getDatabase()
{
    return DB_Conn;
}
