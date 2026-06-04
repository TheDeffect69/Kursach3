#include "ChatWindow.h"
#include <QHostAddress>
#include <QMessageBox>

ChatWindow::ChatWindow(QWidget *parent)
    : QWidget(parent), dbLogger(nullptr), tcpServer(new QTcpServer(this)), tcpSocket(nullptr) {

    setupUI();

    connect(tcpServer, &QTcpServer::newConnection, this, &ChatWindow::newConnection);
}

ChatWindow::~ChatWindow() {
    delete dbLogger;
}

void ChatWindow::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Database setup layout
    QHBoxLayout* dbLayout = new QHBoxLayout();
    dbLayout->addWidget(new QLabel("Файл базы данных:"));
    dbNameInput = new QLineEdit("chat_db.sqlite");
    dbLayout->addWidget(dbNameInput);
    mainLayout->addLayout(dbLayout);

    // Server setup layout
    QHBoxLayout* serverLayout = new QHBoxLayout();
    serverLayout->addWidget(new QLabel("Порт прослушивания:"));
    listenPortInput = new QSpinBox();
    listenPortInput->setRange(1024, 65535);
    listenPortInput->setValue(12345);
    serverLayout->addWidget(listenPortInput);
    listenButton = new QPushButton("Начать прослушивание");
    connect(listenButton, &QPushButton::clicked, this, &ChatWindow::startServer);
    serverLayout->addWidget(listenButton);
    mainLayout->addLayout(serverLayout);

    // Client setup layout
    QHBoxLayout* clientLayout = new QHBoxLayout();
    clientLayout->addWidget(new QLabel("IP хоста:"));
    peerHostInput = new QLineEdit("127.0.0.1");
    clientLayout->addWidget(peerHostInput);
    clientLayout->addWidget(new QLabel("Порт хоста:"));
    peerPortInput = new QSpinBox();
    peerPortInput->setRange(1024, 65535);
    peerPortInput->setValue(12345);
    clientLayout->addWidget(peerPortInput);
    connectButton = new QPushButton("Подключиться");
    connect(connectButton, &QPushButton::clicked, this, &ChatWindow::connectToPeer);
    clientLayout->addWidget(connectButton);
    mainLayout->addLayout(clientLayout);

    // Chat display
    chatDisplay = new QTextEdit();
    chatDisplay->setReadOnly(true);
    mainLayout->addWidget(chatDisplay);

    // Message input
    QHBoxLayout* messageLayout = new QHBoxLayout();
    messageInput = new QLineEdit();
    connect(messageInput, &QLineEdit::returnPressed, this, &ChatWindow::sendMessage);
    messageLayout->addWidget(messageInput);
    sendButton = new QPushButton("Отправить");
    connect(sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);
    messageLayout->addWidget(sendButton);
    mainLayout->addLayout(messageLayout);

    setLayout(mainLayout);
    setWindowTitle("Qt P2P Чат");
    resize(500, 400);
}

void ChatWindow::startServer() {
    if (!dbLogger) {
        dbLogger = new DatabaseLogger("QSQLITE", "", -1, dbNameInput->text(), "", "");
        dbNameInput->setEnabled(false);
    }

    int port = listenPortInput->value();
    if (tcpServer->listen(QHostAddress::Any, port)) {
        appendMessage("Система", "Прослушивание порта " + QString::number(port) + " начато");
        listenButton->setEnabled(false);
        listenPortInput->setEnabled(false);
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось запустить сервер: " + tcpServer->errorString());
    }
}

void ChatWindow::connectToPeer() {
    if (!dbLogger) {
        dbLogger = new DatabaseLogger("QSQLITE", "", -1, dbNameInput->text(), "", "");
        dbNameInput->setEnabled(false);
    }

    if (tcpSocket) {
        tcpSocket->disconnect(); // disconnect signals to avoid calling peerDisconnected synchronously
        tcpSocket->disconnectFromHost();
        tcpSocket->deleteLater();
        tcpSocket = nullptr;
    }

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &ChatWindow::readMessage);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &ChatWindow::peerDisconnected);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(tcpSocket, &QAbstractSocket::errorOccurred, this, &ChatWindow::socketError);
#else
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ChatWindow::socketError);
#endif

    QString host = peerHostInput->text();
    int port = peerPortInput->value();
    tcpSocket->connectToHost(host, port);
    appendMessage("Система", "Подключение к " + host + ":" + QString::number(port) + "...");
}

void ChatWindow::newConnection() {
    if (tcpSocket) {
        // We already have a connection, ignore new ones or handle them
        // For simplicity, we just drop the old connection and accept the new one.
        tcpSocket->disconnect(); // disconnect signals to avoid calling peerDisconnected synchronously
        tcpSocket->disconnectFromHost();
        tcpSocket->deleteLater();
        tcpSocket = nullptr;
    }

    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket, &QTcpSocket::readyRead, this, &ChatWindow::readMessage);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &ChatWindow::peerDisconnected);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(tcpSocket, &QAbstractSocket::errorOccurred, this, &ChatWindow::socketError);
#else
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ChatWindow::socketError);
#endif

    appendMessage("Система", "Пользователь подключился с адреса " + tcpSocket->peerAddress().toString());
}

void ChatWindow::readMessage() {
    QByteArray data = tcpSocket->readAll();
    QString message = QString::fromUtf8(data);
    appendMessage("Собеседник", message);
    if (dbLogger) {
        dbLogger->logMessage("IN", message);
    }
}

void ChatWindow::sendMessage() {
    QString message = messageInput->text();
    if (message.isEmpty() || !tcpSocket || tcpSocket->state() != QAbstractSocket::ConnectedState) {
        return;
    }

    tcpSocket->write(message.toUtf8());
    tcpSocket->flush();
    appendMessage("Я", message);
    messageInput->clear();

    if (dbLogger) {
        dbLogger->logMessage("OUT", message);
    }
}

void ChatWindow::appendMessage(const QString& sender, const QString& message) {
    chatDisplay->append("<b>" + sender + ":</b> " + message);
}

void ChatWindow::peerDisconnected() {
    appendMessage("Система", "Собеседник отключился.");
    tcpSocket->deleteLater();
    tcpSocket = nullptr;
}

void ChatWindow::socketError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);
    if (tcpSocket) {
        appendMessage("Система", "Ошибка соединения: " + tcpSocket->errorString());
    }
}
