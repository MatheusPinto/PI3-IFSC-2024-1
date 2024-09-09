/*
 * motor.h
 *
 *  Created on: Jul 12, 2024
 *      Author: guidomomm
 */

#include "stm32f1xx_hal.h"
#include "sensor.h"

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

typedef struct{
	TIM_HandleTypeDef *motor_htim;	//	PWM Timer
	uint16_t motor_channel;	//	PWM Output
	uint32_t distance;
} motor_t;

void motor_init(motor_t *motor);
void motor_speed(motor_t motor, uint16_t speed);
void motor_pulse(motor_t motor);

#endif /* INC_VIBRATOR_H_ */
