#include <avr/io.h>
#include <stdint.h>

#include "buttons.h"
#include "main.h"

#define PRESSED (1 << 0)
#define HOLD (1 << 1)
#define DOWN (1 << 0)
#define UP (1 << 1)
#define OK (1 << 2)

#define HOLD_DOWN (1 << 0)
#define HOLD_UP (1 << 1)
#define HOLD_OK (1 << 2)
#define PRESSED_DOWN (1 << 3)
#define PRESSED_UP (1 << 4)
#define PRESSED_OK (1 << 5)
#define BURST_DOWN (1 << 6)
#define BURST_UP (1 << 7)
#define BURST_OK (0) // never burst

uint8_t status;

void buttonsInit()
{
	BUTTONS_OK_DDR &= ~BUTTONS_OK_MASK;
	BUTTONS_OK_PORT |= BUTTONS_OK_MASK;

	BUTTONS_DOWN_DDR &= ~BUTTONS_DOWN_MASK;
	BUTTONS_DOWN_PORT |= BUTTONS_DOWN_MASK;

	BUTTONS_UP_DDR &= ~BUTTONS_UP_MASK;
	BUTTONS_UP_PORT |= BUTTONS_UP_MASK;

	status = 0;
}

void buttonsOkUpdate()
{
	static uint16_t count = 0;
	if (BUTTONS_OK_PINR & BUTTONS_OK_MASK)
	{
		status &= ~(HOLD_OK | PRESSED_OK);
		count = 80 / FSM_BASE_TIME_MS;
		return;
	}

	if (count > 0)
	{
		status &= ~PRESSED_OK;
		count -= 1;
		return;
	}

	if ((status & HOLD_OK) == 0)
	{
		status |= HOLD_OK | PRESSED_OK;
		count = BUTTONS_HOLD_TIME_MS / FSM_BASE_TIME_MS;
		return;
	}

	if (status & BURST_OK)
	{
		status |= PRESSED_OK;
		count = BUTTONS_BURST_TIME_MS / FSM_BASE_TIME_MS;
	}
}

void buttonsDownUpdate()
{
	static uint16_t count = 0;
	if (BUTTONS_DOWN_PINR & BUTTONS_DOWN_MASK)
	{
		status &= ~(HOLD_DOWN | PRESSED_DOWN);
		count = 80 / FSM_BASE_TIME_MS;
		return;
	}

	if (count > 0)
	{
		status &= ~PRESSED_DOWN;
		count -= 1;
		return;
	}

	if ((status & HOLD_DOWN) == 0)
	{
		status |= HOLD_DOWN | PRESSED_DOWN;
		count = BUTTONS_HOLD_TIME_MS / FSM_BASE_TIME_MS;
		return;
	}

	if (status & BURST_DOWN)
	{
		status |= PRESSED_DOWN;
		count = BUTTONS_BURST_TIME_MS / FSM_BASE_TIME_MS;
	}
}

void buttonsUpUpdate()
{
	static uint16_t count = 0;
	if (BUTTONS_UP_PINR & BUTTONS_UP_MASK)
	{
		status &= ~(HOLD_UP | PRESSED_UP);
		count = 80 / FSM_BASE_TIME_MS;
		return;
	}

	if (count > 0)
	{
		status &= ~PRESSED_UP;
		count -= 1;
		return;
	}

	if ((status & HOLD_UP) == 0)
	{
		status |= HOLD_UP | PRESSED_UP;
		count = BUTTONS_HOLD_TIME_MS / FSM_BASE_TIME_MS;
		return;
	}

	if (status & BURST_UP)
	{
		status |= PRESSED_UP;
		count = BUTTONS_BURST_TIME_MS / FSM_BASE_TIME_MS;
	}
}

void buttonsUpdate()
{
	buttonsOkUpdate();
	buttonsDownUpdate();
	buttonsUpUpdate();
}

uint8_t buttonsUp()
{
	return status & PRESSED_UP;
}
uint8_t buttonsDown()
{
	return status & PRESSED_DOWN;
}
uint8_t buttonsOk()
{
	return status & PRESSED_OK;
}

void buttonsDownBurst(uint8_t flag)
{
	if (flag)
	{
		status |= BURST_DOWN;
	}
	else
	{
		status &= ~BURST_DOWN;
	}
}
void buttonsUpBurst(uint8_t flag)
{
	if (flag)
	{
		status |= BURST_UP;
	}
	else
	{
		status &= ~BURST_UP;
	}
}
