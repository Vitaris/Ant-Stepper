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
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"
#include "pico/stdlib.h"

lcd_t lcd_create(uint32_t RS, uint32_t RW, uint32_t EN, uint32_t D4, uint32_t D5,
 				uint32_t D6, uint32_t D7, uint32_t COL, uint32_t ROW)
{
	// Create lcd data structure
	lcd_t lcd = (lcd_t)malloc(sizeof(struct lcd_controller));

	lcd->data[0] = D4;
	lcd->data[1] = D5;
	lcd->data[2] = D6;
	lcd->data[3] = D7;
	lcd->RS = RS;
	lcd->RW = RW;
	lcd->EN = EN;

	gpio_init(lcd->data[0]);
    gpio_set_dir(lcd->data[0], GPIO_OUT);
	gpio_init(lcd->data[1]);
    gpio_set_dir(lcd->data[1], GPIO_OUT);
	gpio_init(lcd->data[2]);
    gpio_set_dir(lcd->data[2], GPIO_OUT);
	gpio_init(lcd->data[3]);
    gpio_set_dir(lcd->data[3], GPIO_OUT);
	gpio_init(lcd->RS);
    gpio_set_dir(lcd->RS, GPIO_OUT);
	gpio_init(lcd->RW);
    gpio_set_dir(lcd->RW, GPIO_OUT);
	gpio_init(lcd->EN);
    gpio_set_dir(lcd->EN, GPIO_OUT);
	
	lcd->COL = COL;
	lcd->ROW = ROW;

	lcd->Xcurrent = 0;
	lcd->Ycurrent = 0;

	lcd_init(lcd);

	return lcd;
}

void e_blink(lcd_t lcd)
{
	gpio_put(lcd->EN, 1);
	busy_wait_us(DELAY);
	gpio_put(lcd->EN, 0);
	busy_wait_us(DELAY);
}

void command4bit(lcd_t lcd, uint8_t cmd)
{
	gpio_put(lcd->data[0], (cmd & 0x01) >> 0);
	gpio_put(lcd->data[1], (cmd & 0x02) >> 1);
	gpio_put(lcd->data[2], (cmd & 0x04) >> 2);
	gpio_put(lcd->data[3], (cmd & 0x08) >> 3);
	e_blink(lcd);
}

void command(lcd_t lcd, uint8_t cmd)
{
	gpio_put(lcd->RS, 0);
	command4bit(lcd, cmd >> 4);
	command4bit(lcd, cmd & 0x0F);
	busy_wait_us(DELAY_250_uS);
}

void write_data(lcd_t lcd, uint8_t data)
{
	gpio_put(lcd->RS, 1);
	command4bit(lcd, data >> 4);
	command4bit(lcd, data & 0x0F);
	busy_wait_us(DELAY_250_uS);
}

void gotoxy(lcd_t lcd, uint8_t x, uint8_t y)
{
	uint8_t row_addr[4] = {0x00, 0x40, 0x14, 0x54};
	if (y >= lcd->ROW)
	{
		y = 0;
	}
	lcd->Xcurrent = x;
	lcd->Ycurrent = y;

	command(lcd, 0x80 | (x + row_addr[y]));
}

void clrscr(lcd_t lcd)
{
	command(lcd, LCD_CLR);
	busy_wait_ms(1);
}

void lcd_init(lcd_t lcd)
{
	uint8_t FuncCnTrL = 0;
	busy_wait_ms(1);
	// gotoxy(lcd, 0, 0);

	// Changing Address if --> LCD Rows>2 Line
	if (lcd->ROW > 1)
	{
		FuncCnTrL |= LCD_2LINE;
	}

	// Starts Commands to set LCD in 4Bit Interface
	command4bit(lcd, 0x03);
	busy_wait_us(DELAY_250_uS);
	command4bit(lcd, 0x03);
	busy_wait_us(DELAY_250_uS);
	command4bit(lcd, 0x03);
	busy_wait_us(DELAY_250_uS);
	command4bit(lcd, 0x02);
	busy_wait_us(DELAY_250_uS);

	// Turns Displays on - No Cursor - No Blinking - Position 0,0 - Default Font
	command(lcd, 0x20 | FuncCnTrL);
	command(lcd, 0x08 | 0x04);
	clrscr(lcd);
	command(lcd, 0x04 | 0x02);
	busy_wait_us(DELAY_250_uS);
}

void writeText(lcd_t lcd, char string[])
{
	uint8_t i = 0;
	while (string[i] != '\0')
	{
		write_data(lcd, string[i]);
		i++;
	}
}

void int2LCD(lcd_t lcd, uint8_t x, uint8_t y, uint8_t max_length, int number)
{
    gotoxy(lcd, x, y); 
    char* str;
    asprintf (&str, "%*i", max_length, number);
    writeText(lcd, str);
    free(str);
}

void float2LCD(lcd_t lcd, uint8_t x, uint8_t y, uint8_t max_length, float number)
{
    gotoxy(lcd, x, y); 
    char* str;
    asprintf (&str, "%*.4f", max_length, number);
    writeText(lcd, str);
    free(str);
}

void string2LCD(lcd_t lcd, uint8_t x, uint8_t y, char string[])
{
    gotoxy(lcd, x, y); 
    writeText(lcd, string);
}

