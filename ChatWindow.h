#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include "DatabaseLogger.h"

class ChatWindow : public QWidget {
    Q_OBJECT

public:
    ChatWindow(const QString& dbType, const QString& dbHost, int dbPort,
               const QString& dbName, const QString& dbUser, const QString& dbPass,
               QWidget *parent = nullptr);
    ~ChatWindow();

private slots:
    void startServer();
    void connectToPeer();
    void sendMessage();
    void newConnection();
    void readMessage();
    void peerDisconnected();
    void socketError(QAbstractSocket::SocketError error);

private:
    void appendMessage(const QString& sender, const QString& message);
    void setupUI();

    DatabaseLogger* dbLogger;

    QTcpServer* tcpServer;
    QTcpSocket* tcpSocket;

    QTextEdit* chatDisplay;
    QLineEdit* messageInput;
    QPushButton* sendButton;

    QLineEdit* peerHostInput;
    QSpinBox* peerPortInput;
    QPushButton* connectButton;

    QSpinBox* listenPortInput;
    QPushButton* listenButton;

    QLineEdit* dbNameInput;

    QString m_dbType;
    QString m_dbHost;
    int m_dbPort;
    QString m_dbName;
    QString m_dbUser;
    QString m_dbPass;
};

#endif // CHATWINDOW_H
