#include <avr/io.h>
#include "motor.h"

static uint8_t motorDuty;

void motorInit()
{
    MOTOR_DDRR |= MOTOR_MASK;
    OCR2A = 255; // 16E6 / 255 / X = 1kHz
    OCR2B = MOTOR_OFF;
    TCCR2A = (1 << COM2B1) | (1 << COM2B0) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << CS22) | (1 << WGM22);
}

void motorEnable(void)
{
    OCR2B = motorDuty;
}

void motorDisable(void)
{
    OCR2B = MOTOR_OFF;
}

void motorOff(void)
{
    motorDuty = MOTOR_OFF;
}

void motorLow(void)
{
    motorDuty = MOTOR_LOW;
}

void motorMed(void)
{
    motorDuty = MOTOR_MED;
}

void motorHigh(void)
{
    motorDuty = MOTOR_HIG;
}

void motorInc()
{
    switch (motorDuty)
    {
    case MOTOR_OFF:
        motorLow();
        break;
    case MOTOR_LOW:
        motorMed();
        break;
    case MOTOR_MED:
        motorHigh();
        break;
    case MOTOR_HIG:
        motorHigh();
        break;
    }
}

void motorDec()
{
    switch (motorDuty)
    {
    case MOTOR_OFF:
        motorOff();
        break;
    case MOTOR_LOW:
        motorOff();
        break;
    case MOTOR_MED:
        motorLow();
        break;
    case MOTOR_HIG:
        motorMed();
        break;
    }
}

const char *motorStr(void)
{
    switch (motorDuty)
    {
    case MOTOR_OFF:
        return "OFF";
    case MOTOR_LOW:
        return "LOW";
    case MOTOR_MED:
        return "MED";
    case MOTOR_HIG:
        return "HIG";
    }
    return "UNK";
}