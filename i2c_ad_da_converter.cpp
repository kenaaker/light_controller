#include "i2c_ad_da_converter.h"
#include <QDebug>
#include <QCoreApplication>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

i2c_ad_da_converter::i2c_ad_da_converter(unsigned char i2c_dev_addr, int ain_channel, int aout_channel) {

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
        ad_da_device_in_channel = ain_channel;
        ad_da_device_out_channel = aout_channel;
        lower_bound = 0;
        upper_bound = 100;
    }
}

i2c_ad_da_converter::~i2c_ad_da_converter(void) {
    close(ad_da_device_fd);
}

void i2c_ad_da_converter::set_ain_channel(int ain_channel) {
    ad_da_device_in_channel = ain_channel;
}

void i2c_ad_da_converter::set_aout_channel(int aout_channel) {
    ad_da_device_out_channel = aout_channel;
}

void i2c_ad_da_converter::set_lower_bound(int new_lower_bound) {
    lower_bound = new_lower_bound;
}

void i2c_ad_da_converter::set_upper_bound(int new_upper_bound) {
    upper_bound = new_upper_bound;
}

void i2c_ad_da_converter::set_instrument_lower_bound(int new_instrument_lower_bound) {
    instrument_lower_bound = new_instrument_lower_bound;
}

void i2c_ad_da_converter::set_instrument_upper_bound(int new_instrument_upper_bound) {
    instrument_upper_bound = new_instrument_upper_bound;
}

/* This method will return one 8 bit sample from the ad_da converter on the i2c bus. */
unsigned char i2c_ad_da_converter::converter_read() {
    int rc;
    char i2c_buffer;
    char i2c_cmd_buffer[2];
    int range = (upper_bound - lower_bound);
    int instrument_range = (instrument_upper_bound - instrument_lower_bound);
    int ranged_value;

    /* Command the i2c device (PCF8591) first to read from the specific AIN line */
    i2c_cmd_buffer[0] = ((ad_da_device_in_channel) & 0x03); /* Output enable 0x40 read input in_channel */
    i2c_cmd_buffer[1] = 0;

    rc = write(ad_da_device_fd, i2c_cmd_buffer, sizeof(i2c_cmd_buffer));

    /* Discard the first read */
    rc = read(ad_da_device_fd, &i2c_buffer, sizeof(i2c_buffer));
    if (rc != sizeof(i2c_buffer)) {
        qDebug() << "Couldn't read from i2c slave device, quitting.";
        QCoreApplication::exit(-1);
    }
    rc = read(ad_da_device_fd, &i2c_buffer, sizeof(i2c_buffer));
    if (rc != sizeof(i2c_buffer)) {
        qDebug() << "Couldn't read from i2c slave device, quitting.";
        QCoreApplication::exit(-1);
    }

    if (instrument_range < 0) {
        ranged_value = range + ((i2c_buffer * range) / instrument_range);
    } else {
        ranged_value = (i2c_buffer * range) / instrument_range;
    }

#if 0
    if (ad_da_device_in_channel == 0) {
    qDebug("convert_read ain device %d instrument bounds = [%d,%d] range = %d, converter bounds = [%d,%d] range = %d",
           ad_da_device_in_channel, instrument_lower_bound, instrument_upper_bound,
           (instrument_upper_bound - instrument_lower_bound),
           lower_bound, upper_bound, (upper_bound - lower_bound));
    qDebug("convert_read ain device %d got this bytes AIN = %d, data=0x%0x=%d, ranged value = %d",
           ad_da_device_in_channel, ad_da_device_in_channel, i2c_buffer, i2c_buffer, ranged_value);
    }
#endif

    if (ranged_value != last_ranged_value) {
        emit value_changed(ranged_value);
        last_ranged_value = ranged_value;
    }
    return i2c_buffer;
}

void i2c_ad_da_converter::poll_ad_value() {
    converter_read();
}

