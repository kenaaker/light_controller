#include "light_controller_sslsock.h"
#include <QSslKey>
#include <QSettings>
#include <QFile>
#include <QApplication>

#define CACERTIFICATES_FILE ":/light_controller_ca.pem"
#define LOCALCERTIFICATE_FILE ":/light_controller_server.pem"
#define PRIVATEKEY_FILE ":/light_controller_server.key"

void light_controller_sslsock::incomingConnection(qintptr socketDescriptor) {

    qDebug("%s:%d Socket Descriptor is %d", __func__, __LINE__, socketDescriptor);
    socket = new QSslSocket();
    if (!socket) {
        qWarning("not enough memory to create new QSslSocket");
    } else {
        socket->setProtocol(QSsl::AnyProtocol);
        connectSocketSignals();
        if (!socket->setSocketDescriptor(socketDescriptor)) {
            qWarning("couldn't set socket descriptor");
            delete socket;
        } else {
            addPendingConnection(socket);
            startServerEncryption();
        } /* endif */
    } /* endif */
} /* incomingConnection */

void light_controller_sslsock::send_line(QString &a_line) {
    socket->write(a_line.toLatin1(), a_line.size());
}

QString light_controller_sslsock::recv_line() {
    QByteArray in_buff(256, ' ');
    socket->readLine(in_buff.size());
    return QString(in_buff);
}

void light_controller_sslsock::startServerEncryption() {
    bool b;
    QFile file(PRIVATEKEY_FILE);

    qDebug() << __func__ << " Entered socket state is " << socket->state();
    file.open(QIODevice::ReadOnly);
    if (!file.isOpen()) {
        qWarning("couldn't open %s", PRIVATEKEY_FILE);
        socket->disconnectFromHost();
    } else {
        QSslKey key(&file, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, "");
        qDebug() << "Set up key";
        if (key.isNull()) {
            qDebug() << "Key isn't good.";
            qWarning("key is null");
            socket->disconnectFromHost();
            return;
        } else {
            b = socket->addCaCertificates(CACERTIFICATES_FILE);
            qDebug() << "CA setup";
            if (!b) {
                qWarning("Couldn't add CA certificates (\"%s\")", CACERTIFICATES_FILE);
            } else {
                qDebug() << "CA added.";
                socket->setLocalCertificate(LOCALCERTIFICATE_FILE);
                socket->setPrivateKey(key);
                socket->setPeerVerifyMode(QSslSocket::VerifyPeer);
                socket->startServerEncryption();
                qDebug() << "Started Encryption.";
            }
        }
    }
}

void light_controller_sslsock::connectSocketSignals() {

    qDebug() << __func__ << " Entered socket state is " << socket->state();

    connect(socket, SIGNAL(encrypted()), this, SLOT(slot_encrypted()));
    connect(socket, SIGNAL(encryptedBytesWritten(qint64)),
            this, SLOT(slot_encryptedBytesWritten(qint64)));
    connect(socket, SIGNAL(modeChanged(QSslSocket::SslMode)),
            this, SLOT(slot_modeChanged(QSslSocket::SslMode)));
    connect(socket, SIGNAL(peerVerifyError(const QSslError &)),
            this, SLOT(slot_peerVerifyError (const QSslError &)));
    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)),
            this, SLOT(slot_sslErrors(const QList<QSslError> &)));
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(slot_readyRead()));
    connect(socket, SIGNAL(connected()),
            this, SLOT(slot_connected()));
    connect(socket, SIGNAL(disconnected()),
            this, SLOT(slot_disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slot_error(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(hostFound()),
            this, SLOT(slot_hostFound()));
    connect(socket, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)),
            this, SLOT(slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *)));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(slot_stateChanged(QAbstractSocket::SocketState)));
}

void light_controller_sslsock::slot_encrypted() {
    qDebug("light_controller_sslsock::slot_encrypted");
}

void light_controller_sslsock::slot_encryptedBytesWritten(qint64 written) {
    qDebug("light_controller_sslsock::slot_encryptedBytesWritten(%ld)", (long) written);
}

void light_controller_sslsock::slot_modeChanged(QSslSocket::SslMode mode) {
    qDebug("light_controller_sslsock::slot_modeChanged(%d)", mode);
}

void light_controller_sslsock::slot_peerVerifyError(const QSslError &) {
    qDebug("light_controller_sslsock::slot_peerVerifyError");
}

void light_controller_sslsock::slot_sslErrors(const QList<QSslError> &) {
    qDebug("light_controller_sslsock::slot_sslErrors");
}

void light_controller_sslsock::slot_readyRead() {
    QByteArray rem_command;

    qDebug("light_controller_sslsock::slot_readyRead");

    rem_command = socket->readLine(256);
    if (rem_command.size() < 2) {
        QApplication::quit();
    } else {
        current_command = QString(rem_command);
        emit command_received(current_command);
    } /* endif */
}

void light_controller_sslsock::slot_connected() {
    qDebug("light_controller_sslsock::slot_connected");
}

void light_controller_sslsock::slot_disconnected() {
    qDebug("light_controller_sslsock::slot_disconnected");
}

void light_controller_sslsock::slot_error(QAbstractSocket::SocketError err) {
    qDebug() << "light_controller_sslsock::slot_error(" << err << ")";
    qDebug() << socket->errorString();
}

void light_controller_sslsock::slot_hostFound() {
    qDebug("light_controller_sslsock::slot_hostFound");
}

void light_controller_sslsock::slot_proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *) {
    qDebug("light_controller_sslsock::slot_proxyAuthenticationRequired");
}

void light_controller_sslsock::slot_stateChanged(QAbstractSocket::SocketState state) {
    qDebug() << "light_controller_sslsock::slot_stateChanged(" << state << ")";
}
