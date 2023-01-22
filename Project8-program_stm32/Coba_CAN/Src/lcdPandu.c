/*
 * lcd.c
 *
 *  Created on: Jul 4, 2016
 *      Author: Pandu Surya Tantra
 */

#include"lcdPandu.h"

uint8_t lcd_address[4];
uint8_t lcd_max_column;
uint8_t lcd_max_row;
uint8_t lcd2_address[4];
uint8_t lcd2_max_column;
uint8_t lcd2_max_row;

void lcd_init(uint8_t column, uint8_t row)
{
/*
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = lcd_rs_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(lcd_rs_port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = lcd_e_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(lcd_e_port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = lcd_db4_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(lcd_db4_port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = lcd_db5_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(lcd_db5_port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = lcd_db6_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(lcd_db6_port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = lcd_db7_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(lcd_db7_port, &GPIO_InitStructure);
*/

	lcd_address[0] = 0x00;
	lcd_address[1] = 0x40;
	lcd_address[2] = 0x00 + column;
	lcd_address[3] = 0x40 + column;

	lcd_max_column = column;
	lcd_max_row = row;

	lcd_rs_reset;

	//delay_ms(100);
	HAL_Delay(100);
	lcd_4bit(0b0011);
	//delay_ms(5);
	HAL_Delay(5);
	lcd_4bit(0b0011);
	//delay_ms(1);
	HAL_Delay(1);
	lcd_4bit(0b0011);
	//delay_ms(1);
	HAL_Delay(1);
	lcd_4bit(0b0010);
	//delay_ms(1);
	HAL_Delay(1);
	lcd_8bit(0b00101000);
	//delay_ms(1);
	lcd_8bit(0b00001000);
	//delay_ms(1);
	HAL_Delay(1);
	lcd_8bit(0b00000001);
	//delay_ms(3);
	HAL_Delay(3);
	lcd_8bit(0b00000110);
	//delay_ms(1);
	HAL_Delay(1);
	lcd_8bit(0b00001100);
	//delay_ms(1);
	HAL_Delay(1);

	//---------------------------------------
	//Tulisan angka berukuran besar
	//Segmen-segmennya dibuat sebanyak 8 buah
	//Semoga bermanfaat
	//---------------------------------------

	//Tepi kiri atas
	lcd_rs_reset;
	lcd_8bit(0x40);
	lcd_rs_set;
	lcd_8bit(0b00111);
	lcd_8bit(0b01111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);

	//Tepi kiri bawah
	lcd_rs_reset;
	lcd_8bit(0x48);
	lcd_rs_set;
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b01111);
	lcd_8bit(0b00111);

	//Tepi kanan atas
	lcd_rs_reset;
	lcd_8bit(0x50);
	lcd_rs_set;
	lcd_8bit(0b11100);
	lcd_8bit(0b11110);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);

	//Tepi kanan bawah
	lcd_rs_reset;
	lcd_8bit(0x58);
	lcd_rs_set;
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11110);
	lcd_8bit(0b11100);

	//Tengah-Atas
	lcd_rs_reset;
	lcd_8bit(0x60);
	lcd_rs_set;
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b00000);
	lcd_8bit(0b00000);
	lcd_8bit(0b00000);
	lcd_8bit(0b00000);
	lcd_8bit(0b00000);

	//Tengah-Tengah
	lcd_rs_reset;
	lcd_8bit(0x68);
	lcd_rs_set;
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b00000);
	lcd_8bit(0b00000);
	lcd_8bit(0b00000);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);

	//Tengah-Bawah
	lcd_rs_reset;
	lcd_8bit(0x70);
	lcd_rs_set;
	lcd_8bit(0b00000);
	lcd_8bit(0b00000);
	lcd_8bit(0b00000);
	lcd_8bit(0b00000);
	lcd_8bit(0b00000);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);

	//Penuh
	lcd_rs_reset;
	lcd_8bit(0x78);
	lcd_rs_set;
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
	lcd_8bit(0b11111);
}

void lcd_gotoxy(uint8_t column, uint8_t row)
{
	if(column >= lcd_max_column) column = 0;
	if(row >= lcd_max_row) row = 0;

	lcd_rs_reset;
	lcd_8bit(0x80 | (lcd_address[row] + column));
	//delay_us(37);
	HAL_Delay(0.037);
}

void lcd_clear(void)
{
	lcd_rs_reset;
	lcd_8bit(0x01);
	//delay_ms(3);
	HAL_Delay(3);
}

