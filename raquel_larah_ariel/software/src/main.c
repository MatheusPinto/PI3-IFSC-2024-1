#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "lcd.h"
#include "buttons.h"
#include "termometro.h"
#include "interface.h"
#include "main.h"

struct
{
    VAR_TEMPERATURA_T target_temp;
    VAR_TEMPO_T tempo;
    VAR_INTERVALO_T intervalo;
    VAR_VELOCIDADE_T velocidade;
    VAR_MOTOR_T motor : 1;
    VAR_ELETRODO_T eletrodo : 2;
    VAR_AQUECE_T aquece : 1;
} vars = {
    .target_temp = VAR_TEMPERATURA_DEFAULT,
    .tempo = VAR_TEMPO_DEFAULT,
    .intervalo = VAR_INTERVALO_DEFAULT,
    .velocidade = VAR_VELOCIDADE_DEFAULT,
    .motor = VAR_MOTOR_DEFAULT,
    .eletrodo = VAR_ELETRODO_DEFAULT,
    .aquece = VAR_AQUECE_DEFAULT,
};

typedef enum
{
    MAIN_STATE_RESUMO,
    MAIN_STATE_SEL_TEMPERATURA,
    MAIN_STATE_SEL_TEMPO,
    MAIN_STATE_SEL_INTERVALO,
    MAIN_STATE_SEL_VELOCIDADE,
    MAIN_STATE_AJUSTE_TEMPERATURA,
    MAIN_STATE_AJUSTE_TEMPERATURA_INC,
    MAIN_STATE_AJUSTE_TEMPERATURA_DEC,
    MAIN_STATE_AJUSTE_TEMPO,
    MAIN_STATE_AJUSTE_TEMPO_INC,
    MAIN_STATE_AJUSTE_TEMPO_DEC,
    MAIN_STATE_AJUSTE_INTERVALO,
    MAIN_STATE_AJUSTE_INTERVALO_INC,
    MAIN_STATE_AJUSTE_INTERVALO_DEC,
    MAIN_STATE_AJUSTE_VELOCIDADE,
    MAIN_STATE_AJUSTE_VELOCIDADE_INC,
    MAIN_STATE_AJUSTE_VELOCIDADE_DEC,
    MAIN_STATE_AQUECIMENTO,
    MAIN_STATE_FUNCIONAMENTO_SET_TIMER,
    MAIN_STATE_FUNCIONAMENTO,
    MAIN_STATE_FUNCIONAMENTO_DEC_MINUTO,
    MAIN_STATE_FUNCIONAMENTO_INVERTE,
    MAIN_STATE_ERROR,
    MAIN_STATE_ERROR_BT,
    MAIN_STATE_ERROR_TERMO,
    MAIN_STATE_LENGTH,
} main_stateType;

static main_stateType main_state = MAIN_STATE_RESUMO;
static uint8_t main_state_changed = 1;

static void main_state_resumo(void);
static void main_state_sel_temperatura(void);
static void main_state_sel_tempo(void);
static void main_state_sel_intervalo(void);
static void main_state_sel_velocidade(void);
static void main_state_ajuste_temperatura(void);
static void main_state_ajuste_temperatura_inc(void);
static void main_state_ajuste_temperatura_dec(void);
static void main_state_ajuste_tempo(void);
static void main_state_ajuste_tempo_inc(void);
static void main_state_ajuste_tempo_dec(void);
static void main_state_ajuste_intervalo(void);
static void main_state_ajuste_intervalo_inc(void);
static void main_state_ajuste_intervalo_dec(void);
static void main_state_ajuste_velocidade(void);
static void main_state_ajuste_velocidade_inc(void);
static void main_state_ajuste_velocidade_dec(void);
static void main_state_aquecimento(void);
static void main_state_funcionamento_set_timer(void);
static void main_state_funcionamento(void);
static void main_state_funcionamento_dec_minuto(void);
static void main_state_funcionamento_inverte(void);
static void main_state_error(void);
static void main_state_error_bt(void);
static void main_state_error_termo(void);

