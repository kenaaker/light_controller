#include "light_controller.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    light_controller w;
    qDebug() << "Setting up UI.";
    w.show();

    qDebug() << "Running app.";
    return a.exec();
    qDebug() << "Exit.";
}
