#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "driver/i2c_types.h"

#include "esp32-i2c-LCD1602.h"


struct esp_i2c_lcd1602 esp_i2c_lcd1602_init(i2c_master_dev_handle_t i2c_dev,
	uint8_t columns, uint8_t rows, uint8_t dotsize, uint8_t backlight) {
	/* {{{ */

	struct esp_i2c_lcd1602 ret = {
		.i2c_dev = i2c_dev,
		.columns = columns,
		.rows = rows,
		.dotsize = 0, // 5x8 dotsize
		.backlight = backlight
	};

	return ret;
	/* }}} */
}


/* See page 46 of the HD44780 datasheet for 4-bit initialization */
void esp_i2c_lcd1602_begin(struct esp_i2c_lcd1602 *esp_i2c_lcd1602) {

	/* According to the HD44780 datasheet, when the display powers up it is
	 * configured as follows:
	 *
	 * 1. Display clear
	 * 2. Function set:
	 *    DL = 1; 8-bit interface data
	 *    N = 0; 1-line display
	 *    F = 0; 5x8 dot character font
	 * 3. Display on/off control:
	 *    D = 0; Display off
	 *    C = 0; Cursor off
	 *    B = 0; Blinking off
	 * 4. Entry mode set:
	 *    I/D = 1; Increment by 1
	 *    S = 0; No shift
	 * This function exists then to change those values to what could be
	 * common-case defaults.
	 */


	/* According to page 46 of the HD44780 datasheet, we must wait for 40ms
	 * after the Vcc reaches 2.7 V before sending commands. */
	/* Sleep for 40ms */
	vTaskDelay(40 / portTICK_PERIOD_MS);

	/* Set the functionality of the LCD (E.g. here: 4-bit operation . 2 display
	 * lines, font 0 (i.e. 5x8 dots) */
	esp_i2c_lcd1602_function_set(esp_i2c_lcd1602, 4, 2, 0);

	/* Set the display control of the LCD  (E.g. here: display = on,
	 * cursor = on, cursor blinking = on) */
	esp_i2c_lcd1602_display_control(esp_i2c_lcd1602, 1, 1, 1);

	/* Clear the display */
	esp_i2c_lcd1602_clear_display(esp_i2c_lcd1602);

	/* Set the LCD to have new characters appear left-to-right, and do not
	 * shift the display upon receiving a new character */
	esp_i2c_lcd1602_entry_mode_set(esp_i2c_lcd1602, 1, 0);

	/* Move the cursor back to the beginning */
	esp_i2c_lcd1602_cursor_home(esp_i2c_lcd1602);
}


/** Clear the display, and set the cursor position to zero */
void esp_i2c_lcd1602_clear_display(struct esp_i2c_lcd1602 *esp_i2c_lcd1602) {
	/* {{{ */
	/* See page 28 of the HD44780 datasheet */

	/* Set DB7 to DB0 appropriately */
	uint8_t data = LCD_CLEARDISPLAY; // 00000001
	/* Set RS and R/W appropriately */
	uint8_t mode = set_mode(0, 0);
	/* Respect backlight settings for the LCD */
	mode |= esp_i2c_lcd1602->backlight;

	esp_i2c_lcd1602_write_4bitmode(esp_i2c_lcd1602, data, mode);

	// TODO:
	/* See page 24 of the HD44780 datasheet. No time is listed for this
	 * instruction. */
	/* Sleep for 2000µs */
	vTaskDelay(2 / portTICK_PERIOD_MS);
	/* }}} */
}


/*/1** Set the cursor position *1/ */
/*void esp_i2c_lcd1602_set_cursor_pos(struct esp_i2c_lcd1602 *esp_i2c_lcd1602, uint8_t ac) { */
/*	/1* {{{ *1/ */
/*	/1* See page 21, 24 of the HD44780 datasheet *1/ */

/*	/1* Set the command type *1/ */
/*	uint8_t data = LCD_SETDDRAMADDR; */
/*	data |= ac; */
/*	/1* Set RS and R/W appropriately *1/ */
/*	uint8_t mode = set_mode(0, 0); */
/*	/1* Respect backlight settings for the LCD *1/ */
/*	mode |= esp_i2c_lcd1602->backlight; */

/*	esp_i2c_lcd1602_write_4bitmode(esp_i2c_lcd1602, data, mode); */