void lcd_putc(uint8_t data)
{
	lcd_rs_set;
	lcd_8bit(data);
	//delay_us(37);
	HAL_Delay(0.037);
}

void lcd_puts(char* data)
{
	while(*data)
		lcd_putc(*data++);
}

void lcd_print(uint8_t column, uint8_t row, char* data)
{
	uint8_t current_column = column;
	uint8_t current_row = row;

	while(*data)
	{
		if(current_column >= lcd_max_column && current_row >= lcd_max_row)
		{
			current_column = 0;
			current_row = 0;

			lcd_gotoxy(current_column,current_row);
			lcd_putc(*data++);
			current_column++;
		}
		else if(current_column >= lcd_max_column)
		{
			current_column = 0;
			current_row++;

			lcd_gotoxy(current_column,current_row);
			lcd_putc(*data++);
			current_column++;
		}
		else
		{
			lcd_gotoxy(current_column,current_row);
			lcd_putc(*data++);
			current_column++;
		}
	}
}

void lcd_4bit(uint8_t data)
{
/*	GPIO_WriteBit(lcd_db7_port, lcd_db7_pin, (data & 0x08) >> 3);
	GPIO_WriteBit(lcd_db6_port, lcd_db6_pin, (data & 0x04) >> 2);
	GPIO_WriteBit(lcd_db5_port, lcd_db5_pin, (data & 0x02) >> 1);
	GPIO_WriteBit(lcd_db4_port, lcd_db4_pin, (data & 0x01) >> 0);*/

	HAL_GPIO_WritePin(lcd_db7_GPIO_Port,lcd_db7_Pin,(data & 0x08)>>3);
	HAL_GPIO_WritePin(lcd_db6_GPIO_Port,lcd_db6_Pin,(data & 0x04)>>2);
	HAL_GPIO_WritePin(lcd_db5_GPIO_Port,lcd_db5_Pin,(data & 0x02)>>1);
	HAL_GPIO_WritePin(lcd_db4_GPIO_Port,lcd_db4_Pin,(data & 0x01)>>0);




	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e_set;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e_reset;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e_set;
}

void lcd_8bit(uint8_t data)
{
/*	GPIO_WriteBit(lcd_db7_port, lcd_db7_pin, (data & 0x80) >> 7);
	GPIO_WriteBit(lcd_db6_port, lcd_db6_pin, (data & 0x40) >> 6);
	GPIO_WriteBit(lcd_db5_port, lcd_db5_pin, (data & 0x20) >> 5);
	GPIO_WriteBit(lcd_db4_port, lcd_db4_pin, (data & 0x10) >> 4);*/

	HAL_GPIO_WritePin(lcd_db7_GPIO_Port,lcd_db7_Pin,(data & 0x80)>>7);
	HAL_GPIO_WritePin(lcd_db6_GPIO_Port,lcd_db6_Pin,(data & 0x40)>>6);
	HAL_GPIO_WritePin(lcd_db5_GPIO_Port,lcd_db5_Pin,(data & 0x20)>>5);
	HAL_GPIO_WritePin(lcd_db4_GPIO_Port,lcd_db4_Pin,(data & 0x10)>>4);

	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e_set;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e_reset;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e_set;

/*	GPIO_WriteBit(lcd_db7_port, lcd_db7_pin, (data & 0x08) >> 3);
	GPIO_WriteBit(lcd_db6_port, lcd_db6_pin, (data & 0x04) >> 2);
	GPIO_WriteBit(lcd_db5_port, lcd_db5_pin, (data & 0x02) >> 1);
	GPIO_WriteBit(lcd_db4_port, lcd_db4_pin, (data & 0x01) >> 0);*/

	HAL_GPIO_WritePin(lcd_db7_GPIO_Port,lcd_db7_Pin,(data & 0x08)>>3);
	HAL_GPIO_WritePin(lcd_db6_GPIO_Port,lcd_db6_Pin,(data & 0x04)>>2);
	HAL_GPIO_WritePin(lcd_db5_GPIO_Port,lcd_db5_Pin,(data & 0x02)>>1);
	HAL_GPIO_WritePin(lcd_db4_GPIO_Port,lcd_db4_Pin,(data & 0x01)>>0);

	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e_set;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e_reset;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e_set;
}

