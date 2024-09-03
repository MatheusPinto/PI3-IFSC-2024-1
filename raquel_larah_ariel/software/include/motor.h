#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

#define MOTOR_PORT PORTD
#define MOTOR_DDRR DDRD
#define MOTOR_PINR PIND
#define MOTOR_MASK (1 << PIND3)

typedef enum
{
    MOTOR_OFF = 0,
    MOTOR_LOW = 50,
    MOTOR_MED = 150,
    MOTOR_HIG = 250,
} motorVelocityType;

void motorInit();
void motorEnable(void);
void motorDisable(void);
void motorOff(void);
void motorLow(void);
void motorMed(void);
void motorHigh(void);
/*Retorna a representação em texto do estado selecionado (sempre 4 chars incluindo '\0')*/
const char *motorStr(void);
void motorInc();
void motorDec();

#endif