/*	/1* According to page 24 of the HD44780 datasheet, this operation takes a */
/*	 * maximum of 37µs *1/ */
/*	/1* Sleep for 37µs *1/ */
/*	struct timespec a = (struct timespec) { .tv_sec = 0, .tv_nsec = 37000}; */
/*	nanosleep(&a, NULL); */
/*	/1* }}} *1/ */
/*} */


/** Move the cursor to (0, 0) */
void esp_i2c_lcd1602_cursor_home(struct esp_i2c_lcd1602 *esp_i2c_lcd1602) {
	/* {{{ */
	/* See page 24 of the HD44780 datasheet */

	/* Set DB7 to DB0 appropriately */
	uint8_t data = LCD_RETURNHOME;
	/* Set RS and R/W appropriately */
	uint8_t mode = set_mode(0, 0);
	/* Respect backlight settings for the LCD */
	mode |= esp_i2c_lcd1602->backlight;

	esp_i2c_lcd1602_write_4bitmode(esp_i2c_lcd1602, data, mode);

	/* According to page 24 of the HD44780 datasheet, this operation takes a
	 * maximum of 1.52ms */
	/* Sleep for 1.52ms */
	vTaskDelay(2 / portTICK_PERIOD_MS);
	/* }}} */
}


/** Set the entry mode for the i2c LCD */
void esp_i2c_lcd1602_entry_mode_set(struct esp_i2c_lcd1602 *esp_i2c_lcd1602, uint8_t
	increment, uint8_t shift) {
	/* {{{ */
	/* See page 24, 26, 40, 42 of the HD44780 datasheet */

	/* Set the command type */
	uint8_t data = LCD_ENTRYMODESET;
	/* Set the details of the command, i.e., set the entry mode
	 * - Increment (have the cursor move to the right after receiving a new
	 *   character.
	 * - Decrement (have the cursor move to the left after receiving a new
	 *   character.
	 * - Shift (move the whole display in the direction of the cursor moves
	 *   (which depends on whether increment or decrement is set) after
	 *   receiving a new character).
	 * - No shift (do NOT move the whole display in the direction that the
	 *   cursor moves (which depends on whether increment or decrement is set)
	 *   after receiving a new character).
	 */
	if (increment == 1) {
		data |= LCD_ENTRYINCREMENT;
		esp_i2c_lcd1602->entry_shift_increment = 1;
	} else if (increment == 0) {
		data |= LCD_ENTRYDECREMENT;
		esp_i2c_lcd1602->entry_shift_increment = 0;
	}
	if (shift == 1) {
		data |= LCD_ENTRYSHIFT;
		esp_i2c_lcd1602->entry_shift = 1;
	} else if (shift == 0) {
		data |= LCD_ENTRYNOSHIFT;
		esp_i2c_lcd1602->entry_shift = 0;
	}
	/* Set both RS and R/W to 0 */
	uint8_t mode = set_mode(0, 0);
	/* Respect backlight settings for the LCD */
	mode |= esp_i2c_lcd1602->backlight;

	esp_i2c_lcd1602_write_4bitmode(esp_i2c_lcd1602, data, mode);

	/* According to page 24 of the HD44780 datasheet, this operation takes a
	 * maximum of 37µs */
	/* Sleep for 37µs */
	vTaskDelay(10 / portTICK_PERIOD_MS);
	/* }}} */
}


/** Set the display controls for the i2c LCD */
void esp_i2c_lcd1602_display_control(struct esp_i2c_lcd1602 *esp_i2c_lcd1602,
	uint8_t display, uint8_t cursor, uint8_t cursorblinking) {
	/* {{{ */
	/* See page 24, 42 of the HD44780 datasheet */

	/* Set the command type */
	uint8_t data = LCD_DISPLAYONOFFCONTROL;
	/* Set the details of the command, i.e., set the display settings */
	if (display == 1) data |= LCD_DISPLAYON;
	else if (display == 0) data |= LCD_DISPLAYOFF;
	if (cursor == 1) data |= LCD_CURSORON;
	else if (cursor == 0) data |= LCD_CURSOROFF;
	if (cursorblinking == 1) data |= LCD_BLINKON;
	else if (cursorblinking == 0) data |= LCD_BLINKOFF;
	/* Set both RS and R/W to 0 */
	uint8_t mode = set_mode(0, 0);
	/* Respect backlight settings for the LCD */
	mode |= esp_i2c_lcd1602->backlight;

	esp_i2c_lcd1602_write_4bitmode(esp_i2c_lcd1602, data, mode);

	/* According to page 24 of the HD44780 datasheet, this operation takes a
	 * maximum of 37µs */
	/* Sleep for 37µs */
	vTaskDelay(10 / portTICK_PERIOD_MS);
	/* }}} */
}


