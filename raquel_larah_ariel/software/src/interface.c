#include <stdio.h>
#include "lcd.h"
#include "variables.h"
#include "main.h"

#define DEGREE 0xDF

static struct
{
    enum
    {
        INICIANDO,
        SELECAO,
        EDICAO,
        AQUECENDO,
        FUNCIONAMENTO,
        ERRO,
    } tipo;
    uint8_t opts;
} tela;

static uint8_t move_para_sel(uint8_t sel)
{
    switch (sel)
    {
    case 0:
    case 4:
        lcd_SendCmd(0xC0 + 15);
        break;
    case 1:
        lcd_SendCmd(0x80 + 00);
        break;
    case 2:
        lcd_SendCmd(0xC0 + 00);
        break;
    case 3:
        lcd_SendCmd(0x80 + 15);
        break;
    default:
        return 1;
    }
    return 0;
}

uint8_t tela_selecao(uint8_t sel)
{
    if (tela.tipo != SELECAO)
    {
        lcd_SendCmd(0x01); // apaga tudo

        lcd_SendCmd(0x80 + 1); // move para 0,1
        lcd_SendChar('T');
        lcd_SendChar(':');
        lcd_Write(variables->str.temperatura);
        lcd_SendChar(DEGREE);
        lcd_SendChar('C');

        lcd_SendCmd(0x80 + 9); // move para 0,9
        lcd_SendChar('i');
        lcd_SendChar(':');
        lcd_Write(variables->str.intervalo);
        lcd_SendChar('m');
        lcd_SendChar('i');
        lcd_SendChar('n');

        lcd_SendCmd(0xC0 + 1); // move para 1,1
        lcd_SendChar('t');
        lcd_SendChar(':');
        lcd_Write(variables->str.tempo);
        lcd_SendChar('h');

        lcd_SendCmd(0xC0 + 9); // move para 1,9
        lcd_SendChar('V');
        lcd_SendChar(':');
        lcd_Write(variables->str.velocidade);
        lcd_SendChar('%');
    }
    else if (tela.opts != sel)
    {
        if (move_para_sel(tela.opts))
            return 1;
        lcd_SendChar(' ');
    }

    if (move_para_sel(sel))
        return 1;

    if (sel < 3)
        lcd_SendChar('>');
    else
        lcd_SendChar('<');

    tela.tipo = SELECAO;
    tela.opts = sel;
    return 0;
}

uint8_t tela_edicao(uint8_t sel)
{
    if (tela.tipo != EDICAO || tela.opts != sel)
    {
        lcd_SendCmd(0x01);
        switch (sel)
        {
        case 0:
            // ................
            // ...Temperatura..
            // ......TT°C......
            lcd_SendCmd(0x80 + 3);
            lcd_Write("Temperatura");
            lcd_SendCmd(0xC0 + 8);
            lcd_SendChar(DEGREE);
            lcd_SendChar('C');
            break;
        case 1:
            // ................
            // .....Tempo......
            // .....H:MMh......
            lcd_SendCmd(0x80 + 5);
            lcd_Write("Tempo");
            lcd_SendCmd(0xC0 + 9);
            lcd_SendChar('h');
            break;
        case 2:
            // ................
            // ...Intervalo....
            // ......Mmin......
            lcd_SendCmd(0x80 + 4);
            lcd_Write("Intervalo");
            lcd_SendCmd(0xC0 + 8);
            lcd_SendChar('m');
            lcd_SendChar('i');
            lcd_SendChar('n');
            break;
        case 3:
            // ................
            // ...Velocidade...
            // ......VVV%......
            lcd_SendCmd(0x80 + 3);
            lcd_Write("Velocidade");
            lcd_SendCmd(0xC0 + 9);
            lcd_SendChar('%');
            break;
        default:
            return 1;
        }
    }

    switch (sel)
    {
    case 0:
        lcd_SendCmd(0xC0 + 6);
        lcd_Write(variables->str.temperatura);
        break;
    case 1:
        lcd_SendCmd(0xC0 + 5);
        lcd_Write(variables->str.tempo);
        break;
    case 2:
        lcd_SendCmd(0xC0 + 6);
        lcd_Write(variables->str.intervalo);
        break;
    case 3:
        lcd_SendCmd(0xC0 + 6);
        lcd_Write(variables->str.velocidade);
        break;
    default:
        return 1;
    }

    tela.tipo = EDICAO;
    tela.opts = sel;
    return 0;
}

void tela_aquecendo() {
	char temp[5];
	if (tela.tipo != AQUECENDO) {
		lcd_SendCmd(0x01);
		lcd_Write("Aquecendo");	
		lcd_SendCmd(0xC0 + 5);
		lcd_SendChar('/');
		lcd_Write(variables->str.temperatura);
		lcd_SendChar(DEGREE);
		lcd_SendChar('C');
	}
	lcd_SendCmd(0xC0 + 0);
	snprintf(temp,5,"%2d%cC", termo_temperatura, DEGREE);
	lcd_Write(temp);
	tela.tipo = AQUECENDO;
}

uint8_t tela_erro(const char *msg, uint8_t codigo) {
	if (tela.opts != ERRO) {
		lcd_SendCmd(0x01);
		lcd_Write(msg);
		lcd_SendCmd(0xC0);
		char cd[5];
		snprintf(cd, 5, "%#04x", codigo);
		lcd_Write(cd);
	} 
	return 0;
}