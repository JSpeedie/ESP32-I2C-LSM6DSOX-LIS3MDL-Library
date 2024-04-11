#include <inttypes.h>

#include "driver/i2c_master.h"

#include "esp32-i2c-lis3mdl.h"


int16_t esp_i2c_lis3mdl_get_x(i2c_master_dev_handle_t *dev_handle) {
    /* Get the higher 8 bits from their respective register */
    uint8_t buf = OUTX_H;
    uint8_t outx_h;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outx_h, 1, -1));

    /* Get the lower 8 bits from their respective register */
    buf = OUTX_L;
    uint8_t outx_l;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outx_l, 1, -1));

    /* Combine the 2 sets of 8 bits into one 16 bit, signed int */
    int16_t outx = 0;
    outx |= (outx_h << 8);
    outx |= outx_l;

    return outx;
}


int16_t esp_i2c_lis3mdl_get_y(i2c_master_dev_handle_t *dev_handle) {
    /* Get the higher 8 bits from their respective register */
    uint8_t buf = OUTY_H;
    uint8_t outy_h;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outy_h, 1, -1));

    /* Get the lower 8 bits from their respective register */
    buf = OUTY_L;
    uint8_t outy_l;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outy_l, 1, -1));

    /* Combine the 2 sets of 8 bits into one 16 bit, signed int */
    int16_t outy = 0;
    outy |= (outy_h << 8);
    outy |= outy_l;

    return outy;
}


int16_t esp_i2c_lis3mdl_get_z(i2c_master_dev_handle_t *dev_handle) {
    /* Get the higher 8 bits from their respective register */
    uint8_t buf = OUTZ_H;
    uint8_t outz_h;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outz_h, 1, -1));

    /* Get the lower 8 bits from their respective register */
    buf = OUTZ_L;
    uint8_t outz_l;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*dev_handle, &buf, \
        sizeof(buf), &outz_l, 1, -1));

    /* Combine the 2 sets of 8 bits into one 16 bit, signed int */
    int16_t outz = 0;
    outz |= (outz_h << 8);
    outz |= outz_l;

    return outz;
}