/*/1** Shift the screen or the cursor to the right or to the left *1/ */
/*void esp_i2c_lcd1602_shift(struct esp_i2c_lcd1602 *esp_i2c_lcd1602, uint8_t screen_cursor, */
/*	uint8_t right_left) { */
/*	/1* {{{ *1/ */
/*	/1* See page 24, 29 of the HD44780 datasheet *1/ */

/*	/1* Set the data *1/ */
/*	uint8_t data = LCD_CURSORDISPLAYSHIFT; // 00010000 */
/*	if (screen_cursor == 1) data |= LCD_DISPLAYMOVE; */
/*	else if (screen_cursor == 0) data |= LCD_CURSORMOVE; */
/*	if (right_left == 1) data |= LCD_MOVERIGHT; */
/*	else if (right_left == 0) data |= LCD_MOVELEFT; */
/*	/1* Set RS and R/W appropriately *1/ */
/*	uint8_t mode = set_mode(0, 0); */
/*	/1* Respect backlight settings for the LCD *1/ */
/*	mode |= esp_i2c_lcd1602->backlight; */

/*	esp_i2c_lcd1602_write_4bitmode(esp_i2c_lcd1602, data, mode); */

/*	/1* According to page 24 of the HD44780 datasheet, this operation takes a */
/*	 * maximum of 37µs *1/ */
/*	/1* Sleep for 37µs *1/ */
/*	struct timespec a = (struct timespec) { .tv_sec = 0, .tv_nsec = 37000}; */
/*	nanosleep(&a, NULL); */
/*	/1* }}} *1/ */
/*} */


/** Establish the functionality settings for the i2c LCD */
void esp_i2c_lcd1602_function_set(struct esp_i2c_lcd1602 *esp_i2c_lcd1602,
	uint8_t data_length, uint8_t display_lines, uint8_t font) {
	/* {{{ */
	/* See page 24, 27, 29 of the HD44780 datasheet */

	/* Set the command type */
	uint8_t data = LCD_FUNCTIONSET; // 00100000
	/* Set the details of the command, i.e. ... */
	/* ... Set the interface data length ... */
	if (data_length == 8) data |= LCD_8BITMODE;
	else if (data_length == 4) data |= LCD_4BITMODE;
	/* ... Set the number of display lines ... */
	if (display_lines == 2) data |= LCD_2LINE;
	else if (display_lines == 1) data |= LCD_1LINE;
	/* ... and the character font */
	if (font == 1) data |= LCD_5x10DOTS;
	else if (font == 0) data |= LCD_5x8DOTS;
	/* Set both RS and R/W to 0 */
	uint8_t mode = set_mode(0, 0);
	/* Respect backlight settings for the LCD */
	mode |= esp_i2c_lcd1602->backlight;

	esp_i2c_lcd1602_write_4bitmode(esp_i2c_lcd1602, data, mode);

	/* According to page 24 of the HD44780 datasheet, this operation takes a
	 * maximum of 37µs */
	/* Sleep for 37µs */
	vTaskDelay(10 / portTICK_PERIOD_MS);
	/* }}} */
}


void esp_i2c_lcd1602_send_char(struct esp_i2c_lcd1602 *esp_i2c_lcd1602, char c) {
	/* {{{ */
	/* See page 25 of the HD44780 datasheet */

	/* Set the data */
	uint8_t data = c;
	/* Set RS and R/W appropriately */
	uint8_t mode = set_mode(1, 0);
	/* Respect backlight settings for the LCD */
	mode |= esp_i2c_lcd1602->backlight;

	esp_i2c_lcd1602_write_4bitmode(esp_i2c_lcd1602, data, mode);

	/* According to page 25 of the HD44780 datasheet, this operation takes a
	 * maximum of 37µs + 4µs */
	/* Sleep for 41µs */
	vTaskDelay(10 / portTICK_PERIOD_MS);
	/* }}} */
}


