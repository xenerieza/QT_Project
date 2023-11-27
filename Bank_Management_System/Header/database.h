#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
class Database : public QObject
{
    Q_OBJECT

public:
    static Database& instance();
    static QSqlDatabase &getDatabase();


private:
    Database(QObject *parent = nullptr);
    ~Database();
    Database(const Database&) = delete;
    static QSqlDatabase DB_Conn;

};

#endif // DATABASE_H
