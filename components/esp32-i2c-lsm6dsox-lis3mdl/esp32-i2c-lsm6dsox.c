#include <inttypes.h>

#include "driver/i2c_master.h"

#include "esp32-i2c-lsm6dsox.h"


void esp_i2c_lsm6dsox_begin(i2c_master_dev_handle_t *dev_handle) {
    /* {{{ */
    /* 1. Set accelerometer to ON and in high-performance mode */
    /* 1a. Read the current value for the CTRL1_XL register */
    uint8_t buf = CTRL1_XL;
    uint8_t ctrl1_xl_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &ctrl1_xl_content, 1, -1));
    /* 1b. Set the ODR bits of the CTRL1_XL register to high performance mode
     * (datasheet page 56) */
    ctrl1_xl_content &= 0x0f; // Zero the ODR bits of the register
    /* ctrl1_xl_content |= 0xa0; // Set the ODR bits to 1010 */
    ctrl1_xl_content |= 0x10; // Set the ODR bits to 0001
    ctrl1_xl_content |= 0x02; // Set the LPF2 enable bit to 1 (0010)
    /* 1c. Write the new value back to the CTRL1_XL register */
    uint8_t sub_and_data[2];
    sub_and_data[0] = buf;
    sub_and_data[1] = ctrl1_xl_content;
    ESP_ERROR_CHECK(i2c_master_transmit(*dev_handle, &sub_and_data[0], \
        sizeof(sub_and_data), -1));

    /* 2. Set gyroscope to ON and in high-performance mode */
    /* 2a. Read the current value for the CTRL2_G register */
    buf = CTRL2_G;
    uint8_t ctrl2_g_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &ctrl2_g_content, 1, -1));
    /* 2b. Set the ODR bits of the CTRL2_G register to high performance mode
     * (datasheet page 56) */
    ctrl2_g_content &= 0x0f; // Zero the ODR bits of the register
    /* ctrl2_g_content |= 0xa0; // Set the ODR bits to 1010 */
    ctrl2_g_content |= 0x10; // Set the ODR bits to 0001
    /* 2c. Write the new value back to the CTRL2_G register */
    sub_and_data[0] = buf;
    sub_and_data[1] = ctrl2_g_content;
    ESP_ERROR_CHECK(i2c_master_transmit(*dev_handle, &sub_and_data[0], \
        sizeof(sub_and_data), -1));
    /* }}} */
}


int16_t esp_i2c_lsm6dsox_get_gyro_x(i2c_master_dev_handle_t *dev_handle) {
    /* Get the higher 8 bits from their respective register */
    uint8_t buf = OUTX_H_G;
    uint8_t outx_h_g;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outx_h_g, 1, -1));

    /* Get the lower 8 bits from their respective register */
    buf = OUTX_L_G;
    uint8_t outx_l_g;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outx_l_g, 1, -1));

    /* Combine the 2 sets of 8 bits into one 16 bit, signed int */
    int16_t outx_g = 0;
    outx_g |= (outx_h_g << 8);
    outx_g |= outx_l_g;

    return outx_g;
}


int16_t esp_i2c_lsm6dsox_get_gyro_y(i2c_master_dev_handle_t *dev_handle) {
    /* Get the higher 8 bits from their respective register */
    uint8_t buf = OUTY_H_G;
    uint8_t outy_h_g;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outy_h_g, 1, -1));

    /* Get the lower 8 bits from their respective register */
    buf = OUTY_L_G;
    uint8_t outy_l_g;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outy_l_g, 1, -1));

    /* Combine the 2 sets of 8 bits into one 16 bit, signed int */
    int16_t outy_g = 0;
    outy_g |= (outy_h_g << 8);
    outy_g |= outy_l_g;

    return outy_g;
}


int16_t esp_i2c_lsm6dsox_get_gyro_z(i2c_master_dev_handle_t *dev_handle) {
    /* Get the higher 8 bits from their respective register */
    uint8_t buf = OUTZ_H_G;
    uint8_t outz_h_g;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outz_h_g, 1, -1));

    /* Get the lower 8 bits from their respective register */
    buf = OUTZ_L_G;
    uint8_t outz_l_g;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outz_l_g, 1, -1));

    /* Combine the 2 sets of 8 bits into one 16 bit, signed int */
    int16_t outz_g = 0;
    outz_g |= (outz_h_g << 8);
    outz_g |= outz_l_g;

    return outz_g;
}


int16_t esp_i2c_lsm6dsox_get_accel_x(i2c_master_dev_handle_t *dev_handle) {
    /* Get the higher 8 bits from their respective register */
    uint8_t buf = OUTX_H_A;
    uint8_t outx_h_a;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outx_h_a, 1, -1));

    /* Get the lower 8 bits from their respective register */
    buf = OUTX_L_A;
    uint8_t outx_l_a;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outx_l_a, 1, -1));

    /* Combine the 2 sets of 8 bits into one 16 bit, signed int */
    int16_t outx_a = 0;
    outx_a |= (outx_h_a << 8);
    outx_a |= outx_l_a;

    return outx_a;
}


int16_t esp_i2c_lsm6dsox_get_accel_y(i2c_master_dev_handle_t *dev_handle) {
    /* Get the higher 8 bits from their respective register */
    uint8_t buf = OUTY_H_A;
    uint8_t outy_h_a;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outy_h_a, 1, -1));

    /* Get the lower 8 bits from their respective register */
    buf = OUTY_L_A;
    uint8_t outy_l_a;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outy_l_a, 1, -1));

    /* Combine the 2 sets of 8 bits into one 16 bit, signed int */
    int16_t outy_a = 0;
    outy_a |= (outy_h_a << 8);
    outy_a |= outy_l_a;

    return outy_a;
}


int16_t esp_i2c_lsm6dsox_get_accel_z(i2c_master_dev_handle_t *dev_handle) {
    /* Get the higher 8 bits from their respective register */
    uint8_t buf = OUTZ_H_A;
    uint8_t outz_h_a;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outz_h_a, 1, -1));

    /* Get the lower 8 bits from their respective register */
    buf = OUTZ_L_A;
    uint8_t outz_l_a;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outz_l_a, 1, -1));

    /* Combine the 2 sets of 8 bits into one 16 bit, signed int */
    int16_t outz_a = 0;
    outz_a |= (outz_h_a << 8);
    outz_a |= outz_l_a;

    return outz_a;
}