void lcd_custom_number(uint8_t column, uint8_t row, uint8_t num)
{
	switch (num)
	{
		case 0:
			lcd_gotoxy(column, row + 0); lcd_putc(0); lcd_putc(4); lcd_putc(2);
			lcd_gotoxy(column, row + 1); lcd_putc(1); lcd_putc(6); lcd_putc(3);
			break;

		case 1:
			lcd_gotoxy(column, row + 0); lcd_putc(4); lcd_putc(2); lcd_putc(32);
			lcd_gotoxy(column, row + 1); lcd_putc(6); lcd_putc(7); lcd_putc(6);
			break;

		case 2:
			lcd_gotoxy(column, row + 0); lcd_putc(5); lcd_putc(5); lcd_putc(2);
			lcd_gotoxy(column, row + 1); lcd_putc(1); lcd_putc(6); lcd_putc(6);
			break;

		case 3:
			lcd_gotoxy(column, row + 0); lcd_putc(5); lcd_putc(5); lcd_putc(2);
			lcd_gotoxy(column, row + 1); lcd_putc(6); lcd_putc(6); lcd_putc(3);
			break;

		case 4:
			lcd_gotoxy(column, row + 0); lcd_putc(1); lcd_putc(6); lcd_putc(7);
			lcd_gotoxy(column, row + 1); lcd_putc(32); lcd_putc(32); lcd_putc(7);
			break;

		case 5:
			lcd_gotoxy(column, row + 0); lcd_putc(1); lcd_putc(5); lcd_putc(5);
			lcd_gotoxy(column, row + 1); lcd_putc(6); lcd_putc(6); lcd_putc(3);
			break;

		case 6:
			lcd_gotoxy(column, row + 0); lcd_putc(0); lcd_putc(5); lcd_putc(5);
			lcd_gotoxy(column, row + 1); lcd_putc(1); lcd_putc(6); lcd_putc(3);
			break;

		case 7:
			lcd_gotoxy(column, row + 0); lcd_putc(4); lcd_putc(4); lcd_putc(2);
			lcd_gotoxy(column, row + 1); lcd_putc(32); lcd_putc(32); lcd_putc(7);
			break;

		case 8:
			lcd_gotoxy(column, row + 0); lcd_putc(0); lcd_putc(5); lcd_putc(2);
			lcd_gotoxy(column, row + 1); lcd_putc(1); lcd_putc(6); lcd_putc(3);
			break;

		case 9:
			lcd_gotoxy(column, row + 0); lcd_putc(0); lcd_putc(5); lcd_putc(2);
			lcd_gotoxy(column, row + 1); lcd_putc(6); lcd_putc(6); lcd_putc(3);
			break;

		default:
			break;
	}
}


void lcd2_init(uint8_t column, uint8_t row)
{
/*	GPIO_InitTypeDef GPIO_InitStructure;*/

/*	GPIO_InitStructure.GPIO_Pin = lcd_e2_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(lcd_e2_port, &GPIO_InitStructure);*/

	lcd2_address[0] = 0x00;
	lcd2_address[1] = 0x40;
	lcd2_address[2] = 0x00 + column;
	lcd2_address[3] = 0x40 + column;

	lcd2_max_column = column;
	lcd2_max_row = row;

	lcd_rs_reset;
	lcd_e2_set;

	//delay_ms(100);
	HAL_Delay(100);
	lcd2_4bit(0b0011);
	//delay_ms(5);
	HAL_Delay(5);
	lcd2_4bit(0b0011);
	//delay_ms(1);
	HAL_Delay(1);
	lcd2_4bit(0b0011);
	//delay_ms(1);
	HAL_Delay(1);
	lcd2_4bit(0b0010);
	//delay_ms(1);
	HAL_Delay(1);
	lcd2_8bit(0b00101000);
	//delay_ms(1);
	HAL_Delay(1);
	lcd2_8bit(0b00001000);
	//delay_ms(1);
	HAL_Delay(1);
	lcd2_8bit(0b00000001);
	//delay_ms(3);
	HAL_Delay(3);
	lcd2_8bit(0b00000110);
	//delay_ms(1);
	HAL_Delay(1);
	lcd2_8bit(0b00001100);
	//delay_ms(1);
	HAL_Delay(1);

	//---------------------------------------
	//Tulisan angka berukuran besar
	//Segmen-segmennya dibuat sebanyak 8 buah
	//Semoga bermanfaat
	//---------------------------------------

	//Tepi kiri atas
	lcd_rs_reset;
	lcd2_8bit(0x40);
	lcd_rs_set;
	lcd2_8bit(0b00111);
	lcd2_8bit(0b01111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);

	//Tepi kiri bawah
	lcd_rs_reset;
	lcd2_8bit(0x48);
	lcd_rs_set;
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b01111);
	lcd2_8bit(0b00111);

	//Tepi kanan atas
	lcd_rs_reset;
	lcd2_8bit(0x50);
	lcd_rs_set;
	lcd2_8bit(0b11100);
	lcd2_8bit(0b11110);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);

	//Tepi kanan bawah
	lcd_rs_reset;
	lcd2_8bit(0x58);
	lcd_rs_set;
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11110);
	lcd2_8bit(0b11100);

	//Tengah-Atas
	lcd_rs_reset;
	lcd2_8bit(0x60);
	lcd_rs_set;
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b00000);

	//Tengah-Tengah
	lcd_rs_reset;
	lcd2_8bit(0x68);
	lcd_rs_set;
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);

	//Tengah-Bawah
	lcd_rs_reset;
	lcd2_8bit(0x70);
	lcd_rs_set;
	lcd2_8bit(0b00000);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b00000);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);

	//Penuh
	lcd_rs_reset;
	lcd2_8bit(0x78);
	lcd_rs_set;
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
	lcd2_8bit(0b11111);
}

