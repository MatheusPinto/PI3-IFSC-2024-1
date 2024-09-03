#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

#include "adc.h"

static volatile uint16_t adc_Value;

void adcInit(void)
{
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADPS2) | (1 << ADPS0) | (1 << ADATE) | (1 << ADPS1) | (1 << ADIE);
	DIDR0 = (1 << ADC0D);
}

void adcEnable(void)
{
	ADCSRA |= (1 << ADEN);
}

void adcDisable(void)
{
	ADCSRA &= ~(1 << ADEN);
}

int16_t adcValue(void)
{
	return ((int32_t)adc_Value - ADC_MIN) * (ADC_MAP_MAX - ADC_MAP_MIN) / (ADC_MAX - ADC_MIN) + ADC_MAP_MIN;
}

const char *adcToString()
{
	static char str[6] = "sCc";
	snprintf(str, 6, "%+02dmA", adcValue());
	return str;
}

ISR(ADC_vect)
{
	adc_Value = ADC;
}