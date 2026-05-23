#ifndef DATABASELOGGER_H
#define DATABASELOGGER_H

#include <QString>
#include <QSqlDatabase>
#include <QDateTime>

class DatabaseLogger {
public:
    DatabaseLogger(const QString& dbType, const QString& hostName, int port,
                   const QString& databaseName, const QString& userName, const QString& password);
    ~DatabaseLogger();

    bool isConnected() const;
    bool logMessage(const QString& type, const QString& message);

private:
    QSqlDatabase db;
    bool connected;
    QString connectionName;
};

#endif // DATABASELOGGER_H
