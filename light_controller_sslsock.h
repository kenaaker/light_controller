#ifndef DISPENSARY_SSLSOCK_H
#define DISPENSARY_SSLSOCK_H

#include <QTcpServer>
#include <QSslSocket>
#include <QList>
#include <QDomDocument>
#include <QtSql>

class kennel_fan_sslsock : public QTcpServer {
    Q_OBJECT

    void incomingConnection(qintptr socketDescriptor);
    void send_line(QString &a_line);
    QString recv_line(void);

signals:
    void command_received(QString &cmd);

private:

    QSslSocket *socket;
    QSqlDatabase db;
    QString current_command;


    void startServerEncryption();
    void connectSocketSignals();
    void connect_to_db();

private slots:

    void slot_encrypted();
    void slot_encryptedBytesWritten(qint64 written);
    void slot_modeChanged(QSslSocket::SslMode mode);
    void slot_peerVerifyError(const QSslError &error);
    void slot_sslErrors(const QList<QSslError> &errors);

    void slot_connected();
    void slot_disconnected();
    void slot_error(QAbstractSocket::SocketError);
    void slot_hostFound();
    void slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *);
    void slot_stateChanged(QAbstractSocket::SocketState);

    void slot_readyRead();
};

#endif // DISPENSARY_SSLSOCK_H
