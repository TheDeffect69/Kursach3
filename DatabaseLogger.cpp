#include "DatabaseLogger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QUuid>

DatabaseLogger::DatabaseLogger(const QString& dbType, const QString& hostName, int port,
                               const QString& databaseName, const QString& userName, const QString& password) {
    connectionName = QUuid::createUuid().toString();
    db = QSqlDatabase::addDatabase(dbType, connectionName);
    db.setHostName(hostName);
    if (port > 0) db.setPort(port);
    db.setDatabaseName(databaseName);
    db.setUserName(userName);
    db.setPassword(password);

    connected = db.open();
    if (!connected) {
        qDebug() << "Error: Could not connect to database:" << db.lastError().text();
    } else {
        qDebug() << "Connected to database successfully.";
        QSqlQuery query(db);
        if (!query.exec("CREATE TABLE IF NOT EXISTS chat_logs ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
                        "type TEXT,"
                        "message TEXT"
                        ")")) {
            // It might fail for PostgreSQL if we use INTEGER PRIMARY KEY AUTOINCREMENT (which is SQLite syntax)
            // Let's try an alternative for PostgreSQL or fallback
            if(dbType == "QPSQL") {
               query.exec("CREATE TABLE IF NOT EXISTS chat_logs ("
                            "id SERIAL PRIMARY KEY,"
                            "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                            "type TEXT,"
                            "message TEXT"
                            ")");
            } else {
                qDebug() << "Warning: Could not create table:" << query.lastError().text();
            }
        }
    }
}

DatabaseLogger::~DatabaseLogger() {
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
}

bool DatabaseLogger::isConnected() const {
    return connected;
}

bool DatabaseLogger::logMessage(const QString& type, const QString& message) {
    if (!connected) return false;

    QSqlQuery query(db);
    query.prepare("INSERT INTO chat_logs (timestamp, type, message) VALUES (:timestamp, :type, :message)");
    query.bindValue(":timestamp", QDateTime::currentDateTime());
    query.bindValue(":type", type);
    query.bindValue(":message", message);

    if (!query.exec()) {
        qDebug() << "Error: Could not log message:" << query.lastError().text();
        return false;
    }
    return true;
}
