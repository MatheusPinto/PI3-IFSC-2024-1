#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "lcd.h"
#include "buttons.h"
#include "termometro.h"
#include "main.h"
#include "adc.h"
#include "eletrodos.h"
#include "motor.h"
#include "aquecimento.h"
#include "timing.h"

typedef enum
{
	MS_INIT_OE,
	MS_INIT,
	MS_SELECT_OE,
	MS_SELECT,
	MS_EDIT_OE,
	MS_EDIT,
	MS_PREPARE_OE,
	MS_PREPARE,
	MS_RUN_OE,
	MS_RUN,
	MS_ERROR,
	MAIN_STATE_LENGTH,
} main_stateType;

static main_stateType main_state = MS_INIT_OE;

static void ms_init_oe(void);
static void ms_init(void);
static void ms_select_oe(void);
static void ms_select(void);
static void ms_edit_oe(void);
static void ms_edit(void);
static void ms_prepare_oe(void);
static void ms_prepare(void);
static void ms_run_oe(void);
static void ms_run(void);
static void ms_error(void);

static void (*const states[MAIN_STATE_LENGTH])(void) = {
	ms_init_oe,
	ms_init,
	ms_select_oe,
	ms_select,
	ms_edit_oe,
	ms_edit,
	ms_prepare_oe,
	ms_prepare,
	ms_run_oe,
	ms_run,
	ms_error,
};

struct
{
	int16_t temperature;
	int8_t thermo_result;
	enum
	{
		NONE,
		SYNC,
		THERMO_COM,
		THERMO_CRC,
		THERMO_PULL,
	} error;

	enum
	{
		START,
		TARGET_TEMP,
		INTERVAL,
		TOTAL_TIME,
		VELOCITY,
	} selected;

} vars = {
	.temperature = 0,
	.thermo_result = 0,
	.error = NONE,
	.selected = START,
};

volatile uint16_t main_timer = 0;

int main(void)
{
	/**** INICIALIZAÇÔES ****/
	lcd_Init();
	adcInit();
	buttonsInit();
	eletrodosInit();
	motorInit();
	aquecimentoInit();
	timingInit();

	// Mode 4 - TCC - OCRA
	TCCR1A = 0;
	TCCR1B = (1 << CS11) | (1 << WGM12);
	// 16Mhz/8/20000 = 10ms
	OCR1A = 20000;
	TIMSK1 = (1 << OCIE1A);

	set_sleep_mode(SLEEP_MODE_IDLE);

	main_timer = 0;
	TCNT1 = 0;

	adcEnable();
	sei();
	while (1)
	{
		cli();
		if (main_timer > 1)
		{
			vars.error = SYNC;
			main_state = MS_ERROR;
		}
		while (main_timer != 1)
		{
			sei();
			sleep_mode();
		}
		main_timer = 0;

		buttonsUpdate();
		timingUpdate();
		termo_update();
		vars.thermo_result = termo_read(&vars.temperature);
		aquecimentoUpdate(vars.temperature);
		switch (vars.thermo_result)
		{
		case TERMO_READY:
		case TERMO_UNINIT:
			termo_conv();
			break;
		case TERMO_ERROR_COM:
			vars.error = THERMO_COM;
			main_state = MS_ERROR;
			break;
		case TERMO_ERROR_CRC:
			vars.error = THERMO_CRC;
			main_state = MS_ERROR;
			break;
		case TERMO_ERROR_PULL:
			vars.error = THERMO_PULL;
			main_state = MS_ERROR;
			break;
		}

		states[main_state]();

		switch (main_state)
		{
		case MS_INIT_OE:
			main_state = MS_INIT;
			break;
		case MS_INIT:
			// if (vars.thermo_result == TERMO_READY)
			main_state = MS_SELECT_OE;
			break;
		case MS_SELECT_OE:
			main_state = MS_SELECT;
			break;
		case MS_SELECT:
			if (buttonsOk())
			{
				if (vars.selected == START)
					main_state = MS_PREPARE_OE;
				else
					main_state = MS_EDIT_OE;
			}
			break;
		case MS_EDIT_OE:
			main_state = MS_EDIT;
			break;
		case MS_EDIT:
			if (buttonsOk())
				main_state = MS_SELECT_OE;
			break;
		case MS_PREPARE_OE:
			main_state = MS_PREPARE;
			break;
		case MS_PREPARE:
			if (buttonsOk())
				main_state = MS_SELECT_OE;
			if (aquecimentoStatusGet() == AQUECIMENTO_COOLING && eletrodosStatus() == ELETRODOS_OFF)
				main_state = MS_RUN_OE;
			break;
		case MS_RUN_OE:
			main_state = MS_RUN;
			break;
		case MS_RUN:
			if (buttonsOk())
				main_state = MS_SELECT_OE;
			break;
		case MAIN_STATE_LENGTH:
		case MS_ERROR:
			break;
		}
	}
}

