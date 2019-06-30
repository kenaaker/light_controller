#ifndef LIGHT_CONTROLLER_REMOTE_UI_H
#define LIGHT_CONTROLLER_REMOTE_UI_H

#include <QMainWindow>
#include <QApplication>
#include <ssl_client.h>
#include "zconfservicebrowser.h"

namespace Ui {
class light_controller_remote_ui;
}

class light_controller_remote_ui_class : public QMainWindow
{
    Q_OBJECT

public:
    explicit light_controller_remote_ui_class(QWidget *parent = 0);
    ~light_controller_remote_ui_class();
public slots:
    void socket_up();
    void socket_down();
    void service_found(QString);

private slots:
    void on_pushButton_clicked();

    void on_counter_clockwise_1_clicked();

    void on_home_button_clicked();

    void on_clockwise_1_clicked();

    void on_light_controller_position_clicked();

private:
    Ui::light_controller_remote_ui *ui;
    SslClient light_controller_connection;
    ZConfServiceBrowser *light_controller_svc;
};

#endif // LIGHT_CONTROLLER_REMOTE_UI_H
