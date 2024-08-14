#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "main.h"
#include "buttons.h"

#define BUTTONS_OK() ((BUTTONS_OK_PINR & BUTTONS_OK_MASK))
#define BUTTONS_DW() ((BUTTONS_DW_PINR & BUTTONS_DW_MASK))
#define BUTTONS_UP() ((BUTTONS_UP_PINR & BUTTONS_UP_MASK))

/***********************************************/
/* Definir em algum source file (ex.: main.c) */
extern volatile uint16_t buttons_timer;
/***********************************************/

typedef enum
{
    BUTTONS_STATE_IDLE,
    BUTTONS_STATE_DELAY_PRESSED,
    BUTTONS_STATE_OK,
    BUTTONS_STATE_DOWN,
    BUTTONS_STATE_UP,
    BUTTONS_STATE_DOWN_BURST,
    BUTTONS_STATE_UP_BURST,
    BUTTONS_STATE_DELAY_RELEASED,
    BUTTONS_STATE_ERROR,
    BUTTONS_STATE_LENGTH,
} buttons_stateType;
static buttons_stateType buttons_curr_state = BUTTONS_STATE_IDLE;
static uint8_t buttons_state_changed = 1;

static uint8_t buttons_state_Idle(void);
static uint8_t buttons_state_DelayPressed(void);
static uint8_t buttons_state_Ok(void);
static uint8_t buttons_state_Down(void);
static uint8_t buttons_state_Up(void);
static uint8_t buttons_state_DownBurst(void);
static uint8_t buttons_state_UpBurst(void);
static uint8_t buttons_state_DelayReleased(void);
static uint8_t buttons_state_Error(void);

static FSM_applyType buttons_state_Fns[BUTTONS_STATE_LENGTH] = {
    buttons_state_Idle,
    buttons_state_DelayPressed,
    buttons_state_Ok,
    buttons_state_Down,
    buttons_state_Up,
    buttons_state_DownBurst,
    buttons_state_UpBurst,
    buttons_state_DelayReleased,
    buttons_state_Error,
};

static void
buttons_PCINTEnable();
static void buttons_PCINTDisable();
static void buttons_isr();

uint8_t buttons_state_Apply()
{
    uint8_t fsm_continue_wait = buttons_state_Fns[buttons_curr_state]();
    buttons_state_changed = 0;
    return fsm_continue_wait;
}

void buttons_state_Next()
{
    buttons_state_changed = 0;
    switch (buttons_curr_state)
    {
    case BUTTONS_STATE_IDLE:
        if (*ev_flags & EV_BT_EXT)
        {
            buttons_curr_state = BUTTONS_STATE_DELAY_PRESSED;
            buttons_state_changed = 1;
        }
        break;
    case BUTTONS_STATE_DELAY_PRESSED:
        if (*ev_flags & EV_BT_TIMER)
        {
            if (BUTTONS_OK() == 0)
            {
                buttons_curr_state = BUTTONS_STATE_OK;
            }
            else if (BUTTONS_DW() == 0)
            {
                buttons_curr_state = BUTTONS_STATE_DOWN;
            }
            else if (BUTTONS_UP() == 0)
            {
                buttons_curr_state = BUTTONS_STATE_UP;
            }
            else
            {
                buttons_curr_state = BUTTONS_STATE_DELAY_RELEASED;
            }
            buttons_state_changed = 1;
        }
        break;
    case BUTTONS_STATE_OK:
        if (BUTTONS_UP())
        {
            buttons_curr_state = BUTTONS_STATE_DELAY_RELEASED;
            buttons_state_changed = 1;
        }
        break;
    case BUTTONS_STATE_DOWN:
    case BUTTONS_STATE_DOWN_BURST:
        if (BUTTONS_DW())
        {
            buttons_curr_state = BUTTONS_STATE_DELAY_RELEASED;
            buttons_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_TIMER)
        {
            buttons_curr_state = BUTTONS_STATE_DOWN_BURST;
            buttons_state_changed = 1;
        }
        break;
    case BUTTONS_STATE_UP:
    case BUTTONS_STATE_UP_BURST:
        if (BUTTONS_UP())
        {
            buttons_curr_state = BUTTONS_STATE_DELAY_RELEASED;
            buttons_state_changed = 1;
        }
        else if (*ev_flags & EV_BT_TIMER)
        {
            buttons_curr_state = BUTTONS_STATE_UP_BURST;
            buttons_state_changed = 1;
        }
        break;
    case BUTTONS_STATE_DELAY_RELEASED:
        if (*ev_flags & EV_BT_TIMER)
        {
            buttons_curr_state = BUTTONS_STATE_IDLE;
            buttons_state_changed = 1;
        }
        break;
    default:
        buttons_curr_state = BUTTONS_STATE_ERROR;
        buttons_state_changed = 1;
        break;
    }
}