static void safeDefaults()
{
	aquecimentoDisable();
	motorDisable();
	eletrodosOff();
	timingTotalEnable(0);
	timingRefreshEnable(0);
	timingIntervalEnable(0);
}

void ms_init_oe(void)
{
	lcd_SendCmd(0x01);
	lcd_Write("Iniciando");
}

void ms_init(void)
{
	safeDefaults();
}

uint8_t select(uint8_t sel, char *c)
{
	switch (sel)
	{
	case START:
		*c = '>';
		return 0xC0 + 15;
	case TARGET_TEMP:
		*c = '>';
		return 0x80 + 0;
	case INTERVAL:
		*c = '<';
		return 0x80 + 15;
	case TOTAL_TIME:
		*c = '>';
		return 0xC0 + 0;
	case VELOCITY:
		*c = '<';
		return 0xC0 + 15;
	}
	return 0x01;
}

void ms_select_oe(void)
{
	safeDefaults();
	char line[17];

	lcd_SendCmd(0x01);
	snprintf(line, 17, " T:%s  i:%s ", aquecimentoTargetStr(), timingIntervalStr());
	lcd_Write(line);

	lcd_SendCmd(0xC0);
	snprintf(line, 17, " t:%s  V:%s ", timingTotalStr(), motorStr());
	lcd_Write(line);

	char c;
	uint8_t cmd = select(vars.selected, &c);
	lcd_SendCmd(cmd);
	lcd_SendChar(c);
}

void ms_select(void)
{
	if (buttonsDown() == 0 && buttonsUp() == 0)
		return;

	char k;
	uint8_t cmd = select(vars.selected, &k);
	lcd_SendCmd(cmd);
	lcd_SendChar(' ');

	switch (vars.selected)
	{
	case START:
		if (buttonsUp())
			vars.selected = VELOCITY;
		if (buttonsDown())
			vars.selected = TARGET_TEMP;
		break;
	case TARGET_TEMP:
		if (buttonsUp())
			vars.selected = START;
		if (buttonsDown())
			vars.selected = INTERVAL;
		break;
	case INTERVAL:
		if (buttonsUp())
			vars.selected = TARGET_TEMP;
		if (buttonsDown())
			vars.selected = TOTAL_TIME;
		break;
	case TOTAL_TIME:
		if (buttonsUp())
			vars.selected = INTERVAL;
		if (buttonsDown())
			vars.selected = VELOCITY;
		break;
	case VELOCITY:
		if (buttonsUp())
			vars.selected = TOTAL_TIME;
		if (buttonsDown())
			vars.selected = START;
		break;
	}

	cmd = select(vars.selected, &k);
	lcd_SendCmd(cmd);
	lcd_SendChar(k);
}

void ms_edit_oe(void)
{
	safeDefaults();

	lcd_SendCmd(0x01);

	switch (vars.selected)
	{
	case START: // impossible state
		break;
	case TARGET_TEMP:
		lcd_Write("Temperatura");
		lcd_SendCmd(0xC0);
		lcd_Write(aquecimentoTargetStr());
		break;
	case INTERVAL:
		lcd_Write("Intervalo");
		lcd_SendCmd(0xC0);
		lcd_Write(timingIntervalStr());
		break;
	case TOTAL_TIME:
		lcd_Write("Tempo Total");
		lcd_SendCmd(0xC0);
		lcd_Write(timingTotalStr());
		break;
	case VELOCITY:
		lcd_Write("Velocidade");
		lcd_SendCmd(0xC0);
		lcd_Write(motorStr());
		break;
	}
}

