#include <stdint.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <ds18b20/ds18b20.h>

#define PIN_AQUECE_PORT PORTB
#define PIN_AQUECE_DDRR DDRB
#define PIN_AQUECE_PINR PINB
#define PIN_AQUECE_MASK (1 << PINB7)
#define PIN_AQUECE_ON_HIGH 1

#define PIN_TERMO_PORT PORTB
#define PIN_TERMO_DDR DDRB
#define PIN_TERMO_PORTIN PINB
#define PIN_TERMO_MASK (1 << PORTB0)

#define VAR_TEMPERATURA_DEFAULT 60
#define VAR_TEMPERATURA_MIN 50
#define VAR_TEMPERATURA_MAX 99
#define VAR_TEMPERATURA_STEP 1

static struct
{
    volatile uint16_t timer;
    int8_t target_temp;
    int8_t current_temp;
    uint16_t timeout : 1;
    uint8_t aquece : 1;
    uint8_t status : 3;
} vars;

void tick()
{
    if (vars.timer > 0)
    {
        vars.timer -= 1;
        if (vars.timer == 0)
        {
            vars.timeout = 1;
        }
    }
}

void set_timeout(uint16_t ms)
{
    cli();
    vars.timer = ms;
    vars.timeout = 0;
    sei();
}

static uint8_t timeout()
{
    if (vars.timeout)
    {
        vars.timeout = 0;
        return 1;
    }
    return 0;
}

void aquece_on()
{
    vars.aquece = 1;
}

void aquece_off()
{
#if PIN_AQUECE_ON_HIGH == 1
    PIN_AQUECE_PORT &= ~PIN_AQUECE_MASK;
#elif PIN_AQUECE_ON_HIGH == 0
    PIN_AQUECE_PORT |= PIN_AQUECE_MASK;
#else
#error "O valor de PIN_AQUECE_ON_HIGH deve ser `1` ou `0`
#endif
    vars.aquece = 0;
}

/*******************************************************************************
 * Inicializa os pinos utilizados pelo módulo (pinos definidos em 
 * `aquecimento.h`). PIN_AQUECE é configurado como saída e inicializado como 
 * `desligado` à depender do valor de PIN_AQUECE_HIGH_ON. É iniciada uma 
 * conversão de temperatura e se não houverem erros o módulo se mantém em estado
 * UNINIT por 1 segundo **E** até que a função `temperatura_update` seja chamada
 ******************************************************************************/
void temperatura_init()
{
    PIN_AQUECE_DDRR |= PIN_AQUECE_MASK;
    aquece_off();

    vars.target_temp = VAR_TEMPERATURA_DEFAULT;
    vars.current_temp = 0;
    vars.timeout = 0;

    uint8_t st = ds18b20convert(&PIN_TERMO_PORT, &PIN_TERMO_DDR, &PIN_TERMO_PORTIN, PIN_TERMO_MASK, NULL);
    vars.status = st ? st : 7; // erro ou uninit

    set_timeout(1000);
}


uint8_t temperatura_update()
{
    if (timeout())
    {
        vars.status = ds18b20read(&PIN_TERMO_PORT, &PIN_TERMO_DDR, &PIN_TERMO_PORTIN, PIN_TERMO_MASK, NULL, &vars.current_temp);
        set_timeout(1000);
        vars.status = ds18b20convert(&PIN_TERMO_PORT, &PIN_TERMO_DDR, &PIN_TERMO_PORTIN, PIN_TERMO_MASK, NULL);
        vars.status = 0; // Ok
    }

    if (vars.status == 7)
        return; // esperando primeira conversão

    if (vars.status != 0)
    {
        aquece_off();
        vars.target_temp = 0;
        return;
    }

    if (vars.aquece == 1 && vars.target_temp > vars.current_temp)
    {
#if PIN_AQUECE_ON_HIGH == 1
        PIN_AQUECE_PORT |= PIN_AQUECE_MASK;
#elif PIN_AQUECE_ON_HIGH == 0
        PIN_AQUECE_PORT &= ~PIN_AQUECE_MASK;
#else
#error "O valor de PIN_AQUECE_ON_HIGH deve ser `1` ou `0`
#endif
    }
}

void temperatura_inc(void)
{
    uint8_t new_temp = vars.target_temp + VAR_TEMPERATURA_STEP;
    if (new_temp < VAR_TEMPERATURA_MIN)
    {
        vars.target_temp = VAR_TEMPERATURA_MIN;
    }
    else if (new_temp > VAR_TEMPERATURA_MAX)
    {
        vars.target_temp = VAR_TEMPERATURA_MAX;
    }
    else
    {
        vars.target_temp = new_temp;
    }
}

void temperatura_dec(void)
{
    uint8_t new_temp = vars.target_temp - VAR_TEMPERATURA_STEP;
    if (new_temp < VAR_TEMPERATURA_MIN)
    {
        vars.target_temp = 0;
    }
    else
    {
        vars.target_temp = new_temp;
    }
}