static uint8_t buttons_state_Idle(void)
{
    buttons_PCINTEnable();
    return FSM_EV_WAIT;
}

static uint8_t buttons_state_DelayPressed(void)
{
    if (buttons_state_changed)
    {
        buttons_PCINTDisable();
        buttons_timer = 80 / FSM_BASE_TIME_MS;
    }
    return FSM_EV_WAIT;
}

static uint8_t buttons_state_Ok(void)
{
    if (buttons_state_changed)
    {
        ev_write(EV_BT_OK);
    }
    return FSM_CONTINUE;
}

static uint8_t buttons_state_Down(void)
{
    if (buttons_state_changed)
    {
        ev_write(EV_BT_DOWN);
        buttons_timer = BUTTONS_HOLD_TIME_MS / FSM_BASE_TIME_MS;
    }
    return FSM_CONTINUE;
}

static uint8_t buttons_state_Up(void)
{
    if (buttons_state_changed)
    {
        ev_write(EV_BT_UP);
        buttons_timer = BUTTONS_HOLD_TIME_MS / FSM_BASE_TIME_MS;
    }
    return FSM_CONTINUE;
}

static uint8_t buttons_state_DownBurst(void)
{
    if (buttons_state_changed)
    {
        ev_write(EV_BT_DOWN);
        buttons_timer = BUTTONS_BURST_TIME_MS / FSM_BASE_TIME_MS;
    }
    return FSM_CONTINUE;
}

static uint8_t buttons_state_UpBurst(void)
{
    if (buttons_state_changed)
    {
        ev_write(EV_BT_UP);
        buttons_timer = BUTTONS_BURST_TIME_MS / FSM_BASE_TIME_MS;
    }
    return FSM_CONTINUE;
}

static uint8_t buttons_state_DelayReleased(void)
{
    if (buttons_state_changed)
    {
        buttons_timer = 80 / FSM_BASE_TIME_MS;
    }
    return FSM_EV_WAIT;
}

static uint8_t buttons_state_Error(void)
{
    ev_write(EV_BT_ERROR);
    return FSM_CONTINUE;
}

void buttons_Init()
{
    BUTTONS_OK_DDR &= ~BUTTONS_OK_MASK;
    BUTTONS_OK_PORT |= BUTTONS_OK_MASK;

    BUTTONS_DW_DDR &= ~BUTTONS_DW_MASK;
    BUTTONS_DW_PORT |= BUTTONS_DW_MASK;

    BUTTONS_UP_DDR &= ~BUTTONS_UP_MASK;
    BUTTONS_UP_PORT |= BUTTONS_UP_MASK;

    PCICR |= (BUTTONS_UP_PCICR_MASK) | (BUTTONS_DW_PCICR_MASK) | (BUTTONS_OK_PCICR_MASK);

    buttons_PCINTEnable();
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
    uint8_t mask = BUTTONS_OK() & BUTTONS_DW() & BUTTONS_UP();
    if (mask == 0)
    {
        ev_write(EV_BT_EXT);
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
