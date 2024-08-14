
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#define LCD_RS_PORT PORTD
#define LCD_RS_PINR PIND
#define LCD_RS_DDR DDRD
#define LCD_RS_PIN PIND2
#define LCD_RS_MASK (1 << LCD_RS_PIN)

#define LCD_E_PORT PORTD
#define LCD_E_PINR PIND
#define LCD_E_DDR DDRD
#define LCD_E_PIN PIND3
#define LCD_E_MASK (1 << LCD_E_PIN)

#define LCD_D4_PORT PORTD
#define LCD_D4_PINR PIND
#define LCD_D4_DDR DDRD
#define LCD_D4_PIN PIND4
#define LCD_D4_MASK (1 << LCD_D4_PIN)

#define LCD_D5_PORT PORTD
#define LCD_D5_PINR PIND
#define LCD_D5_DDR DDRD
#define LCD_D5_PIN PIND5
#define LCD_D5_MASK (1 << LCD_D5_PIN)

#define LCD_D6_PORT PORTD
#define LCD_D6_PINR PIND
#define LCD_D6_DDR DDRD
#define LCD_D6_PIN PIND6
#define LCD_D6_MASK (1 << LCD_D6_PIN)

#define LCD_D7_PORT PORTD
#define LCD_D7_PINR PIND
#define LCD_D7_DDR DDRD
#define LCD_D7_PIN PIND7
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

/**
 * @brief  Escreve um string estática (sem printf) no LCD.
 * @param c: ponteiro para a string em FLASH
 *
 * @retval Nenhum
 */
void escreve_LCD_Flash(const char *c);
