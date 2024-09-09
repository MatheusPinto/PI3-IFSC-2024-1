/*
 * vibrator.c
 *
 *  Created on: Jul 12, 2024
 *      Author: guidomomm
 */

#include "motor.h"

void motor_init(motor_t *motor)
{
	HAL_TIM_PWM_Start(motor->motor_htim, motor->motor_channel);
};

void motor_speed(motor_t motor, uint16_t speed)
{
	if(speed < 0 || speed >100)
	{
		return HAL_ERROR;
	}

	motor.motor_htim->Instance->CCR1 = speed;
};

void motor_pulse(motor_t motor)
{

	if(motor.distance>1500)
	{
		motor_speed(motor, 0);
		HAL_Delay(100);
	}
	else if(motor.distance<=1500 && motor.distance>1000)
	{
		motor_speed(motor,50);
		HAL_Delay(100);
		motor_speed(motor,0);
		HAL_Delay(1000);
	}
	else if(motor.distance<=1000 && motor.distance>750)
	{
		motor_speed(motor,50);
		HAL_Delay(100);
		motor_speed(motor,0);
		HAL_Delay(800);
	}
	else if(motor.distance<=750 && motor.distance>500)
	{
		motor_speed(motor,50);
		HAL_Delay(100);
		motor_speed(motor,0);
		HAL_Delay(600);
	}
	else if(motor.distance<=500 && motor.distance>250)
	{
		motor_speed(motor, 50);
		HAL_Delay(100);
		motor_speed(motor,0);
		HAL_Delay(400);
	}
	else if(motor.distance<=250 && motor.distance>100)
	{
		motor_speed(motor, 50);
		HAL_Delay(100);
		motor_speed(motor,0);
		HAL_Delay(200);
	}
	else if(motor.distance<=100 && motor.distance>=0)
	{
		motor_speed(motor, 50);
		HAL_Delay(100);
		motor_speed(motor,0);
		HAL_Delay(100);
	}
};
