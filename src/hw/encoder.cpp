#include "hw/encoder.hpp"
#include "systick.hpp"

using namespace ::delegate;

void hw::encoder::initialize()
{
	// Button
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = _button_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(_port, &GPIO_InitStructure);

	// Quadrature encoder on timer
	GPIO_InitTypeDef GPIO_InitStructure2;
	GPIO_InitStructure2.GPIO_Pin = _encoder_pins;
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(_port, &GPIO_InitStructure2);

	// Configure alternative functions
	// We don't actually need to remap anything right now

	// Configure timer as rotary encoder
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(_timer, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(_timer, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(_timer, ENABLE);
}
