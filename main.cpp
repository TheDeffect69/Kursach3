#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "ChatWindow.h"
#include "DatabaseLogger.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("QtChatApp");
    app.setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("P2P Chat Application with Database Logging");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption dbTypeOption(QStringList() << "t" << "dbtype",
        "Database driver type (e.g., QSQLITE, QPSQL).", "type", "QSQLITE");
    parser.addOption(dbTypeOption);

    QCommandLineOption dbHostOption(QStringList() << "H" << "dbhost",
        "Database host name.", "host", "");
    parser.addOption(dbHostOption);

    QCommandLineOption dbPortOption(QStringList() << "P" << "dbport",
        "Database port.", "port", "-1");
    parser.addOption(dbPortOption);

    QCommandLineOption dbNameOption(QStringList() << "d" << "dbname",
        "Database name (or path for SQLite).", "name", "chat_db.sqlite");
    parser.addOption(dbNameOption);

    QCommandLineOption dbUserOption(QStringList() << "u" << "dbuser",
        "Database user name.", "user", "");
    parser.addOption(dbUserOption);

    QCommandLineOption dbPassOption(QStringList() << "p" << "dbpass",
        "Database password.", "password", "");
    parser.addOption(dbPassOption);

    parser.process(app);

    QString dbType = parser.value(dbTypeOption);
    QString dbHost = parser.value(dbHostOption);
    int dbPort = parser.value(dbPortOption).toInt();
    QString dbName = parser.value(dbNameOption);
    QString dbUser = parser.value(dbUserOption);
    QString dbPass = parser.value(dbPassOption);

    DatabaseLogger logger(dbType, dbHost, dbPort, dbName, dbUser, dbPass);

    ChatWindow window(&logger);
    window.show();

    return app.exec();
}
