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

#ifndef SSLCLIENT_H
#define SSLCLIENT_H

#include <QtWidgets/QWidget>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslSocket>
#include <QTimer>

class SslClient : public QObject {
    Q_OBJECT
public:
    SslClient(QWidget *parent = 0);
    ~SslClient();
    void set_server_ip(QString &host_name_or_ip) {
        light_controller_host_name_or_ip = host_name_or_ip;
    }
    void set_server_port(unsigned short port) {
        light_controller_port = port;
    }

signals:
    void socket_up();
    void socket_down();

public slots:
    void secure_connect();
    void send_data(QString &cmd);

private slots:
    void socket_state_changed(QAbstractSocket::SocketState state);
    void socket_encrypted();
    void socket_ready_read();
    void socket_disconnected();
    void ssl_errors(const QList<QSslError> &errors);

private:
    QString light_controller_host_name_or_ip;
    unsigned short light_controller_port;
    QSslSocket *socket;
    QTimer reconnect_timer;
};

#endif
