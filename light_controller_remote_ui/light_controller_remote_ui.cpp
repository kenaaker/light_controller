#include "light_controller_remote_ui_class.h"
#include "ui_light_controller_remote_ui.h"

light_controller_remote_ui_class::light_controller_remote_ui_class(QWidget *parent) : QMainWindow(parent),
                                    ui(new Ui::light_controller_remote_ui) {
    ui->setupUi(this);
    light_contoller_connection.secureConnect();
    connect(&light_controller_connection, SIGNAL(socket_up()), this, SLOT(socket_up()));
    connect(&light_controller_connection, SIGNAL(socket_down()), this, SLOT(socket_down()));
    light_controller_svc = new ZConfServiceBrowser(this);
    light_controller_svc->browse("_light_controllerui._tcp");
    connect(light_controller_svc, SIGNAL(serviceEntryAdded(QString)), this, SLOT(service_found(QString)));
}

light_controller_remote_ui_class::~light_controller_remote_ui_class() {
    delete ui;
    delete light_controller_svc;
}

void light_controller_remote_ui_class::service_found(QString service_string) {
    ZConfServiceEntry this_service;
    qDebug() << "Found light_controller at " << service_string;
    this_service = light_controller_svc->serviceEntry(service_string);
    qDebug() << "Service entry is " << this_service.ip << "protocol is " << this_service.protocolName() << "host" << this_service.host << "port is" << this_service.port;
    light_controller_connection.set_server_ip(this_service.ip);
    light_controller_connection.set_server_port(this_service.port);
}

void light_controller_remote_ui_class::socket_up() {
    ui->light_controller_controls->setEnabled(true);
}

void light_controller_remote_ui_class::socket_down() {
    ui->light_controller_controls->setEnabled(false);
}

void light_controller_remote_ui_class::on_pushButton_clicked() {
    qApp->quit();
}

void light_controller_remote_ui_class::on_counter_clockwise_1_clicked() {
    QString cmd("counter_clockwise_1");
    light_controller_connection.sendData(cmd);
}

void light_controller_remote_ui_class::on_home_button_clicked() {
    QString cmd("home");
    light_controller_connection.sendData(cmd);
}

void light_controller_remote_ui_class::on_clockwise_1_clicked() {
    QString cmd("clockwise_1");
    light_controller_connection.sendData(cmd);
}

void light_controller_remote_ui_class::on_light_controller_position_clicked() {
    QString cmd("light_controller");
    light_controller_connection.sendData(cmd);
}
