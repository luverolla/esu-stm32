#include "commands.h"

#include "gpio.h"
#include "usart.h"
#include "tim.h"

void turn_on_led(int argc, char** argv)
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim10);

	// reconfiguring pin GPIOA-5 to simple output
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	//PROMPT_SEND_MSG(prompt, "\r\nOk. Led is on\r\n");
}

void turn_off_led(int argc, char** argv)
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim10);

	// reconfiguring pin GPIOA-5 to simple output
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	//PROMPT_SEND_MSG(prompt, "\r\nOk. Led is off\r\n");
}

void toggle_led(int argc, char** argv)
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop_IT(&htim10);

	// reconfiguring pin GPIOA-5 to simple output
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	//PROMPT_SEND_MSG(prompt, "\r\nOk. Led state changed\r\n");
}

void blink_led(int argc, char** argv)
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim10);

	// reconfiguring pin GPIOA-5 to simple output
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//PROMPT_SEND_MSG(prompt, "\r\nOk. Led started blinking\r\n");
}

void pwm_led(int argc, char** argv)
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);

	// 0 = 10% and 9 = 100% duty cicle
	int input = atoi((char*)argv[1]);

	// reconfiguring TIM2 to change pulse
	TIM_OC_InitTypeDef sConfigOC = {0};
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = (input + 1) * 100 - 1;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
		Error_Handler();

	// reconfiguring pin GPIOA-5 to connect to PWM output
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    //PROMPT_SEND_MSG(prompt, "\r\nOk. Led on with intensity level %d\r\n", input);
}
