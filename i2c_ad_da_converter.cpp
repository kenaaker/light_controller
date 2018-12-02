#include "i2c_ad_da_converter.h"
#include <QDebug>
#include <QCoreApplication>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

i2c_ad_da_converter::i2c_ad_da_converter(unsigned char i2c_dev_addr) {

    ad_da_device_fd = open("/dev/i2c-1", O_RDWR);
    if (ad_da_device_fd < 0) {
        qDebug() << "Couldn't open i2c device bus, quitting.";
        QCoreApplication::exit(-1);
    } else {
        int rc;
        dev_address = i2c_dev_addr;
        /* Set the i2c slave address */
        rc = ioctl(ad_da_device_fd, I2C_SLAVE, dev_address);
        if (rc < 0) {
            qDebug() << "Couldn't set i2c slave device address, quitting.";
            QCoreApplication::exit(-1);
        }
    }
}

i2c_ad_da_converter::~i2c_ad_da_converter(void) {
    close(ad_da_device_fd);
}

/* This method will return one 8 bit sample from the ad_da converter on the i2c bus. */
unsigned char i2c_ad_da_converter::converter_read() {
    int read_rc;
    char i2c_buffer[2];

    read_rc = read(ad_da_device_fd, i2c_buffer, sizeof(i2c_buffer));
    if (read_rc != sizeof(i2c_buffer)) {
        qDebug() << "Couldn't read from i2c slave device, quitting.";
        QCoreApplication::exit(-1);
    }
    qDebug("convert_read got these bytes addr=0x%0x, data=0x%0x", i2c_buffer[0] >> 1, i2c_buffer[1]);
    emit value_changed(i2c_buffer[1]);
    return i2c_buffer[1];
}

void i2c_ad_da_converter::poll_ad_value() {
    qDebug() << " Polling timer for light sensor.";
    converter_read();
}

