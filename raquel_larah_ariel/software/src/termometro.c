#include <stdint.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "termometro.h"
#include "main.h"
#include "ds18b20/ds18b20.h"

/***********************************************/
/* Definir em algum source file (ex.: main.c) */
extern int16_t termo_temperatura;
extern uint8_t termo_error;
extern volatile uint16_t termo_timer;
/***********************************************/

typedef enum
{
    TERMO_STATE_START_CONV,
    TERMO_STATE_READ,
    TERMO_STATE_ERROR,
    TERMO_STATE_LENGTH,
} termo_stateType;
static termo_stateType termo_curr_state = TERMO_STATE_START_CONV;
static uint8_t termo_state_changed = 1;

static uint8_t termo_state_StartConv();
static uint8_t termo_state_Read();
static uint8_t termo_state_Error();

static FSM_applyType termo_state_Fns[TERMO_STATE_LENGTH] = {
    termo_state_StartConv,
    termo_state_Read,
    termo_state_Error,
};

uint8_t termo_state_Apply()
{
    return termo_state_Fns[termo_curr_state]();
}

void termo_state_Next()
{
    if (*ev_flags & EV_TERMO_ERROR)
    {
        termo_curr_state = TERMO_STATE_ERROR;
        termo_state_changed = 1;
        return;
    }

    if ((*ev_flags & EV_TERMO_TIMER) == 0)
    {
        return;
    }

    switch (termo_curr_state)
    {
    case TERMO_STATE_START_CONV:
        termo_curr_state = TERMO_STATE_READ;
        break;
    case TERMO_STATE_READ:
        termo_curr_state = TERMO_STATE_START_CONV;
        break;
    default:
        termo_curr_state = TERMO_STATE_ERROR;
        termo_error = TERMO_ERROR_INVALID_STATE;
    }

    termo_state_changed = 1;
}

static uint8_t termo_state_StartConv()
{
    /* Na entrada do estado inicia a conversão*/
    if (termo_state_changed)
    {
        termo_state_changed = 0;
        termo_error = ds18b20convert(&TERMO_PORT, &TERMO_DDR, &TERMO_PORTIN, TERMO_MASK, NULL);
        /*
        se ocorreu um erro emite o evento e informa que a logica de próximo estado deve ser
        chamada imediatamente, em teoria esse evento já causaria a execução da logica de
        proximo estado, mas assim fica mais evidente o que deve acontecer. Na logica de próximo
        estado a maquina vai ser direcionada ao estado de erro
        */
        if (termo_error)
        {
            ev_write(EV_TERMO_ERROR);
            return FSM_CONTINUE;
        }
        /*
        Se não ocorreram erros inicia um contador de 1 segundo
        */
        termo_timer = 1000 / FSM_BASE_TIME_MS;
    }
    /*
    esse estado possui apenas logica de entrada, após um segundo a logica de próximo estado
    direciona a FSM para o estado de leitura do valor
    */
    return FSM_EV_WAIT;
}

static uint8_t termo_state_Read()
{
    if (termo_state_changed)
    {
        termo_state_changed = 0;
        termo_error = ds18b20read(&PORTB, &DDRB, &PINB, (1 << PINB0), NULL, &(termo_temperatura));
        termo_temperatura /= 16;
		ev_write(EV_TERMO_NEW);
        /*
        se ocorreu um erro emite o evento e informa que a logica de próximo estado deve ser
        chamada imediatamente, em teoria esse evento já causaria a execução da logica de
        proximo estado, mas assim fica mais evidente o que deve acontecer. Na logica de próximo
        estado a maquina vai ser direcionada ao estado de erro
        */
        if (termo_error)
        {
            ev_write(EV_TERMO_ERROR);
            return FSM_CONTINUE;
        }
        /*
        Se não ocorreram erros inicia um contador de 1 segundo
        */
        termo_timer = 1000 / FSM_BASE_TIME_MS;
    }
    /*
    esse estado possui apenas logica de entrada, após trinta segundos a logica de próximo estado
    direciona a FSM para o estado de início de conversão
    */
    return FSM_EV_WAIT;
}

static uint8_t termo_state_Error()
{
    /*
    emite o evento de erro e indica que a o loop principal deve continuar ocorrendo
    independentemente dos eventos ocorridos, a ideia é "forçar" as outras máquinas
    a tratarem o erro
    */
   termo_error = 9;
    ev_write(EV_TERMO_ERROR);
    return FSM_CONTINUE;
}