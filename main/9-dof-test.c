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
		.flags.enable_internal_pullup = false
	};

	i2c_master_bus_handle_t bus_handle;
	ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

	/* 2. Configure the LIS3MDL (magnetometer) */
	i2c_device_config_t magnetometer_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = 0x1E,
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

    /* Sometimes I find I have to dance with the magnetometer. If
     * calling the 'esp_i2c_lis3mdl_begin()' function is causing your
     * program to block, uncomment this line, run the program with the
     * address 0x1E. If the program crashes on repeat, then try this call
     * with device address 0x1C. If that doesn't crash, change the 'i2c_device_config_t'
     * struct for the magnetometer to have a device address of 0x1C. If that
     * crashes, change both this probe call and the struct to use device
     * address 0x1E. Hopefully then everything should work fine. */
    /* ESP_ERROR_CHECK(i2c_master_probe(bus_handle, 0x1E, -1)); */

    /* 4a. Turn on and set operation control for accelerometer and gyro */
    struct i2c_lsm6dsox i2c_lsm6dsox;
    i2c_lsm6dsox.i2c_handle = &accelgyro_handle;
    esp_i2c_lsm6dsox_begin(&i2c_lsm6dsox);
    /* 4b. Turn on and set operation control for magnetometer */
    struct i2c_lis3mdl i2c_lis3mdl;
    i2c_lis3mdl.i2c_handle = &magnetometer_handle;
    esp_i2c_lis3mdl_begin(&i2c_lis3mdl);

	while (1) {
		vTaskDelay(500 / portTICK_PERIOD_MS);

        float outxyz_g[3];
        esp_i2c_lsm6dsox_get_gyro_data(&i2c_lsm6dsox, &outxyz_g[0]);
        /* Convert from mdps (millidegrees per second) to dps (degrees per second) */
        outxyz_g[0] /= 1000;
        outxyz_g[1] /= 1000;
        outxyz_g[2] /= 1000;

        float outxyz_a[3];
        esp_i2c_lsm6dsox_get_accel_data(&i2c_lsm6dsox, &outxyz_a[0]);
        /* Convert from mg (milligravity, not milligrams) to g (gravity) */
        outxyz_a[0] /= 1000;
        outxyz_a[1] /= 1000;
        outxyz_a[2] /= 1000;

        float outxyz_m[3];
        esp_i2c_lis3mdl_get_data(&i2c_lis3mdl, &outxyz_m[0]);

        printf("gyroscope: (% #7.2fdps % #7.2fdps % #7.2fdps)    " \
            "accelerometer: (% #7.3fg % #7.3fg % #7.3fg)    " \
            "magnetometer: (% #7.4fG % #7.4fG % #7.4fG)\n", \
            outxyz_g[0], outxyz_g[1], outxyz_g[2], \
            outxyz_a[0], outxyz_a[1], outxyz_a[2], \
            outxyz_m[0], outxyz_m[1], outxyz_m[2]);
	}
}
