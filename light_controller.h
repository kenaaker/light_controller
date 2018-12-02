#ifndef LIGHT_CONTROLLER_H
#define LIGHT_CONTROLLER_H

#include <QMainWindow>
#include <QTimer>
#include "i2c_ad_da_converter.h"

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

private:
    Ui::light_controller *ui;
    QTimer *light_sensor_poll;
    i2c_ad_da_converter first_converter;
};

#endif // LIGHT_CONTROLLER_H
