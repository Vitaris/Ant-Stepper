/*	LCD Library for HD44780 compatible LCDs

MIT License

Copyright (c) [2023] [Vitaris]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ANT_LCD_H
#define ANT_LCD_H

#include <stdbool.h>
#include <stdint.h>


#define LCD_CLR 0x01  // Screen Clear Command Address
#define LCD_DISP_ON 0x0C  // Screen On Command Address
#define LCD_DISP_OFF 0x08  // Screen Off Command Address
#define LCD_BLINK_ON 0x0D  // Cursor Blink On Command Address
#define LCD_BLINK_OFF 0x0C  // Cursor Blink Off Command Address
#define LCD_CURSOR_ON 0x0E  // Cursor On Command Address
#define LCD_CURSOR_OFF 0x0C  // Cursor Off Command Address
#define LCD_2LINE 0x08  // Address for More Than 2 Line LCDs (For Example 4*20)
#define DELAY 15  // Default Delay
#define DELAY_5_MS 1
#define DELAY_5_MS 1
#define DELAY_250_uS 250



/**
 * Structure that holds LCD instance Data, multiple instances are
 * posible using different structures for each controller
 */
struct lcd_controller {
	uint8_t data[4];
	uint8_t RS;
	uint8_t RW;
	uint8_t EN;
	uint8_t COL;
	uint8_t ROW;
	uint8_t Xcurrent;
	uint8_t Ycurrent;
};

typedef struct lcd_controller * lcd_t;

#ifdef	__cplusplus
extern "C" {
#endif
	/**
	 * @brief Creates a new LCD controller
	 *
	 * Creates a LCD controller and initializes it�s outputs
	 *
	 * @param lcd LCD controller handle
	 * @param RS RS pin number
	 * @param RW RW pin number
	 * @param EN EN pin number
	 * @param D4 D4 pin number
	 * @param D5 D5 pin number
	 * @param D6 D6 pin number
	 * @param D7 D7 pin number
	 * @param COL LCD columns
	 * @param ROW LCD rows
	 * 
	 * @return returns a lcd_t controller handle
	 */
	lcd_t lcd_create(uint32_t RS, uint32_t RW, uint32_t EN, uint32_t D4, uint32_t D5,
 					uint32_t D6, uint32_t D7, uint32_t COL, uint32_t ROW);

	/**
	 * @brief Init LCD controller
	 * 
	 * @param lcd LCD controller handle
	 * 
	 */
	void lcd_init(lcd_t lcd);

	/**
	 * @brief A pulse to EN Pin
	 * 
	 * @param lcd LCD controller handle
	 * 
	 */
	void e_blink(lcd_t lcd);

	/**
	 * @brief Command Data Transfer Function(4Bit Mode)
	 * 
	 * @param lcd LCD controller handle
	 * @param cmd 4-bit command
	 * 
	 */
	void command4bit(lcd_t lcd, uint8_t cmd);

	/**
	 * @brief Command Data Transfer Function
	 * 
	 * @param lcd LCD controller handle
	 * @param cmd 8-bit command
	 * 
	 */
	void command(lcd_t lcd, uint8_t cmd);

	/**
	 * @brief Sends RAW DATA to LCD
	 * 
	 * @param lcd LCD controller handle
	 * @param data 8-bit data
	 * 
	 */
	void write_data(lcd_t lcd, uint8_t data);

	/**
	 * @brief Cursor Position set Function
	 * 
	 * @param lcd LCD controller handle
	 * @param x X position
	 * @param y Y position
	 * 
	 */
	void gotoxy(lcd_t lcd, uint8_t x, uint8_t y);

	/**
	 * @brief Screen Clear Function
	 * 
	 * @param lcd LCD controller handle
	 * 
	 */
	void clrscr(lcd_t lcd);

	/**
	 * @brief Puts String on LCD
	 * 
	 * @param lcd LCD controller handle
	 * @param string String to be displayed
	 * 
	 */
	void writeText(lcd_t lcd, char string[]);

	/**
	 * @brief Puts Int on LCD
	 * 
	 * @param lcd LCD controller handle
	 * @param x X position
	 * @param y Y position
	 * @param max_length Max length of the number
	 * @param number Int to be displayed
	 * 
	 */
	void int2LCD(lcd_t lcd, uint8_t x, uint8_t y, uint8_t max_length, int number);

	/**
	 * @brief Puts Float on LCD
	 * 
	 * @param lcd LCD controller handle
	 * @param x X position
	 * @param y Y position
	 * @param max_length Max length of the number
	 * @param number Float to be displayed
	 * 
	 */
	void float2LCD(lcd_t lcd, uint8_t x, uint8_t y, uint8_t max_length, float number);

	/**
	 * @brief Puts String on LCD
	 * 
	 * @param lcd LCD controller handle
	 * @param x X position
	 * @param y Y position
	 * @param string String to be displayed
	 * 
	 */
	void string2LCD(lcd_t lcd, uint8_t x, uint8_t y, char string[]);



#ifdef	__cplusplus
}
#endif

#endif
