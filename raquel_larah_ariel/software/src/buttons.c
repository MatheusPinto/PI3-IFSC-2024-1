#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "main.h"
#include "buttons.h"

#define BUTTONS_OK() ((BUTTONS_OK_PINR & BUTTONS_OK_MASK))
#define BUTTONS_DW() ((BUTTONS_DW_PINR & BUTTONS_DW_MASK))
#define BUTTONS_UP() ((BUTTONS_UP_PINR & BUTTONS_UP_MASK))

static void buttons_PCINTEnable();
static void buttons_PCINTDisable();

static enum {
    DEBOUNCING,
    HOLD,
} mode;

static volatile buttons_result status;
static volatile uint16_t timer;

buttons_result buttons_read()
{
    switch (status)
    {
    case BUTTONS_UNINIT:
        buttons_PCINTDisable();
        break;
    case BUTTONS_NONE:
        mode = DEBOUNCING;
        if (timer == 0)
            buttons_PCINTEnable();
        break;
    case BUTTONS_DOWN:
        if (timer != 0)
            return BUTTONS_NONE;
        if (BUTTONS_DW())
        {
            status = BUTTONS_NONE;
            timer = 80;
        }
        else if (mode == DEBOUNCING)
        {
            timer = BUTTONS_HOLD_TIME_MS / FSM_BASE_TIME_MS;
            mode = HOLD;
        }
        else if (mode == HOLD)
        {
            timer = BUTTONS_BURST_TIME_MS / FSM_BASE_TIME_MS;
        }
        break;
    case BUTTONS_UP:
        if (timer != 0)
            return BUTTONS_NONE;
        if (BUTTONS_UP())
        {
            status = BUTTONS_NONE;
            timer = 80;
        }
        else if (mode == DEBOUNCING)
        {
            timer = BUTTONS_HOLD_TIME_MS / FSM_BASE_TIME_MS;
            mode = HOLD;
        }
        else if (mode == HOLD)
        {
            timer = BUTTONS_BURST_TIME_MS / FSM_BASE_TIME_MS;
        }
        break;
    case BUTTONS_OK:
        if (timer != 0)
            return BUTTONS_NONE;
        status = BUTTONS_NONE;
        timer = 80;
        return BUTTONS_OK;
        break;
    }
    return status;
}

void buttons_init()
{
    BUTTONS_OK_DDR &= ~BUTTONS_OK_MASK;
    BUTTONS_OK_PORT |= BUTTONS_OK_MASK;

    BUTTONS_DW_DDR &= ~BUTTONS_DW_MASK;
    BUTTONS_DW_PORT |= BUTTONS_DW_MASK;

    BUTTONS_UP_DDR &= ~BUTTONS_UP_MASK;
    BUTTONS_UP_PORT |= BUTTONS_UP_MASK;

    PCICR |= (BUTTONS_UP_PCICR_MASK) | (BUTTONS_DW_PCICR_MASK) | (BUTTONS_OK_PCICR_MASK);

    buttons_PCINTEnable();
    status = BUTTONS_NONE;
}

void buttons_update()
{
    if (timer > 0)
    {
        timer -= 1;
    }
}

static void buttons_PCINTEnable()
{
    BUTTONS_OK_PCMSK |= BUTTONS_OK_MASK;
    BUTTONS_DW_PCMSK |= BUTTONS_DW_MASK;
    BUTTONS_UP_PCMSK |= BUTTONS_UP_MASK;
}

static void buttons_PCINTDisable()
{
    BUTTONS_OK_PCMSK &= BUTTONS_OK_MASK;
    BUTTONS_DW_PCMSK &= BUTTONS_DW_MASK;
    BUTTONS_UP_PCMSK &= BUTTONS_UP_MASK;
}

static void buttons_isr()
{
    if (!BUTTONS_OK())
    {
        buttons_PCINTDisable();
        status = BUTTONS_OK;
        timer = 80;
    }
    else if (!BUTTONS_DW())
    {
        buttons_PCINTDisable();
        status = BUTTONS_DOWN;
        timer = 80;
    }
    else if (!BUTTONS_UP())
    {
        buttons_PCINTDisable();
        status = BUTTONS_UP;
        timer = 80;
    }
}

ISR(PCINT0_vect)
{
    buttons_isr();
}

ISR(PCINT1_vect)
{
    buttons_isr();
}

ISR(PCINT2_vect)
{
    buttons_isr();
}