static void (*const states[MAIN_STATE_LENGTH])(void) = {
    main_state_resumo,
    main_state_sel_temperatura,
    main_state_sel_tempo,
    main_state_sel_intervalo,
    main_state_sel_velocidade,
    main_state_ajuste_temperatura,
    main_state_ajuste_temperatura_inc,
    main_state_ajuste_temperatura_dec,
    main_state_ajuste_tempo,
    main_state_ajuste_tempo_inc,
    main_state_ajuste_tempo_dec,
    main_state_ajuste_intervalo,
    main_state_ajuste_intervalo_inc,
    main_state_ajuste_intervalo_dec,
    main_state_ajuste_velocidade,
    main_state_ajuste_velocidade_inc,
    main_state_ajuste_velocidade_dec,
    main_state_aquecimento,
    main_state_funcionamento_set_timer,
    main_state_funcionamento,
    main_state_funcionamento_dec_minuto,
    main_state_funcionamento_inverte,
    main_state_error,
    main_state_error_bt,
    main_state_error_termo,
};

volatile uint16_t valor_adc = VAR_CORRENTE_ADC_THRESHOLD; // TODO: fix this
volatile uint16_t main_timer = 0;
static uint8_t contador_intervalo = 0;
static int16_t temperatura;
static termo_result result;
static uint8_t buttons;