/*/1** Set the backlight setting for the LCD *1/ */
/*void esp_i2c_lcd1602_set_backlight(struct esp_i2c_lcd1602 *esp_i2c_lcd1602, uint8_t */
/*	backlight) { */
/*	/1* {{{ *1/ */
/*	// TODO: */
/*	/1* See page 24, 26, 40, 42 of the HD44780 datasheet *1/ */

/*	/1* There is no "set backlight" command for the LCD. Instead, the backlight */
/*	 * is set on or off with each sent command. That said, we can change the */
/*	 * stored value for the backlight that exists in the LCD struct, and use */
/*	 * the entry mode set command as a means of immediately bringing about */
/*	 * that change without changing anything else (i.e. without sending a */
/*	 * character or shifting the display) *1/ */

/*	/1* Set the command type *1/ */
/*	uint8_t data = LCD_ENTRYMODESET; */
/*	/1* Maintain the LCD  entry mode settings *1/ */
/*	data |= esp_i2c_lcd1602->entry_shift_increment; */
/*	data |= esp_i2c_lcd1602->entry_shift; */
/*	/1* Set both RS and R/W to 0 *1/ */
/*	uint8_t mode = set_mode(0, 0); */

/*	esp_i2c_lcd1602_write_4bitmode(esp_i2c_lcd1602, data, mode); */

/*	/1* According to page 24 of the HD44780 datasheet, this operation takes a */
/*	 * maximum of 37µs *1/ */
/*	/1* Sleep for 37µs *1/ */
/*	struct timespec a = (struct timespec) { .tv_sec = 0, .tv_nsec = 37000}; */
/*	nanosleep(&a, NULL); */
/*	/1* }}} *1/ */
/*} */


/** Write to the i2c LCD in 4 bit mode. The real difference between writing
 * and reading in 4 bit vs. 8 bit mode is that two 4 bit instructions are
 * used to accomplish what would be accomplished in one 8 bit instruction.
 * Compare the last stages of page 45 and page 46 of the HD44780 datasheet
 * to see what I mean.
 */
void esp_i2c_lcd1602_write_4bits(struct esp_i2c_lcd1602 *esp_i2c_lcd,
	uint8_t data_and_mode) {

    /* 1. Send data */
	ESP_ERROR_CHECK(i2c_master_transmit(esp_i2c_lcd->i2c_dev, &data_and_mode, sizeof(uint8_t), -1));
    // TODO: this delay is probably too long
	vTaskDelay(10 / portTICK_PERIOD_MS);

    /* 2. Send data with enable bit */
    uint8_t data_and_mode_and_enable = data_and_mode | E;
	ESP_ERROR_CHECK(i2c_master_transmit(esp_i2c_lcd->i2c_dev, &data_and_mode_and_enable, sizeof(uint8_t), -1));
    // TODO: this delay is probably too long
	vTaskDelay(10 / portTICK_PERIOD_MS);

    /* 3. Send data with disable bit */
    uint8_t data_and_mode_and_disable = data_and_mode & ~E;
	ESP_ERROR_CHECK(i2c_master_transmit(esp_i2c_lcd->i2c_dev, &data_and_mode_and_disable, sizeof(uint8_t), -1));
}


/** Send an instruction to the i2c LCD in 4 bit mode. The real difference
 * between doing so in 4 bit mode vs. 8 bit mode is that two 4 bit instructions
 * are used to accomplish what would be accomplished in one 8 bit instruction.
 * Compare the last stages of page 45 and page 46 of the HD44780 datasheet to
 * see what I mean.
 */
void esp_i2c_lcd1602_write_4bitmode(struct esp_i2c_lcd1602 *esp_i2c_lcd,
	uint8_t data, uint8_t mode) {

	uint8_t highnib = (data & 0xf0) | mode;
	uint8_t lownib = ((data << 4) & 0xf0) | mode;

	esp_i2c_lcd1602_write_4bits(esp_i2c_lcd, highnib);
	esp_i2c_lcd1602_write_4bits(esp_i2c_lcd, lownib);
}


uint8_t set_mode(uint8_t rs, uint8_t rw) {
	/* {{{ */
	uint8_t mode = 0x00; // 00000000

	if (rs == 1) mode |= Rs;
	if (rw == 1) mode |= Rw;

	return mode;
	/* }}} */
}