void ms_edit(void)
{
	buttonsDownBurst(1);
	buttonsUpBurst(1);
	if (buttonsDown() == 0 && buttonsUp() == 0)
		return;

	if (vars.selected != VELOCITY)
	{
		motorDisable();
	}

	lcd_SendCmd(0xC0);
	switch (vars.selected)
	{
	case START: // impossible state
		break;
	case TARGET_TEMP:
		if (buttonsUp())
			aquecimentoTargetInc();
		if (buttonsDown())
			aquecimentoTargetDec();
		lcd_Write(aquecimentoTargetStr());
		break;
	case INTERVAL:
		if (buttonsUp())
			timingIntervalInc();
		if (buttonsDown())
			timingIntervalDec();
		lcd_Write(timingIntervalStr());
		break;
	case TOTAL_TIME:
		if (buttonsUp())
			timingTotalInc();
		if (buttonsDown())
			timingTotalDec();
		lcd_Write(timingTotalStr());
		break;
	case VELOCITY:
		motorEnable();
		if (buttonsUp())
			motorInc();
		if (buttonsDown())
			motorDec();
		lcd_Write(motorStr());
		break;
	}
}

void ms_prepare_oe(void)
{
	safeDefaults();

	lcd_SendCmd(0x01);
	lcd_Write("Aguarde");
	lcd_SendCmd(0xC0);
	char line[17];
	snprintf(line, 17, "%02d%cC/%s  %s", vars.temperature, 0xDF, aquecimentoTargetStr(), adcToString());
	lcd_Write(line);

	timingRefreshEnable(1);

	aquecimentoEnable();
	eletrodosPos();
}

void ms_prepare(void)
{
	if (adcValue() > 90)
		eletrodosOff();

	if (!timingRefreshDone())
		return;

	lcd_SendCmd(0xC0 + 0);
	char str[17] = "..°C";
	snprintf(str, 17, "%02d%cC/%s  %s", vars.temperature, 0xDF, aquecimentoTargetStr(), eletrodosStatus() == ELETRODOS_OFF ? "     " : adcToString());
	lcd_Write(str);
}

void ms_run_oe(void)
{
	char line[17];
	lcd_SendCmd(0x01);
	snprintf(line, 17, "T:%d%cC   i:%s", vars.temperature, 0xDF, adcToString());
	lcd_Write(line);

	lcd_SendCmd(0xC0);
	snprintf(line, 17, "t:%s    V:%s", timingTotalStr(), motorStr());
	lcd_Write(line);
	timingRefreshEnable(1);

	aquecimentoEnable();
	eletrodosPos();
}
void ms_run(void)
{
	motorEnable();
	timingTotalEnable(1);
	timingIntervalEnable(1);

	if (timingIntervalDone())
	{
		eletrodosInv();
	}

	if (buttonsDown() == 0 && buttonsUp() == 0 && timingRefreshDone() == 0)
		return;

	if (buttonsDown())
		motorDec();
	else if (buttonsUp())
		motorInc();

	char line[17];
	lcd_SendCmd(0x01);
	snprintf(line, 17, "T:%d%cC   i:%s", vars.temperature, 0xDF, adcToString());
	lcd_Write(line);

	lcd_SendCmd(0xC0);
	snprintf(line, 17, "t:%s    V:%s", timingTotalStr(), motorStr());
	lcd_Write(line);
}
void ms_error()
{
	lcd_SendCmd(0x01);
	switch (vars.error)
	{
	case NONE:
		lcd_Write("NONE");
		break;
	case SYNC:
		lcd_Write("SYNC");
		break;
	case THERMO_COM:
		lcd_Write("THERMO_COM");
		break;
	case THERMO_CRC:
		lcd_Write("THERMO_CRC");
		break;
	case THERMO_PULL:
		lcd_Write("THERMO_PULL");
		break;
	}
	cli();
	sleep_mode();
}

ISR(TIMER1_COMPA_vect)
{
	main_timer += 1;
}
