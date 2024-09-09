#include "lcd.h"

static inline void lcd_Enable()
{
    _delay_us(1);
    LCD_E_PORT |= LCD_E_MASK;
    _delay_us(1);
    LCD_E_PORT &= ~(LCD_E_MASK);
    _delay_us(45);
}

static inline void lcd_SendNibble(uint8_t nibble)
{
    LCD_D4_PORT = (LCD_D4_PORT & ~(1 << LCD_D4_PIN)) | ((((nibble) >> 0) & 1) << LCD_D4_PIN);
    LCD_D5_PORT = (LCD_D5_PORT & ~(1 << LCD_D5_PIN)) | ((((nibble) >> 1) & 1) << LCD_D5_PIN);
    LCD_D6_PORT = (LCD_D6_PORT & ~(1 << LCD_D6_PIN)) | ((((nibble) >> 2) & 1) << LCD_D6_PIN);
    LCD_D7_PORT = (LCD_D7_PORT & ~(1 << LCD_D7_PIN)) | ((((nibble) >> 3) & 1) << LCD_D7_PIN);
    lcd_Enable();
}

static inline void lcd_SendByte(uint8_t by)
{
    lcd_SendNibble(((by) & 0xF0) >> 4); // MSB
    lcd_SendNibble(by);                 // LSB
}

void lcd_SendCmd(uint8_t cmd)
{
    LCD_RS_PORT &= ~(LCD_RS_MASK);
    lcd_SendByte(cmd);
    if (cmd < 4) // alguns comandos exigem um tempo de espera antes do próximo comando
    {
        _delay_ms(2);
    }
}

void lcd_SendChar(uint8_t chr)
{
    LCD_RS_PORT |= (LCD_RS_MASK);
    lcd_SendByte(chr);
}

/**
 * @brief  Configura hardware: verificar lcd.h para mapa de pinos e nible de dados.
 * @param	Nenhum
 *
 * @retval Nenhum.
 */
void lcd_Init()
{
    // sequência ditada pelo fabricando do circuito integrado HD44780
    // o LCD será só escrito. Então, R/W é sempre zero.

    /* Configura pinos de controle */
    LCD_RS_DDR |= LCD_RS_MASK;
    LCD_E_DDR |= LCD_E_MASK;
    LCD_D4_DDR |= LCD_D4_MASK;
    LCD_D5_DDR |= LCD_D5_MASK;
    LCD_D6_DDR |= LCD_D6_MASK;
    LCD_D7_DDR |= LCD_D7_MASK;

    // RS em zero indicando que o dado para o LCD será uma instrução
    LCD_RS_PORT &= ~(LCD_RS_MASK);
    // pino de habilitação em zero
    LCD_E_PORT &= ~(LCD_E_MASK);

    // tempo para estabilizar a tensão do LCD, após VCC ultrapassar 4.5 V (na prática pode
    // ser maior).
    _delay_ms(20);

    lcd_SendNibble(0x03);
    // habilitação respeitando os tempos de resposta do LCD
    _delay_ms(5);
    lcd_Enable();
    _delay_us(200);
    lcd_Enable();

    lcd_SendNibble(0x02);

    // interface de 4 bits 2 linhas (aqui se habilita as 2 linhas)
    lcd_SendCmd(0x28);

    // desliga o display
    lcd_SendCmd(0x08);
    // limpa todo o display
    lcd_SendCmd(0x01);
    // mensagem aparente cursor inativo não piscando
    lcd_SendCmd(0x0C);
    // inicializa cursor na primeira posição a esquerda - 1a linha
    lcd_SendCmd(0x80);
}

/**
 * @brief  Escreve um string estática (sem printf) no LCD.
 * @param c: ponteiro para a string em RAM
 *
 * @retval Nenhum
 */
void lcd_Write(const char *c)
{
    for (; *c != 0; c++)
        lcd_SendChar(*c);
}