int main(void)
{
    /**** INICIALIZAÇÔES ****/
    lcd_Init();
    buttons_init();
    //    Mode 4 - TCC - OCRA
    TCCR1A = 0;
    TCCR1B = (1 << CS10) | (1 << WGM12);
    // 16Mhz/1/16000 = 1kHz -> 1ms
    OCR1A = 16000;
    TIMSK1 = (1 << OCIE1A);

    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) |
             (1 << ADPS2) | (1 << ADPS0) | (1 << ADPS1); // prescaler 128
    DIDR0 = (1 << ADC0D);

    // set_sleep_mode(SLEEP_MODE_IDLE);

    do
    {
        sei();
        result = termo_read(&temperatura);
        switch (result)
        {
        case TERMO_UNINIT:
            result = termo_conv();
            lcd_Write("iniciando");
            break;
        case TERMO_BUSY:
        case TERMO_READY:
            break;
        case TERMO_ERROR_COM:
        case TERMO_ERROR_CRC:
        case TERMO_ERROR_PULL:
            vars.target_temp = 0;
            temperatura = 0;
            break;
        }
    } while (result == TERMO_BUSY);

    updateTargetTemp(vars.target_temp);
    updateTempo(vars.tempo);
    updateIntervalo(vars.intervalo);
    updateVelocidade(vars.velocidade);
    updateError(result);
    while (1)
    {
        buttons = buttons_read();

        result = termo_read(&temperatura);
        switch (result)
        {
        case TERMO_BUSY:
            break;
        case TERMO_READY:
            updateTemperatura(temperatura);
            result = termo_conv();
            break;
        case TERMO_UNINIT:
        case TERMO_ERROR_COM:
        case TERMO_ERROR_CRC:
        case TERMO_ERROR_PULL:
            vars.target_temp = 0;
            updateTargetTemp(vars.target_temp);
            updateError(result);
            break;
        }

        states[main_state]();

        main_state_changed = 0;
        switch (main_state)
        {
        case MAIN_STATE_RESUMO:
            if (buttons == BUTTONS_DOWN)
            {
                main_state = MAIN_STATE_SEL_TEMPERATURA;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_UP)
            {
                main_state = MAIN_STATE_SEL_VELOCIDADE;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_AQUECIMENTO;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_SEL_TEMPERATURA:
            if (buttons == BUTTONS_DOWN)
            {
                main_state = MAIN_STATE_SEL_TEMPO;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_UP)
            {
                main_state = MAIN_STATE_RESUMO;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_AJUSTE_TEMPERATURA;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_SEL_TEMPO:
            if (buttons == BUTTONS_DOWN)
            {
                main_state = MAIN_STATE_SEL_INTERVALO;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_UP)
            {
                main_state = MAIN_STATE_SEL_TEMPERATURA;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_AJUSTE_TEMPO;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_SEL_INTERVALO:
            if (buttons == BUTTONS_DOWN)
            {
                main_state = MAIN_STATE_SEL_VELOCIDADE;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_UP)
            {
                main_state = MAIN_STATE_SEL_TEMPO;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_AJUSTE_INTERVALO;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_SEL_VELOCIDADE:
            if (buttons == BUTTONS_DOWN)
            {
                main_state = MAIN_STATE_RESUMO;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_UP)
            {
                main_state = MAIN_STATE_SEL_INTERVALO;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_AJUSTE_VELOCIDADE;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_AJUSTE_TEMPERATURA:
            if (buttons == BUTTONS_DOWN && vars.target_temp > 0)
            {
                main_state = MAIN_STATE_AJUSTE_TEMPERATURA_DEC;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_UP &&
                     result == TERMO_READY &&
                     vars.target_temp <= VAR_TEMPERATURA_MAX - VAR_TEMPERATURA_STEP)
            {
                main_state = MAIN_STATE_AJUSTE_TEMPERATURA_INC;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_SEL_TEMPERATURA;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_AJUSTE_TEMPO:
            if (buttons == BUTTONS_DOWN && vars.tempo >= VAR_TEMPO_MIN + VAR_TEMPO_STEP)
            {
                main_state = MAIN_STATE_AJUSTE_TEMPO_DEC;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_UP && vars.tempo <= VAR_TEMPO_MAX - VAR_TEMPO_STEP)
            {
                main_state = MAIN_STATE_AJUSTE_TEMPO_INC;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_SEL_TEMPO;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_AJUSTE_INTERVALO:
            if (buttons == BUTTONS_DOWN && vars.intervalo >= VAR_INTERVALO_MIN + VAR_INTERVALO_STEP)
            {
                main_state = MAIN_STATE_AJUSTE_INTERVALO_DEC;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_UP && vars.intervalo <= VAR_INTERVALO_MAX - VAR_INTERVALO_STEP)
            {
                main_state = MAIN_STATE_AJUSTE_INTERVALO_INC;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_SEL_INTERVALO;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_AJUSTE_VELOCIDADE:
            if (buttons == BUTTONS_DOWN && vars.velocidade >= VAR_VELOCIDADE_MIN + VAR_VELOCIDADE_STEP)
            {
                main_state = MAIN_STATE_AJUSTE_VELOCIDADE_DEC;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_UP && vars.velocidade <= VAR_VELOCIDADE_MAX - VAR_VELOCIDADE_STEP)
            {
                main_state = MAIN_STATE_AJUSTE_VELOCIDADE_INC;
                main_state_changed = 1;
            }
            else if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_SEL_VELOCIDADE;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_AJUSTE_TEMPERATURA_INC:
        case MAIN_STATE_AJUSTE_TEMPERATURA_DEC:
            main_state = MAIN_STATE_AJUSTE_TEMPERATURA;
            main_state_changed = 1;
            break;
        case MAIN_STATE_AJUSTE_TEMPO_INC:
        case MAIN_STATE_AJUSTE_TEMPO_DEC:
            main_state = MAIN_STATE_AJUSTE_TEMPO;
            main_state_changed = 1;
            break;
        case MAIN_STATE_AJUSTE_INTERVALO_INC:
        case MAIN_STATE_AJUSTE_INTERVALO_DEC:
            main_state = MAIN_STATE_AJUSTE_INTERVALO;
            main_state_changed = 1;
            break;
        case MAIN_STATE_AJUSTE_VELOCIDADE_INC:
        case MAIN_STATE_AJUSTE_VELOCIDADE_DEC:
            main_state = MAIN_STATE_AJUSTE_VELOCIDADE;
            main_state_changed = 1;
            break;
        case MAIN_STATE_AQUECIMENTO:
            if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_RESUMO;
                main_state_changed = 1;
            }
            else if (valor_adc > VAR_CORRENTE_ADC_THRESHOLD && temperatura >= vars.target_temp)
            {
                main_state = MAIN_STATE_FUNCIONAMENTO_SET_TIMER;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_FUNCIONAMENTO_SET_TIMER:
            main_state = MAIN_STATE_FUNCIONAMENTO;
            main_state_changed = 1;
            break;
        case MAIN_STATE_FUNCIONAMENTO:
            if (buttons == BUTTONS_OK)
            {
                main_state = MAIN_STATE_RESUMO;
                main_state_changed = 1;
            }
            if (main_timer == 0)
            {
                main_state = MAIN_STATE_FUNCIONAMENTO_DEC_MINUTO;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_FUNCIONAMENTO_DEC_MINUTO:
            if (vars.tempo == 0)
            {
                main_state = MAIN_STATE_RESUMO;
                main_state_changed = 1;
            }
            else if (contador_intervalo == vars.intervalo && vars.intervalo != 0)
            {
                main_state = MAIN_STATE_FUNCIONAMENTO_INVERTE;
                main_state_changed = 1;
            }
            else
            {
                main_state = MAIN_STATE_FUNCIONAMENTO_SET_TIMER;
                main_state_changed = 1;
            }
            break;
        case MAIN_STATE_FUNCIONAMENTO_INVERTE:
            main_state = MAIN_STATE_FUNCIONAMENTO_SET_TIMER;
            main_state_changed = 1;
            break;
        case MAIN_STATE_ERROR:
            break;
        case MAIN_STATE_ERROR_BT:
            break;
        case MAIN_STATE_ERROR_TERMO:
            break;
        case MAIN_STATE_LENGTH:
            break;
        }
    }
}

static void main_state_resumo(void)
{
    telaSelecao(SO_NEXT);
}
static void main_state_sel_temperatura(void)
{
    telaSelecao(SO_TTEMP);
}
static void main_state_sel_tempo(void)
{
    telaSelecao(SO_TEMPO);
}
static void main_state_sel_intervalo(void)
{
    telaSelecao(SO_INT);
}
static void main_state_sel_velocidade(void)
{
    telaSelecao(SO_VEL);
}
static void main_state_ajuste_temperatura(void)
{
    if (result != TERMO_READY && result != TERMO_BUSY)
    {
        telaEdicao(ED_TERR);
    }
    else
    {
        telaEdicao(ED_TTEMP);
    }
}
static void main_state_ajuste_temperatura_inc(void)
{
    vars.target_temp += VAR_TEMPERATURA_STEP;
    if (vars.target_temp < VAR_TEMPERATURA_MIN)
    {
        vars.target_temp = VAR_TEMPERATURA_MIN;
    }
    updateTargetTemp(vars.target_temp);
}
static void main_state_ajuste_temperatura_dec(void)
{
    vars.target_temp -= VAR_TEMPERATURA_STEP;
    if (vars.target_temp < VAR_TEMPERATURA_MIN)
    {
        vars.target_temp = 0;
    }
    updateTargetTemp(vars.target_temp);
}
static void main_state_ajuste_tempo(void)
{
    telaEdicao(ED_TEMPO);
}
static void main_state_ajuste_tempo_inc(void)
{
    vars.tempo += VAR_TEMPO_STEP;
    updateTempo(vars.tempo);
}
static void main_state_ajuste_tempo_dec(void)
{
    vars.tempo -= VAR_TEMPO_STEP;
    updateTempo(vars.tempo);
}
static void main_state_ajuste_intervalo(void)
{
    telaEdicao(ED_INT);
}
static void main_state_ajuste_intervalo_inc(void)
{
    vars.intervalo += VAR_INTERVALO_STEP;
    updateIntervalo(vars.intervalo);
}
static void main_state_ajuste_intervalo_dec(void)
{
    vars.intervalo -= VAR_INTERVALO_STEP;
    updateIntervalo(vars.intervalo);
}
static void main_state_ajuste_velocidade(void)
{
    // TODO: ligar o motor
    telaEdicao(ED_VEL);
}
static void main_state_ajuste_velocidade_inc(void)
{
    vars.velocidade += VAR_VELOCIDADE_STEP;
    updateVelocidade(vars.velocidade);
}
static void main_state_ajuste_velocidade_dec(void)
{
    vars.velocidade -= VAR_VELOCIDADE_STEP;
    updateVelocidade(vars.velocidade);
}
static void main_state_aquecimento(void)
{
    updateCorrente(valor_adc);
    telaAquecendo();
}
static void main_state_funcionamento_set_timer(void)
{
    main_timer = 60000;
}
static void main_state_funcionamento(void)
{
    updateCorrente(valor_adc);
    telaFuncionamento();
}
static void main_state_funcionamento_dec_minuto(void)
{
    vars.tempo -= 1;
    updateTempo(vars.tempo);
    contador_intervalo += 1;
}
static void main_state_funcionamento_inverte(void)
{
    contador_intervalo = 0;
    // TODO: inverte polaridade;
}
static void main_state_error(void) {}
static void main_state_error_bt(void) {}
static void main_state_error_termo(void) {}

ISR(TIMER1_COMPA_vect)
{
    termo_update();
    buttons_update();
    if (main_timer > 0)
    {
        main_timer -= 1;
    }
}

ISR(ADC_vect)
{
    valor_adc = ADC;
}