#include "main_fsm.h"
#include "main.h"
#include "lcd.h"
#include "variables.h"
#include "interface.h"

typedef enum
{
    MAIN_STATE_RESUMO,
    MAIN_STATE_SEL_TEMPERATURA,
    MAIN_STATE_SEL_TEMPO,
    MAIN_STATE_SEL_INTERVALO,
    MAIN_STATE_SEL_VELOCIDADE,
    MAIN_STATE_AJUSTE_TEMPERATURA,
    MAIN_STATE_AJUSTE_TEMPO,
    MAIN_STATE_AJUSTE_INTERVALO,
    MAIN_STATE_AJUSTE_VELOCIDADE,
    MAIN_STATE_AQUECIMENTO,
    MAIN_STATE_ERROR,
    MAIN_STATE_ERROR_BT,
    MAIN_STATE_ERROR_TERMO,
    MAIN_STATE_LENGTH,
} main_stateType;
static main_stateType main_curr_state = MAIN_STATE_RESUMO;
static uint8_t main_state_changed = 1;

static uint8_t main_state_Resumo(void);
static uint8_t main_state_SelTemperatura(void);
static uint8_t main_state_SelTempo(void);
static uint8_t main_state_SelIntervalo(void);
static uint8_t main_state_SelVelocidade(void);
static uint8_t main_state_AjusteTemperatura(void);
static uint8_t main_state_AjusteTempo(void);
static uint8_t main_state_AjusteIntervalo(void);
static uint8_t main_state_AjusteVelocidade(void);
static uint8_t main_state_Aquecimento(void);
static uint8_t main_state_Error(void);
static uint8_t main_state_ErrorBt(void);
static uint8_t main_state_ErrorTermo(void);

static FSM_applyType main_state_Fns[MAIN_STATE_LENGTH] = {
    main_state_Resumo,
    main_state_SelTemperatura,
    main_state_SelTempo,
    main_state_SelIntervalo,
    main_state_SelVelocidade,
    main_state_AjusteTemperatura,
    main_state_AjusteTempo,
    main_state_AjusteIntervalo,
    main_state_AjusteVelocidade,
    main_state_Aquecimento,
    main_state_Error,
    main_state_ErrorBt,
    main_state_ErrorTermo,
};

uint8_t main_state_Apply(void)
{
    uint8_t fsm_cw = main_state_Fns[main_curr_state]();
    main_state_changed = 0;
    return fsm_cw;
}

void main_state_Next(void)
{
    main_state_changed = 0;

    switch (main_curr_state)
    {
    case MAIN_STATE_RESUMO:
        if (*ev_flags & EV_BT_DOWN)
        {
            main_curr_state = MAIN_STATE_SEL_TEMPERATURA;
            main_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_UP)
        {
            main_curr_state = MAIN_STATE_SEL_VELOCIDADE;
            main_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_OK)
        {
            main_curr_state = MAIN_STATE_AQUECIMENTO;
            main_state_changed = 1;
        }
        break;
    case MAIN_STATE_SEL_TEMPERATURA:
        if (*ev_flags & EV_BT_DOWN)
        {
            main_curr_state = MAIN_STATE_SEL_TEMPO;
            main_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_UP)
        {
            main_curr_state = MAIN_STATE_RESUMO;
            main_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_OK)
        {
            main_curr_state = MAIN_STATE_AJUSTE_TEMPERATURA;
            main_state_changed = 1;
        }
        break;
    case MAIN_STATE_SEL_TEMPO:
        if (*ev_flags & EV_BT_DOWN)
        {
            main_curr_state = MAIN_STATE_SEL_INTERVALO;
            main_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_UP)
        {
            main_curr_state = MAIN_STATE_SEL_TEMPERATURA;
            main_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_OK)
        {
            main_curr_state = MAIN_STATE_AJUSTE_TEMPO;
            main_state_changed = 1;
        }
        break;
    case MAIN_STATE_SEL_INTERVALO:
        if (*ev_flags & EV_BT_DOWN)
        {
            main_curr_state = MAIN_STATE_SEL_VELOCIDADE;
            main_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_UP)
        {
            main_curr_state = MAIN_STATE_SEL_TEMPO;
            main_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_OK)
        {
            main_curr_state = MAIN_STATE_AJUSTE_INTERVALO;
            main_state_changed = 1;
        }
        break;
    case MAIN_STATE_SEL_VELOCIDADE:
        if (*ev_flags & EV_BT_DOWN)
        {
            main_curr_state = MAIN_STATE_RESUMO;
            main_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_UP)
        {
            main_curr_state = MAIN_STATE_SEL_INTERVALO;
            main_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_OK)
        {
            main_curr_state = MAIN_STATE_AJUSTE_VELOCIDADE;
            main_state_changed = 1;
        }
        break;
    case MAIN_STATE_AJUSTE_TEMPERATURA:
        if (*ev_flags & EV_BT_OK)
        {
            main_curr_state = MAIN_STATE_SEL_TEMPERATURA;
            main_state_changed = 1;
        }
        break;
    case MAIN_STATE_AJUSTE_TEMPO:
        if (*ev_flags & EV_BT_OK)
        {
            main_curr_state = MAIN_STATE_SEL_TEMPO;
            main_state_changed = 1;
        }
        break;
    case MAIN_STATE_AJUSTE_INTERVALO:
        if (*ev_flags & EV_BT_OK)
        {
            main_curr_state = MAIN_STATE_SEL_INTERVALO;
            main_state_changed = 1;
        }
        break;
    case MAIN_STATE_AJUSTE_VELOCIDADE:
        if (*ev_flags & EV_BT_OK)
        {
            main_curr_state = MAIN_STATE_SEL_VELOCIDADE;
            main_state_changed = 1;
        }
        break;
    case MAIN_STATE_AQUECIMENTO:
        if (*ev_flags & EV_BT_OK)
        {
            main_curr_state = MAIN_STATE_RESUMO;
            main_state_changed = 1;
        }
        break;
    case MAIN_STATE_ERROR_BT:
        return;
    case MAIN_STATE_ERROR_TERMO:
        return;
    case MAIN_STATE_LENGTH:
        main_state_changed = 1;
    case MAIN_STATE_ERROR:
        main_curr_state = MAIN_STATE_ERROR;
        return;
    }

    if (*ev_flags & EV_BT_ERROR)
    {
        main_curr_state = MAIN_STATE_ERROR_BT;
        main_state_changed = 1;
        return;
    }
    if (*ev_flags & EV_TERMO_ERROR)
    {
        main_curr_state = MAIN_STATE_ERROR_TERMO;
        main_state_changed = 1;
        return;
    }
}

