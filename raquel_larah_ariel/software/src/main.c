#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "main.h"
#include "buttons.h"
#include "termometro.h"
#include "lcd.h"
#include "main_fsm.h"
#include "variables.h"

#ifdef LOG
#include <stdio.h>
#include "usart.h"
#endif // LOG

static volatile uint8_t ev_flags_write = 0;
static volatile uint8_t ev_flags_read = 0;
const volatile uint8_t *const ev_flags = &ev_flags_read;

void ev_write(uint8_t mask)
{
    ev_flags_write |= mask;
}

static void ev_update()
{
    cli();
    ev_flags_read = ev_flags_write;
    ev_flags_write = 0;
    sei();
}

/************************************/
/* Variaveis utilizadas por termo.c */
int16_t termo_temperatura = 0;
uint8_t termo_error = 0;
volatile uint16_t termo_timer = 0;
/************************************/

/**************************************/
/* Variaveis utilizadas por buttons.c */
volatile uint16_t buttons_timer = 0;
/**************************************/

int main(void)
{
    lcd_Init();
    /**** INICIALIZAÇÔES ****/
    // Mode 4 - TCC - OCRA
    TCCR1A = 0;
    TCCR1B = (1 << CS10) | (1 << WGM12);
    // 16Mhz/1/16000 = 1kHz -> 1ms
    OCR1A = 16000;
    TIMSK1 = (1 << OCIE1A);

    buttons_Init();

    var_Init();
#ifdef LOG
    FILE *usart = USART_Init(B9600);
#endif // LOG

    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    while (1)
    {
        uint8_t fsm_continue = 0;
        fsm_continue |= termo_state_Apply();
        fsm_continue |= buttons_state_Apply();
        fsm_continue |= main_state_Apply();
        // fsm_continue |= FSM1_state_Apply();
        // fsm_continue |= FSM2_state_Apply();

        /* Se todas as maquinas estão esperando por um evento desliga a CPU até que um evento aconteça */
        cli();
        while (fsm_continue == FSM_EV_WAIT && ev_flags_write == 0)
        {
            sei();
            sleep_mode();
        }

        ev_update();

        termo_state_Next();
        buttons_state_Next();
        main_state_Next();
        // FSM1_state_Next();
        // FSM2_state_Next();
    }
}

ISR(TIMER1_COMPA_vect)
{
    /* essa interrupção acontece a cada FSM_BASE_TIME_MS milisegundos*/
    /* se o contador for maior que zero decrementa o contador da FSM */
    /* se o contador chegou em zero emite o evento de timer da FSM
    if (FSM1_timer > 0) {
        FSM1_timer -= 1;
        if (FSM1_timer == 0) {
            ev_write(EV_FSM1_TIMER);
        }
    }
    if (FSM2_timer > 0) {
        FSM2_timer -= 1;
        if (FSM2_timer == 0) {
            ev_write(EV_FSM2_TIMER);
        }
    }
    */
    if (termo_timer > 0)
    {
        termo_timer -= 1;
        if (termo_timer == 0)
        {
            ev_write(EV_TERMO_TIMER);
        }
    }
    if (buttons_timer > 0)
    {
        buttons_timer -= 1;
        if (buttons_timer == 0)
        {
            ev_write(EV_BT_TIMER);
        }
    }
}
