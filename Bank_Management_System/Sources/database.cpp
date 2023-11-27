#include "database.h"

QSqlDatabase Database::DB_Conn = QSqlDatabase::addDatabase("QSQLITE");

Database& Database::instance()
{
    static Database instance;
    return instance;
}

Database::Database(QObject *parent) : QObject(parent)
{

    DB_Conn.setDatabaseName("your_path/Database.db");

    if(!DB_Conn.open())
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
    if(DB_Conn.isOpen() && !DB_Conn.open())
    {
        qDebug() << "Database Open Error: " << DB_Conn.lastError().text();
    }

    return DB_Conn;
}
