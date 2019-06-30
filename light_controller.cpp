#include "light_controller.h"
#include "ui_light_controller.h"
#include "light_state_determiner.h"
#include <QDebug>
#include <QTime>
#include <QThread>
#include <lin_gpio.h>

light_controller::light_controller(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::light_controller) {
    QString PIR_gpio_value_path;

    ui->setupUi(this);

    PIR_gpio = new lin_gpio("GPIO_17");
    PIR_gpio->gpio_set_direction("in");
    PIR_gpio->gpio_set_edge("both");
    PIR_gpio_value_path = QString::fromStdString(PIR_gpio->gpio_get_path());
    PIR_gpio_value.setFileName(PIR_gpio_value_path);
    PIR_gpio_value.open(QFile::ReadOnly);
    PIR_gpio_value_notifier = new QSocketNotifier(PIR_gpio_value.handle(),
                                                  QSocketNotifier::Exception);
    PIR_gpio_value_notifier->setEnabled(true);
    connect(PIR_gpio_value_notifier, SIGNAL(activated(int)),
            this, SLOT(PIR_gpio_ready_read(int)));

    power_control_gpio = new lin_gpio("GPIO_04");
    power_control_gpio->gpio_set_direction("out");
    power_control_gpio->gpio_set_value("0");

    light_intensity_converter.set_ain_channel(0);
    light_intensity_converter.set_instrument_lower_bound(255);
    light_intensity_converter.set_instrument_upper_bound(0);
    light_intensity_converter.set_lower_bound(ui->light_intensity->minimum());
    light_intensity_converter.set_upper_bound(ui->light_intensity->maximum());

    power_supply_voltage_converter.set_ain_channel(1);
    power_supply_voltage_converter.set_instrument_lower_bound(0);
    power_supply_voltage_converter.set_instrument_upper_bound(200);
    power_supply_voltage_converter.set_lower_bound(0);
    power_supply_voltage_converter.set_upper_bound(12);

    qDebug() << "Read from light intensity converter = " << light_intensity_converter.converter_read();
    QObject::connect(&light_intensity_converter, &i2c_ad_da_converter::value_changed,
                     ui->light_intensity, &QDial::setValue);
    QObject::connect(&light_intensity_converter, &i2c_ad_da_converter::value_changed,
                     this, &light_controller::light_intensity_changed);

    qDebug() << " Setting up QTimer for polling light sensor.";
    light_sensor_poll = new QTimer(this);
    QObject::connect(light_sensor_poll, &QTimer::timeout,
                     &light_intensity_converter, &i2c_ad_da_converter::poll_ad_value);

    light_sensor_poll->start(100);

    QObject::connect(&power_supply_voltage_converter, &i2c_ad_da_converter::value_changed,
                     ui->ps_voltage, &QDial::setValue);

    qDebug() << " Setting up QTimer for polling power supply voltage.";
    power_supply_voltage_poll = new QTimer(this);
    QObject::connect(power_supply_voltage_poll, &QTimer::timeout,
                     &power_supply_voltage_converter, &i2c_ad_da_converter::poll_ad_value);

    power_supply_voltage_poll->start(1000);

    qDebug() << " Setting up QTimer for overall light state poll.";
    light_state_poll = new QTimer(this);
    QObject::connect(light_state_poll, &QTimer::timeout,
                     this, &light_controller::set_light_state);

    light_state_poll->start(1000);

    qDebug() << __func__ << " Entered, starting listen on 48049";
    if (!cmd_server.listen(QHostAddress::Any, 48049)) {
       QApplication::quit();
    } /* endif */
    qDebug() << __func__ << " Listen on 48049 is in place.";
    QObject::connect(&cmd_server, SIGNAL(command_received(QString &)), this, SLOT(command_proc(QString &)));

    light_controller_svc = new ZConfService(this);

    light_controller_svc->registerService("light_controller", 48049, "_light_controllerui._tcp");

}

light_controller::~light_controller() {
    delete ui;
}

void light_controller::on_actionClose_triggered() {
    power_control_gpio->gpio_set_value("0");
    QApplication::exit(0);
}

void light_controller::light_intensity_changed(int new_intensity) {
    //qDebug() << __func__ << ":" << __LINE__ << " light intensity changed to " << new_intensity;
    light_state_d.set_light_intensity(new_intensity);
}

void light_controller::PIR_gpio_ready_read(int) {
    QByteArray PIR_line;          /* Value read from PIR sensor GPIO value */

    PIR_gpio_value.seek(0);
    PIR_line = PIR_gpio_value.readAll();
    if (PIR_line.size() < 1) {
        qDebug() << __func__ << ":" << __LINE__ << " gpio PIR detector read failed.." ;
    } else {
        if (PIR_line[0] == '1') {               /* PIR has fired. */
            // qDebug() << __func__ << ":" << __LINE__ << " PIR detector has fired." ;
            QThread::msleep(250);
            light_state_d.occupancy_sensor_fired();
            ui->detected_somebody_label->setEnabled(true);
        } else if (PIR_line[0] == '0') {               /* PIR has  gone silent.. */
            //qDebug() << __func__ << ":" << __LINE__ << " PIR detector has gone silent." ;
            QThread::msleep(250);
            ui->detected_somebody_label->setEnabled(false);
        } else {
            qDebug() << __func__ << ":" << __LINE__ << " gpio PIR detector read trash.." ;
        }
    } /* endif */
} /* PIR_gpio_ready_read */

void light_controller::command_proc(QString &cmd) {
    qDebug() << "Got new command = " << cmd;
    if (cmd == "toggle_on_off") {
        light_states current_light_state = light_state_d.user_set_light_state();

        qDebug() << "old light state = " << current_light_state;
        if (current_light_state == off) {
            qDebug() << "old light state indicates off, turning on.";
            light_state_d.user_set_light_on();
        } else if (current_light_state == on) {
            qDebug() << "old light state indicates on, turning off.";
            light_state_d.user_set_light_off();
        } else  if (current_light_state == undefined) {
            qDebug() << "old light state never set, turning on.";
            light_state_d.user_set_light_on();
        } else {
            qDebug() << "Don't understand this new command = " << cmd;
        }
    } else if (cmd == "dimmer_setting") {
        qDebug() << "dimmer_setting command = " << cmd;
    } else {
        qDebug() << "Didn't understand cmd = " << cmd;
    }
} /* command proc */

void light_controller::set_light_state(void) {
    light_states this_light_state;

    this_light_state = light_state_d.light_should_be();
    if (this_light_state == on) {
        power_control_gpio->gpio_set_value("1");
    } else if (this_light_state == off) {
        power_control_gpio->gpio_set_value("0");
    } else {
        qDebug() << "Confused state for light_state_d light_state is  " << this_light_state;
    }
}
