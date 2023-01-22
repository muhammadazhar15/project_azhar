/*
 * lcd.h
 *
 *  Created on: Jul 4, 2016
 *      Author: Pandu Surya Tantra
 */



#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <stdlib.h>



#define lcd_rs_GPIO_Port GPIOB
#define lcd_rs_Pin GPIO_PIN_13

#define lcd_e_GPIO_Port GPIOB
#define lcd_e_Pin GPIO_PIN_15

#define lcd_e2_GPIO_Port GPIOD
#define lcd_e2_Pin GPIO_PIN_8

#define lcd_db4_GPIO_Port GPIOD
#define lcd_db4_Pin GPIO_PIN_9

#define lcd_db5_GPIO_Port GPIOD
#define lcd_db5_Pin GPIO_PIN_11

#define lcd_db6_GPIO_Port GPIOD
#define lcd_db6_Pin GPIO_PIN_13

#define lcd_db7_GPIO_Port GPIOD
#define lcd_db7_Pin GPIO_PIN_15


/*
#define lcd_rs_set GPIO_SetBits(lcd_rs_port, lcd_rs_pin)
#define lcd_rs_reset GPIO_ResetBits(lcd_rs_port, lcd_rs_pin)
*/
#define lcd_rs_set HAL_GPIO_WritePin(lcd_rs_GPIO_Port,lcd_rs_Pin,GPIO_PIN_SET)
#define lcd_rs_reset HAL_GPIO_WritePin(lcd_rs_GPIO_Port,lcd_rs_Pin,GPIO_PIN_RESET)

/*
#define lcd_e_set GPIO_SetBits(lcd_e_port, lcd_e_pin)
#define lcd_e_reset GPIO_ResetBits(lcd_e_port, lcd_e_pin)
*/

#define lcd_e_set HAL_GPIO_WritePin(lcd_e_GPIO_Port,lcd_e_Pin,GPIO_PIN_SET)
#define lcd_e_reset HAL_GPIO_WritePin(lcd_e_GPIO_Port,lcd_e_Pin,GPIO_PIN_RESET)


/*
#define lcd_e2_set GPIO_SetBits(lcd_e_port, lcd_e2_pin)
#define lcd_e2_reset GPIO_ResetBits(lcd_e_port, lcd_e2_pin)
*/

#define lcd_e2_set HAL_GPIO_WritePin(lcd_e2_GPIO_Port,lcd_e2_Pin,GPIO_PIN_SET)
#define lcd_e2_reset HAL_GPIO_WritePin(lcd_e2_GPIO_Port,lcd_e2_Pin,GPIO_PIN_RESET)


void lcd_init(uint8_t column, uint8_t row);
void lcd_gotoxy(uint8_t column, uint8_t row);
void lcd_clear(void);
void lcd_putc(uint8_t data);
void lcd_puts(char* data);
void lcd_print(uint8_t column, uint8_t row, char* data);
void lcd_4bit(uint8_t data);
void lcd_8bit(uint8_t data);
void lcd_custom_number(uint8_t column, uint8_t row, uint8_t num);
void lcd2_init(uint8_t column, uint8_t row);
void lcd2_gotoxy(uint8_t column, uint8_t row);
void lcd2_clear(void);
void lcd2_putc(uint8_t data);
void lcd2_puts(char* data);
void lcd2_print(uint8_t column, uint8_t row, char* data);
void lcd2_4bit(uint8_t data);
void lcd2_8bit(uint8_t data);
void lcd2_custom_number(uint8_t column, uint8_t row, uint8_t num);


