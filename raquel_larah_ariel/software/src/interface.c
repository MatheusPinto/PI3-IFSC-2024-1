#include <stdio.h>
#include "lcd.h"
#include "main.h"
#include "interface.h"

#define DEGREE 0xDF

struct variavel
{
    uint8_t changed;
    char *const repr;
};

static struct variavel vTargetTemp = {.changed = 0, .repr = "00°C"};
static struct variavel vTempo = {.changed = 0, .repr = "0:00h"};
static struct variavel vIntervalo = {.changed = 0, .repr = "0min"};
static struct variavel vVelocidade = {.changed = 0, .repr = "000%%"};
static struct variavel vError = {.changed = 0, .repr = "Err:0x00"};
static struct variavel vTemperatura = {.changed = 0, .repr = "01°C"};
static struct variavel vCorrente = {.changed = 0, .repr = "-10,1mA"};

void updateTargetTemp(VAR_TEMPERATURA_T val)
{
    snprintf(vTargetTemp.repr, 5, "%02d%cC", val, DEGREE);
    vTargetTemp.changed = 1;
}

void updateTempo(VAR_TEMPO_T val)
{
    snprintf(vTempo.repr, 6, "%d:%02dh", val / 60, val % 60);
    vTempo.changed = 1;
}

void updateIntervalo(VAR_INTERVALO_T val)
{
    snprintf(vIntervalo.repr, 5, "%dmin", val);
    vIntervalo.changed = 1;
}

void updateVelocidade(VAR_VELOCIDADE_T val)
{
    snprintf(vVelocidade.repr, 6, "%3d%%", val);
    vVelocidade.changed = 1;
}

void updateError(uint8_t val)
{
    snprintf(vError.repr, 9, "Err:%#04x", val);
    vError.changed = 1;
}

void updateTemperatura(uint8_t val)
{
    snprintf(vTemperatura.repr, 5, "%02d%cC", val, DEGREE);
    vTemperatura.changed = 1;
}

void updateCorrente(uint16_t val)
{
    int16_t map = (int16_t)((uint32_t)val * 2400 / 1023); // mapeia entre 0 e 2400
    map -= 1200;                                          // offset (mapeia entre -1200 e 1200);
    uint8_t rest = map >= 0 ? map % 10 : (-map) % 10;
    snprintf(vCorrente.repr, 8, "%+3d,%dmA", map / 100, rest);
    vCorrente.changed = 1;
}

struct tela
{
    void *opts;
    const struct
    {
        struct variavel *const v;
        uint8_t line : 1;
        uint8_t column : 4;
    } vars[];
};

static enum {
    INICIANDO,
    SELECAO,
    EDICAO,
    AQUECENDO,
    FUNCIONAMENTO,
    ERRO,
} tela_atual = INICIANDO;

/******************************************TELA DE SELECAO */
/*
.|0123456789012345|
0| T:00°C  i:0min |
1| t:0:00h V:100%>|
*/
static selecaoOpts sOpts = SO_NEXT;
const static struct tela Selecao = {
    .vars = {
        {.v = &vTargetTemp, .line = 0, .column = 3},
        {.v = &vTempo, .line = 1, .column = 3},
        {.v = &vIntervalo, .line = 0, .column = 11},
        {.v = &vVelocidade, .line = 1, .column = 11},
    },
    .opts = (void *)&sOpts,
};

