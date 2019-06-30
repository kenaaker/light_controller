#ifndef LIGHT_CONTROLLER_REMOTE_UI_H
#define LIGHT_CONTROLLER_REMOTE_UI_H

#include <QMainWindow>
#include <QApplication>
#include <ssl_client.h>
#include "zconfservicebrowser.h"

namespace Ui {
class light_controller_remote_ui;
}

class light_controller_remote_ui : public QMainWindow {
    Q_OBJECT

public:
    explicit light_controller_remote_ui(QWidget *parent = 0);
    ~light_controller_remote_ui();
public slots:
    void socket_up();
    void socket_down();
    void service_found(QString);
    void exit_button_pressed();

private slots:
    void toggle_on_off_clicked(bool);
    void light_controller_dimmer_set(int value);

private:
    Ui::light_controller_remote_ui *ui;
    SslClient light_controller_connection;
    ZConfServiceBrowser *light_controller_service;
};

#endif // LIGHT_CONTROLLER_REMOTE_UI_H
