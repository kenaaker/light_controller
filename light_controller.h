#ifndef LIGHT_CONTROLLER_H
#define LIGHT_CONTROLLER_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QSocketNotifier>
#include "lin_gpio.h"
#include "i2c_ad_da_converter.h"
#include "light_controller_sslsock.h"
#include "zconfservice.h"
#include "light_state_determiner.h"

namespace Ui {
class light_controller;
}

class light_controller : public QMainWindow {
    Q_OBJECT

  public:
    explicit light_controller(QWidget *parent = nullptr);
    ~light_controller();

private slots:
    void on_actionClose_triggered();
    void PIR_gpio_ready_read(int value);
    void light_intensity_changed(int value);
    void command_proc(QString &cmd);

private:
    Ui::light_controller *ui;
    light_state_determiner light_state_d;
    QTimer *light_sensor_poll;
    QTimer *power_supply_voltage_poll;
    QTimer *light_state_poll;
    lin_gpio *PIR_gpio;
    lin_gpio *power_control_gpio;
    QSocketNotifier *PIR_gpio_value_notifier;
    QFile PIR_gpio_value;                       /* PIR detector GPIO value file. */
    i2c_ad_da_converter light_intensity_converter;
    i2c_ad_da_converter power_supply_voltage_converter;
    light_controller_sslsock cmd_server;
    ZConfService *light_controller_svc;
    void set_light_state(void);
};

#endif // LIGHT_CONTROLLER_H
