#include "light_controller_remote_ui.h"
#include "ui_light_controller_remote_ui.h"

light_controller_remote_ui::light_controller_remote_ui(QWidget *parent) : QMainWindow(parent),
                                    ui(new Ui::light_controller_remote_ui) {
    ui->setupUi(this);
    light_controller_connection.secure_connect();
    connect(&light_controller_connection, SIGNAL(socket_up()), this, SLOT(socket_up()));
    connect(&light_controller_connection, SIGNAL(socket_down()), this, SLOT(socket_down()));
    light_controller_service = new ZConfServiceBrowser(this);
    light_controller_service->browse("_light_controllerui._tcp");
    connect(light_controller_service, SIGNAL(serviceEntryAdded(QString)), this, SLOT(service_found(QString)));
    connect(ui->exit_button, &QPushButton::pressed, this, &light_controller_remote_ui::exit_button_pressed);
    connect(ui->dimmer_slider, &QSlider::valueChanged, this, &light_controller_remote_ui::light_controller_dimmer_set);
    connect(ui->toggle_on_off, &QPushButton::toggled,
            this, &light_controller_remote_ui::toggle_on_off_clicked);
}

light_controller_remote_ui::~light_controller_remote_ui() {
    delete ui;
    delete light_controller_service;
}

void light_controller_remote_ui::service_found(QString service_string) {
    ZConfServiceEntry this_service;
    qDebug() << "Found light_controller at " << service_string;
    this_service = light_controller_service->serviceEntry(service_string);
    qDebug() << "Service entry is " << this_service.ip << "protocol is " << this_service.protocolName() << "host" << this_service.host << "port is" << this_service.port;
    light_controller_connection.set_server_ip(this_service.ip);
    light_controller_connection.set_server_port(this_service.port);
}

void light_controller_remote_ui::socket_up() {
    ui->light_controller_controls->setEnabled(true);
}

void light_controller_remote_ui::socket_down() {
    ui->light_controller_controls->setEnabled(false);
}

void light_controller_remote_ui::toggle_on_off_clicked(bool ) {
    QString cmd("toggle_on_off");

    qDebug() << __func__ << __LINE__ << "Here ";
    light_controller_connection.send_data(cmd);
    qDebug() << __func__ << __LINE__ << "Here ";
}

void light_controller_remote_ui::exit_button_pressed() {
    qApp->quit();
}

void light_controller_remote_ui::light_controller_dimmer_set(int value) {
    QString cmd("light_controller" + QString(value));
    light_controller_connection.send_data(cmd);
}
