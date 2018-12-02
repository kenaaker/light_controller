#include "light_controller.h"
#include "ui_light_controller.h"
#include <QDebug>

light_controller::light_controller(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::light_controller) {

    ui->setupUi(this);

    qDebug() << "Read from first converter = " << first_converter.converter_read();
    QObject::connect(&first_converter, &i2c_ad_da_converter::value_changed,
                     ui->dial, &QDial::setValue);

    qDebug() << " Setting up QTimer for polling light sensor.";
    light_sensor_poll = new QTimer(this);
    QObject::connect(light_sensor_poll, &QTimer::timeout,
                     &first_converter, &i2c_ad_da_converter::poll_ad_value);

    light_sensor_poll->start(1000);
}

light_controller::~light_controller() {
    delete ui;
}

void light_controller::on_actionClose_triggered() {
    QApplication::exit(0);
}
