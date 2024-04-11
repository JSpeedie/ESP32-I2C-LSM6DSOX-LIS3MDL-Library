#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "driver/i2c_master.h"

/* Component includes */
#include "esp32-i2c-lsm6dsox.h"
#include "esp32-i2c-lis3mdl.h"


#define I2C_BUS_PORT 0
/* Going off  https://learn.adafruit.com/assets/111179 */
#define I2C_SDA_PIN_NUM 23
#define I2C_SCL_PIN_NUM 22


void app_main(void)
{
	/* 1. Configure the i2c master bus */
	i2c_master_bus_config_t i2c_mst_config = {
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.i2c_port = I2C_BUS_PORT,
		.scl_io_num = I2C_SCL_PIN_NUM,
		.sda_io_num = I2C_SDA_PIN_NUM,
		.glitch_ignore_cnt = 7,
		.flags.enable_internal_pullup = false,
	};
		/* .flags.enable_internal_pullup = true */

	i2c_master_bus_handle_t bus_handle;
	ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));


	/* 2. Configure the LIS3MDL (magnetometer) */
	i2c_device_config_t magnetometer_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = 0x1C,
		.scl_speed_hz = 100000,
	};

	i2c_master_dev_handle_t magnetometer_handle;
	ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &magnetometer_cfg, \
        &magnetometer_handle));


	/* 3. Configure the LSM6DSOX (accelerometer + gyroscope) */
	i2c_device_config_t accelgyro_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = 0x6A,
		.scl_speed_hz = 100000,
	};

	i2c_master_dev_handle_t accelgyro_handle;
	ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &accelgyro_cfg, \
        &accelgyro_handle));

    /* 4. Turn on and set operation control for accelerometer and gyro */
    esp_i2c_lsm6dsox_begin(&accelgyro_handle);

	vTaskDelay(1000 / portTICK_PERIOD_MS);

	while (1) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);

        int16_t outx_g = esp_i2c_lsm6dsox_get_gyro_x(&accelgyro_handle);
        int16_t outy_g = esp_i2c_lsm6dsox_get_gyro_y(&accelgyro_handle);
        int16_t outz_g = esp_i2c_lsm6dsox_get_gyro_z(&accelgyro_handle);

        int16_t outx_a = esp_i2c_lsm6dsox_get_accel_x(&accelgyro_handle);
        int16_t outy_a = esp_i2c_lsm6dsox_get_accel_y(&accelgyro_handle);
        int16_t outz_a = esp_i2c_lsm6dsox_get_accel_z(&accelgyro_handle);

        int16_t outx_m = esp_i2c_lis3mdl_get_x(&magnetometer_handle);
        int16_t outy_m = esp_i2c_lis3mdl_get_y(&magnetometer_handle);
        int16_t outz_m = esp_i2c_lis3mdl_get_z(&magnetometer_handle);

        printf("gyroscope: (%5d %5d %5d)    " \
            "accelerometer: (%5d %5d %5d)    " \
            "magnetometer: (%5d %5d %5d)\n", \
            outx_g, outy_g, outz_g, \
            outx_a, outy_a, outz_a, \
            outx_m, outy_m, outz_m);
	}
}