void lcd2_gotoxy(uint8_t column, uint8_t row)
{
	if(column >= lcd2_max_column) column = 0;
	if(row >= lcd2_max_row) row = 0;

	lcd_rs_reset;
	lcd2_8bit(0x80 | (lcd2_address[row] + column));
	//delay_us(37);
	HAL_Delay(0.037);
}

void lcd2_clear(void)
{
	lcd_rs_reset;
	lcd2_8bit(0x01);
	//delay_ms(3);
	HAL_Delay(3);
}

void lcd2_putc(uint8_t data)
{
	lcd_rs_set;
	lcd2_8bit(data);
	//delay_us(37);
	HAL_Delay(0.037);
}

void lcd2_puts(char* data)
{
	while(*data)
		lcd2_putc(*data++);
}

void lcd2_print(uint8_t column, uint8_t row, char* data)
{
	uint8_t current_column = column;
	uint8_t current_row = row;

	while(*data)
	{
		if(current_column >= lcd2_max_column && current_row >= lcd2_max_row)
		{
			current_column = 0;
			current_row = 0;

			lcd2_gotoxy(current_column,current_row);
			lcd2_putc(*data++);
			current_column++;
		}
		else if(current_column >= lcd2_max_column)
		{
			current_column = 0;
			current_row++;

			lcd2_gotoxy(current_column,current_row);
			lcd2_putc(*data++);
			current_column++;
		}
		else
		{
			lcd2_gotoxy(current_column,current_row);
			lcd2_putc(*data++);
			current_column++;
		}
	}
}

void lcd2_4bit(uint8_t data)
{
/*
	GPIO_WriteBit(lcd_db7_port, lcd_db7_pin, (data & 0x08) >> 3);
	GPIO_WriteBit(lcd_db6_port, lcd_db6_pin, (data & 0x04) >> 2);
	GPIO_WriteBit(lcd_db5_port, lcd_db5_pin, (data & 0x02) >> 1);
	GPIO_WriteBit(lcd_db4_port, lcd_db4_pin, (data & 0x01) >> 0);
*/
	HAL_GPIO_WritePin(lcd_db7_GPIO_Port,lcd_db7_Pin,(data & 0x08) >> 3);
	HAL_GPIO_WritePin(lcd_db6_GPIO_Port,lcd_db6_Pin,(data & 0x04) >> 2);
	HAL_GPIO_WritePin(lcd_db5_GPIO_Port,lcd_db5_Pin,(data & 0x02) >> 1);
	HAL_GPIO_WritePin(lcd_db4_GPIO_Port,lcd_db4_Pin,(data & 0x01) >> 0);



	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e2_set;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e2_reset;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e2_set;
}

