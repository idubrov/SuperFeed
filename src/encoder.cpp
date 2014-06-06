#include "encoder.hpp"

using namespace ::cfg::encoder;

uint32_t encoder::_state = 0; // Current debounce status
volatile bool encoder::_pressed = false; // Current button state

encoder::encoder()
{
	// Enable port and timer clock
	RCC_APB2PeriphClockCmd(PortClock, ENABLE);
	RCC_APB1PeriphClockCmd(TimerClock, ENABLE);

	// Button
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = ButtonPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(Port, &GPIO_InitStructure);

	// Quadrature encoder on timer
	GPIO_InitTypeDef GPIO_InitStructure2;
	GPIO_InitStructure2.GPIO_Pin = EncoderPins;
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(Port, &GPIO_InitStructure2);

	// Configure alternative functions
	// We don't actually need to remap anything right now

	// Configure timer as rotary encoder
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(Timer, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(Timer, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(Timer, ENABLE);
}
