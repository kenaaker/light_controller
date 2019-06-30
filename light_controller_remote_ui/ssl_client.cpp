/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ssl_client.h"

#include <QSslSocket>
#include <QTimer>

SslClient::SslClient(QWidget *parent)
    : QObject(parent), socket(0) {
}

SslClient::~SslClient() {
}

void SslClient::secure_connect() {
    if (!socket) {
        socket = new QSslSocket(this);
    } /* endif */
    qDebug() << __func__ << " Entered socket state is " << socket->state();
    if ((socket->state() == QAbstractSocket::ConnectedState) ||
            (socket->state() == QAbstractSocket::ConnectingState)) {
        reconnect_timer.stop();
    } else {
        qDebug() << __func__ << __LINE__ << "Here ";
        if (light_controller_host_name_or_ip.isEmpty()) {
            connect(&reconnect_timer, SIGNAL(timeout()), this, SLOT(secure_connect()));
            reconnect_timer.start(2000);    /* Try to reconnect every two seconds */
            qDebug("%s:%d Attempting to connect to light controller in 2 seconds. No hostname or IP available.",
                   __func__, __LINE__);
        } else {
            qDebug("%s:%d Attempting to connect to light controller at %s port %d.",
                   __func__, __LINE__, light_controller_host_name_or_ip.toStdString().c_str(), light_controller_port);
            /* Add self-signed client and server certificates and CA */
            socket->addCaCertificates(":/light_controller_ca.pem");
            socket->setLocalCertificate(":/light_controller_client.pem");
            socket->setPrivateKey(":/light_controller_client.key", QSsl::Rsa, QSsl::Pem, "var6look");
            qDebug() << __func__ << __LINE__ << "Here ";

            connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                    this, SLOT(socket_state_changed(QAbstractSocket::SocketState)));
            connect(socket, SIGNAL(encrypted()), this, SLOT(socket_encrypted()));
            connect(socket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(ssl_errors(const QList<QSslError>)));
            connect(socket, SIGNAL(readyRead()), this, SLOT(socket_ready_read()));
            connect(socket, SIGNAL(disconnected()), this, SLOT(socket_disconnected()));
            connect(&reconnect_timer, SIGNAL(timeout()), this, SLOT(secure_connect()));
            qDebug() << __func__ << __LINE__ << "Here ";

            QList<QSslCertificate> light_controller_cert = QSslCertificate::fromPath(":/light_controller_server.pem");
            QSslError self_signed_error(QSslError::SelfSignedCertificate, light_controller_cert.at(0));
            QSslError host_name_mismatch(QSslError::HostNameMismatch, light_controller_cert.at(0));
            QList<QSslError> expected_ssl_errors;
            expected_ssl_errors.append(self_signed_error);
            expected_ssl_errors.append(host_name_mismatch);
            socket->ignoreSslErrors(expected_ssl_errors);

            qDebug("%s:%d light_controller_host_name_or_ip=\"%s\" light_controller_port=%d",
                   __func__, __LINE__, qPrintable(light_controller_host_name_or_ip), light_controller_port);
            qDebug() << __func__ << __LINE__ << "Here ";
            socket->connectToHostEncrypted(light_controller_host_name_or_ip, light_controller_port,
                                           QIODevice::ReadWrite,
                                           QAbstractSocket::IPv6Protocol);
            qDebug() << __func__ << __LINE__ << "Here ";
        } /* endif */
    } /* endif */
}

void SslClient::socket_state_changed(QAbstractSocket::SocketState state) {
    qDebug() << __func__ << " Entered, socket state is " << socket->state();
    if (state == QAbstractSocket::UnconnectedState) {
        emit socket_down();
        socket->deleteLater();
        socket = nullptr;
        reconnect_timer.start(2000);    /* Try to reconnect every two seconds */
    } else if (state == QAbstractSocket::ConnectedState) {
        emit socket_up();
        reconnect_timer.stop();
    } else {
    } /* endif */
}

void SslClient::socket_encrypted() {
    qDebug() << __func__ << "Entered";
    if (!socket) {
        return;                 // might have disconnected already
    } else {
        qDebug() << "Socket Encrypted";
    } /* endif */
}

void SslClient::socket_ready_read() {
    qDebug() << __func__ << __LINE__ << "Here ";
    remote_line = socket->readLine(remote_line.capacity());
    qDebug("read \"%s\" %d bytes", remote_line.toStdString().c_str(), remote_line.size());
    qDebug() << __func__ << __LINE__ << "Here ";
}

QByteArray SslClient::get_remote_data(void) {
    return remote_line;
}

/* This will try to reconnect by using a timer */
void SslClient::socket_disconnected() {
    reconnect_timer.start(2000);    /* Try to reconnect every two seconds */
}

void SslClient::send_data(QString &cmd) {
    QByteArray local_cmd;
    local_cmd.append(cmd);

    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(local_cmd, local_cmd.size());
        qDebug("wrote \"%s\" %d bytes", local_cmd.toStdString().c_str(), local_cmd.size());
        socket->flush();
    }
}

void SslClient::ssl_errors(const QList<QSslError> &errors) {
    qDebug() << __func__ << " Entered, socket state is " << socket->state();
    foreach (const QSslError &error, errors) {
        qDebug() << error.errorString();
    } /* endfor */

    socket->ignoreSslErrors();

    // did the socket state change?
    if (socket->state() != QAbstractSocket::ConnectedState) {
        socket_state_changed(socket->state());
    } /* endif */
}