void lcd2_8bit(uint8_t data)
{
/*	GPIO_WriteBit(lcd_db7_port, lcd_db7_pin, (data & 0x80) >> 7);
	GPIO_WriteBit(lcd_db6_port, lcd_db6_pin, (data & 0x40) >> 6);
	GPIO_WriteBit(lcd_db5_port, lcd_db5_pin, (data & 0x20) >> 5);
	GPIO_WriteBit(lcd_db4_port, lcd_db4_pin, (data & 0x10) >> 4);*/


	HAL_GPIO_WritePin(lcd_db7_GPIO_Port,lcd_db7_Pin,(data & 0x80) >> 7);
	HAL_GPIO_WritePin(lcd_db6_GPIO_Port,lcd_db6_Pin,(data & 0x40) >> 6);
	HAL_GPIO_WritePin(lcd_db5_GPIO_Port,lcd_db5_Pin,(data & 0x20) >> 5);
	HAL_GPIO_WritePin(lcd_db4_GPIO_Port,lcd_db4_Pin,(data & 0x10) >> 4);

	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e2_set;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e2_reset;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e2_set;

/*	GPIO_WriteBit(lcd_db7_port, lcd_db7_pin, (data & 0x08) >> 3);
	GPIO_WriteBit(lcd_db6_port, lcd_db6_pin, (data & 0x04) >> 2);
	GPIO_WriteBit(lcd_db5_port, lcd_db5_pin, (data & 0x02) >> 1);
	GPIO_WriteBit(lcd_db4_port, lcd_db4_pin, (data & 0x01) >> 0);*/

	HAL_GPIO_WritePin(lcd_db7_GPIO_Port,lcd_db7_Pin,(data & 0x08) >> 3);
	HAL_GPIO_WritePin(lcd_db6_GPIO_Port,lcd_db6_Pin,(data & 0x04) >> 2);
	HAL_GPIO_WritePin(lcd_db5_GPIO_Port,lcd_db5_Pin,(data & 0x02) >> 1);
	HAL_GPIO_WritePin(lcd_db4_GPIO_Port,lcd_db4_Pin,(data & 0x01) >> 0);

	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e2_set;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e2_reset;
	//delay_us(37);
	HAL_Delay(0.037);
	lcd_e2_set;
}

void lcd2_custom_number(uint8_t column, uint8_t row, uint8_t num)
{
	switch (num)
	{
		case 0:
			lcd2_gotoxy(column, row + 0); lcd2_putc(0); lcd2_putc(4); lcd2_putc(2);
			lcd2_gotoxy(column, row + 1); lcd2_putc(1); lcd2_putc(6); lcd2_putc(3);
			break;

		case 1:
			lcd2_gotoxy(column, row + 0); lcd2_putc(4); lcd2_putc(2); lcd2_putc(32);
			lcd2_gotoxy(column, row + 1); lcd2_putc(6); lcd2_putc(7); lcd2_putc(6);
			break;

		case 2:
			lcd2_gotoxy(column, row + 0); lcd2_putc(5); lcd2_putc(5); lcd2_putc(2);
			lcd2_gotoxy(column, row + 1); lcd2_putc(1); lcd2_putc(6); lcd2_putc(6);
			break;

		case 3:
			lcd2_gotoxy(column, row + 0); lcd2_putc(5); lcd2_putc(5); lcd2_putc(2);
			lcd2_gotoxy(column, row + 1); lcd2_putc(6); lcd2_putc(6); lcd2_putc(3);
			break;

		case 4:
			lcd2_gotoxy(column, row + 0); lcd2_putc(1); lcd2_putc(6); lcd2_putc(7);
			lcd2_gotoxy(column, row + 1); lcd2_putc(32); lcd2_putc(32); lcd2_putc(7);
			break;

		case 5:
			lcd2_gotoxy(column, row + 0); lcd2_putc(1); lcd2_putc(5); lcd2_putc(5);
			lcd2_gotoxy(column, row + 1); lcd2_putc(6); lcd2_putc(6); lcd2_putc(3);
			break;

		case 6:
			lcd2_gotoxy(column, row + 0); lcd2_putc(0); lcd2_putc(5); lcd2_putc(5);
			lcd2_gotoxy(column, row + 1); lcd2_putc(1); lcd2_putc(6); lcd2_putc(3);
			break;

		case 7:
			lcd2_gotoxy(column, row + 0); lcd2_putc(4); lcd2_putc(4); lcd2_putc(2);
			lcd2_gotoxy(column, row + 1); lcd2_putc(32); lcd2_putc(32); lcd2_putc(7);
			break;

		case 8:
			lcd2_gotoxy(column, row + 0); lcd2_putc(0); lcd2_putc(5); lcd2_putc(2);
			lcd2_gotoxy(column, row + 1); lcd2_putc(1); lcd2_putc(6); lcd2_putc(3);
			break;

		case 9:
			lcd2_gotoxy(column, row + 0); lcd2_putc(0); lcd2_putc(5); lcd2_putc(2);
			lcd2_gotoxy(column, row + 1); lcd2_putc(6); lcd2_putc(6); lcd2_putc(3);
			break;

		default:
			break;
	}
}
