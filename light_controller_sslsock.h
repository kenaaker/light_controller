#ifndef LIGHT_CONTROLLER_SSLSOCK_H
#define LIGHT_CONTROLLER_SSLSOCK_H

#include <QTcpServer>
#include <QSslSocket>
#include <QList>
#include <QDomDocument>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

class light_controller_sslsock : public QTcpServer {
    Q_OBJECT
public:
    void incomingConnection(qintptr socketDescriptor);
    void send_line(QString &a_line);
    QString recv_line(void);

signals:
    void command_received(QString &cmd);

private:

    QSslSocket *socket;
    QString current_command;

    void startServerEncryption();
    void connectSocketSignals();

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

#endif // LIGHT_CONTROLLER_SSLSOCK_H
