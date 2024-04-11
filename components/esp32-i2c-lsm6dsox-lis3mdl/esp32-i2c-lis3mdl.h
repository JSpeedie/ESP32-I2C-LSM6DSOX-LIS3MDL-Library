#ifndef __ESP32_I2C_LIS3MDL_
#define __ESP32_I2C_LIS3MDL_

#include "driver/i2c_master.h"


#define CTRL_REG1 0x20 // datasheet page 18
#define CTRL_REG2 0x21 // ^
#define OUTX_L 0x28 // ^
#define OUTX_H 0x29 // ^
#define OUTY_L 0x2A // ^
#define OUTY_H 0x2B // ^
#define OUTZ_L 0x2C // ^
#define OUTZ_H 0x2D // ^


int16_t esp_i2c_lis3mdl_get_x(i2c_master_dev_handle_t *dev_handle);

int16_t esp_i2c_lis3mdl_get_y(i2c_master_dev_handle_t *dev_handle);

int16_t esp_i2c_lis3mdl_get_z(i2c_master_dev_handle_t *dev_handle);


#endif