static uint8_t main_state_Resumo(void)
{
    if (main_state_changed)
    {
        tela_selecao(0);
    }
    return FSM_EV_WAIT;
}
static uint8_t main_state_SelTemperatura(void)
{
    if (main_state_changed)
    {
        tela_selecao(1);
    }
    return FSM_EV_WAIT;
}
static uint8_t main_state_SelTempo(void)
{
    if (main_state_changed)
    {
        tela_selecao(2);
    }
    return FSM_EV_WAIT;
}
static uint8_t main_state_SelIntervalo(void)
{
    if (main_state_changed)
    {
        tela_selecao(3);
    }
    return FSM_EV_WAIT;
}
static uint8_t main_state_SelVelocidade(void)
{
    if (main_state_changed)
    {
        tela_selecao(4);
    }
    return FSM_EV_WAIT;
}

static uint8_t main_state_AjusteTemperatura(void)
{
    if (main_state_changed)
    {
        tela_edicao(0);
    }
    else if (*ev_flags & EV_BT_DOWN)
    {
        var_temperatura_dec();
        tela_edicao(0);
    }
    else if (*ev_flags & EV_BT_UP)
    {
        var_temperatura_inc();
        tela_edicao(0);
    }
    return FSM_EV_WAIT;
}

static uint8_t main_state_AjusteTempo(void)
{
    if (main_state_changed)
    {
        tela_edicao(1);
    }
    else if (*ev_flags & EV_BT_DOWN)
    {
        var_tempo_dec();
        tela_edicao(1);
    }
    else if (*ev_flags & EV_BT_UP)
    {
        var_tempo_inc();
        tela_edicao(1);
    }

    return FSM_EV_WAIT;
}
static uint8_t main_state_AjusteIntervalo(void)
{
    if (main_state_changed)
    {
        tela_edicao(2);
    }
    else if (*ev_flags & EV_BT_DOWN)
    {
        var_intervalo_dec();
        tela_edicao(2);
    }
    else if (*ev_flags & EV_BT_UP)
    {
        var_intervalo_inc();
        tela_edicao(2);
    }

    return FSM_EV_WAIT;
}
static uint8_t main_state_AjusteVelocidade(void)
{
    if (main_state_changed)
    {
        tela_edicao(3);
    }
    else if (*ev_flags & EV_BT_DOWN)
    {
        var_velocidade_dec();
        tela_edicao(3);
    }
    else if (*ev_flags & EV_BT_UP)
    {
        var_velocidade_inc();
        tela_edicao(3);
    }
    var_motor_on();
    return FSM_EV_WAIT;
}

static uint8_t main_state_Aquecimento(void)
{
    if (main_state_changed)
    {
        tela_aquecendo();
    }
    if (*ev_flags & EV_TERMO_NEW)
    {
        tela_aquecendo();
    }
    return FSM_EV_WAIT;
}

static uint8_t main_state_Error(void)
{
    if (main_state_changed)
    {
        tela_erro("ERR_MAIN", 0x01);
    }
    return FSM_EV_WAIT;
}
static uint8_t main_state_ErrorBt(void)
{
    if (main_state_changed)
    {
        tela_erro("ERR_BT", 0x01);
    }
    return FSM_EV_WAIT;
}
static uint8_t main_state_ErrorTermo(void)
{
    if (main_state_changed)
    {
        tela_erro("ERR_TERMO", termo_error);
        main_timer = 1000;
    }
    if (*ev_flags & EV_MAIN_TIMER)
    {
        uint32_t map = valor_adc;
        map = (map * 2400) / 1023;
        uint16_t inteiro = (uint16_t)(map / 100);
        uint16_t decimal = (uint16_t)(map % 100);
        lcd_SendCmd(0x01);
        char tst[17];
        snprintf(tst, 17, "%d,%02d", inteiro, decimal);
        lcd_Write(tst);
        main_timer = 1000;
    }
    return FSM_EV_WAIT;
}
/*
0b0000.0000.0000.0000.0000.0010.1010.1010
0b0000.0000.0011.1111.1111.1111.1111.1111
0b0000.0000.0000.0000.1111.1111.1111.1111

*/