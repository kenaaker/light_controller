#ifndef I2C_AD_DA_CONVERTER_H
#define I2C_AD_DA_CONVERTER_H
#include <unistd.h>
#include <stdio.h>
#include <QObject>
#include <QDebug>

class i2c_ad_da_converter : public QObject {
  Q_OBJECT
  public:
    i2c_ad_da_converter(unsigned char i2c_dev_addr = 0x48, int ain_channel = 0, int aout_channel = 0);
    ~i2c_ad_da_converter(void);
    unsigned char converter_read();
    void converter_write();
    void set_ain_channel(int in_channel);
    void set_aout_channel(int in_channel);
    void set_lower_bound(int new_lower_bound);
    void set_upper_bound(int new_upper_bound);
    void set_instrument_lower_bound(int new_instrument_lower_bound);
    void set_instrument_upper_bound(int new_instrument_upper_bound);

public slots:
    void poll_ad_value();
  signals:
    void value_changed(int new_value);
  private:
    int instrument_lower_bound;
    int instrument_upper_bound;
    int lower_bound;
    int upper_bound;
    int ad_da_device_fd;
    int ad_da_device_in_channel;
    int ad_da_device_out_channel;
    int last_ranged_value;
    unsigned char dev_address;
};

#endif // I2C_AD_DA_CONVERTER_H
