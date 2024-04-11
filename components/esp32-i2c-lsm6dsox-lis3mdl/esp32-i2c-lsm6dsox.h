#ifndef __ESP32_I2C_LSM6DSOX_
#define __ESP32_I2C_LSM6DSOX_

#include "driver/i2c_master.h"


#define CTRL1_XL 0x10 // datasheet page 44
#define CTRL2_G 0x11 // ^
#define OUTX_L_G 0x22 // datasheet page 45
#define OUTX_H_G 0x23 // ^
#define OUTY_L_G 0x24 // ^
#define OUTY_H_G 0x25 // ^
#define OUTZ_L_G 0x26 // ^
#define OUTZ_H_G 0x27 // ^
#define OUTX_L_A 0x28 // ^
#define OUTX_H_A 0x29 // ^
#define OUTY_L_A 0x2A // ^
#define OUTY_H_A 0x2B // ^
#define OUTZ_L_A 0x2C // ^
#define OUTZ_H_A 0x2D // ^


void esp_i2c_lsm6dsox_begin(i2c_master_dev_handle_t *dev_handle);

int16_t esp_i2c_lsm6dsox_get_gyro_x(i2c_master_dev_handle_t *dev_handle);

int16_t esp_i2c_lsm6dsox_get_gyro_y(i2c_master_dev_handle_t *dev_handle);

int16_t esp_i2c_lsm6dsox_get_gyro_z(i2c_master_dev_handle_t *dev_handle);

int16_t esp_i2c_lsm6dsox_get_accel_x(i2c_master_dev_handle_t *dev_handle);

int16_t esp_i2c_lsm6dsox_get_accel_y(i2c_master_dev_handle_t *dev_handle);

int16_t esp_i2c_lsm6dsox_get_accel_z(i2c_master_dev_handle_t *dev_handle);

#endif
