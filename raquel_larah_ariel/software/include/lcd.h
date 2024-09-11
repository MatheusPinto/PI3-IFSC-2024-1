
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#define LCD_RS_PORT PORTB
#define LCD_RS_PINR PINB
#define LCD_RS_DDR DDRB
#define LCD_RS_PIN PINB5
#define LCD_RS_MASK (1 << LCD_RS_PIN)

#define LCD_E_PORT PORTB
#define LCD_E_PINR PINB
#define LCD_E_DDR DDRB
#define LCD_E_PIN PINB0
#define LCD_E_MASK (1 << LCD_E_PIN)

#define LCD_D4_PORT PORTB
#define LCD_D4_PINR PINB
#define LCD_D4_DDR DDRB
#define LCD_D4_PIN PINB1
#define LCD_D4_MASK (1 << LCD_D4_PIN)

#define LCD_D5_PORT PORTB
#define LCD_D5_PINR PINB
#define LCD_D5_DDR DDRB
#define LCD_D5_PIN PINB2
#define LCD_D5_MASK (1 << LCD_D5_PIN)

#define LCD_D6_PORT PORTB
#define LCD_D6_PINR PINB
#define LCD_D6_DDR DDRB
#define LCD_D6_PIN PINB3
#define LCD_D6_MASK (1 << LCD_D6_PIN)

#define LCD_D7_PORT PORTB
#define LCD_D7_PINR PINB
#define LCD_D7_DDR DDRB
#define LCD_D7_PIN PINB4
#define LCD_D7_MASK (1 << LCD_D7_PIN)

void lcd_SendCmd(uint8_t cmd);
void lcd_SendChar(uint8_t chr);

/**
 * @brief  Configura hardware: verificar lcd.h para mapa de pinos.
 * @param	Nenhum
 *
 * @retval Nenhum.
 */
void lcd_Init();

/**
 * @brief  Escreve um string estática (sem printf) no LCD.
 * @param c: ponteiro para a string em RAM
 *
 * @retval Nenhum
 */
void lcd_Write(const char *c);
