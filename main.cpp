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

    parser.process(app);

    ChatWindow window;
    window.show();

    return app.exec();
}