void telaSelecao(selecaoOpts sel)
{
    if (tela_atual != SELECAO)
    {
        lcd_SendCmd(0x01);
        lcd_Write(" T:00"
                  "\xDF"
                  "C  i:0min ");
        lcd_SendCmd(0xC0);
        lcd_Write(" t:0:00h V:100% ");
    }
    if (sel != *(selecaoOpts *)Selecao.opts || tela_atual != SELECAO)
    {
        switch (*(selecaoOpts *)Selecao.opts)
        {
        case SO_NEXT:
        case SO_VEL:
            lcd_SendCmd(0xC0 + 15);
            break;
        case SO_TTEMP:
            lcd_SendCmd(0x80 + 0);
            break;
        case SO_TEMPO:
            lcd_SendCmd(0xC0 + 0);
            break;
        case SO_INT:
            lcd_SendCmd(0x80 + 15);
            break;
        }
        lcd_SendChar(' ');

        char sign = '?';
        switch (sel)
        {
        case SO_NEXT:
            sign = '>';
            lcd_SendCmd(0xC0 + 15);
            break;
        case SO_VEL:
            sign = '<';
            lcd_SendCmd(0xC0 + 15);
            break;
        case SO_TTEMP:
            sign = '>';
            lcd_SendCmd(0x80 + 0);
            break;
        case SO_TEMPO:
            sign = '>';
            lcd_SendCmd(0xC0 + 0);
            break;
        case SO_INT:
            sign = '<';
            lcd_SendCmd(0x80 + 15);
            break;
        }
        lcd_SendChar(sign);
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        struct variavel *v = Selecao.vars[i].v;
        if (v->changed != 0 || tela_atual != SELECAO)
        {
            uint8_t line = Selecao.vars[i].line;
            uint8_t column = Selecao.vars[i].column;
            if (line == 0)
                line = 0x80;
            else
                line = 0xC0;
            lcd_SendCmd(line + column);
            lcd_Write(v->repr);
            v->changed = 0;
        }
    }

    *(selecaoOpts *)Selecao.opts = sel;
    tela_atual = SELECAO;
}
/****************************************************TELA DE EDICAO */
static edicaoOpts eOpts = ED_TERR;
const static struct tela Edicao = {
    .vars = {
        // .|0123456789012345|
        // 0|   Temperatura  |
        // 1|      50°C      |
        {.v = &vTargetTemp, .line = 1, .column = 6},
        // .|0123456789012345|
        // 0|      Tempo     |
        // 1|      1:22h     |
        {.v = &vTempo, .line = 1, .column = 6},
        // .|0123456789012345|
        // 0|    Intervalo   |
        // 1|      4min      |
        {.v = &vIntervalo, .line = 1, .column = 6},
        // .|0123456789012345|
        // 0|   Velocidade   |
        // 1|      100%      |
        {.v = &vVelocidade, .line = 1, .column = 6},
        // .|0123456789012345|
        // 0|   Temperatura  |
        // 1|    Err:0xFF    |
        {.v = &vError, .line = 1, .column = 4},
    },
    .opts = (void *)&eOpts,
};
void telaEdicao(edicaoOpts opts)
{
    uint8_t line = Edicao.vars[opts].line == 0 ? 0x80 : 0xC0;
    uint8_t column = Edicao.vars[opts].column;
    struct variavel *v = Edicao.vars[opts].v;

    if (tela_atual != EDICAO || opts != *(edicaoOpts *)Edicao.opts)
    {
        lcd_SendCmd(0x01);
        switch (opts)
        {
        case ED_TTEMP:
            lcd_Write("   Temperatura  ");
            break;
        case ED_TEMPO:
            lcd_Write("      Tempo     ");
            break;
        case ED_INT:
            lcd_Write("    Intervalo   ");
            break;
        case ED_VEL:
            lcd_Write("   Velocidade   ");
            break;
        case ED_TERR:
            lcd_Write("   Temperatura  ");
            break;
        }
        lcd_SendCmd(line + column);
        lcd_Write(v->repr);
        v->changed = 0;
    }
    if (v->changed)
    {
        lcd_SendCmd(line + column);
        lcd_Write(v->repr);
        v->changed = 0;
    }
    tela_atual = EDICAO;
    *(edicaoOpts *)Edicao.opts = opts;
}
/********Aquecendo*/
/*
.|0123456789012345|
0|     aguarde    |
1|20°C/60°C 12,0mA|
*/
const static struct tela Aquecendo = {
    .vars = {
        {.v = &vTemperatura, .line = 1, .column = 0},
        {.v = &vTargetTemp, .line = 1, .column = 5},
        {.v = &vCorrente, .line = 1, .column = 10},
    },
    .opts = NULL,
};
void telaAquecendo(void)
{
    if (tela_atual != AQUECENDO)
    {
        lcd_SendCmd(0x01);
        lcd_SendCmd(0x80 + 5);
        lcd_Write("Aguarde");
        lcd_SendCmd(0xC0 + 4);
        lcd_SendChar('/');
    }
    for (uint8_t i = 0; i < 3; i++)
    {
        struct variavel *v = Aquecendo.vars[i].v;
        if (v->changed || tela_atual != AQUECENDO)
        {
            uint8_t line = Aquecendo.vars[i].line ? 0xC0 : 0x80;
            uint8_t column = Aquecendo.vars[i].column;
            lcd_SendCmd(line + column);
            lcd_Write(v->repr);
            v->changed = 0;
        }
    }
    tela_atual = AQUECENDO;
}

/********Funcionamento*/
/*
.|0123456789012345|
0|T:20°C i:s12,1mA|
1|t:1:10h   V:050%|
*/
const static struct tela Funcionamento = {
    .vars = {
        {.v = &vTemperatura, .line = 0, .column = 2},
        {.v = &vCorrente, .line = 0, .column = 9},
        {.v = &vTempo, .line = 1, .column = 2},
        {.v = &vVelocidade, .line = 1, .column = 12},
    },
    .opts = NULL,
};
void telaFuncionamento(void)
{
    if (tela_atual != FUNCIONAMENTO)
    {
        lcd_SendCmd(0x01);
        lcd_Write("T:TTGU i:sIIviuU");
        lcd_SendCmd(0xC0);
        lcd_Write("t:tPttU   V:VVVP");
    }
    for (uint8_t i = 0; i < 4; i++)
    {
        struct variavel *v = Funcionamento.vars[i].v;
        if (v->changed != 0 || tela_atual != FUNCIONAMENTO)
        {
            uint8_t line = Funcionamento.vars[i].line;
            uint8_t column = Funcionamento.vars[i].column;
            if (line == 0)
                line = 0x80;
            else
                line = 0xC0;
            lcd_SendCmd(line + column);
            lcd_Write(v->repr);
            v->changed = 0;
        }
    }
    tela_atual = FUNCIONAMENTO;
}