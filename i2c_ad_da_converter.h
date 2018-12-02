#ifndef I2C_AD_DA_CONVERTER_H
#define I2C_AD_DA_CONVERTER_H
#include <unistd.h>
#include <stdio.h>
#include <QObject>
#include <QDebug>

class i2c_ad_da_converter : public QObject {
  Q_OBJECT
  public:
    i2c_ad_da_converter(unsigned char i2c_dev_addr = 0x48);
    ~i2c_ad_da_converter(void);
    unsigned char converter_read();
    void converter_write();
  public slots:
    void poll_ad_value();
  signals:
    void value_changed(int new_value);
  private:
    int ad_da_device_fd;
    unsigned char dev_address;
};

#endif // I2C_AD_DA_CONVERTER_H
