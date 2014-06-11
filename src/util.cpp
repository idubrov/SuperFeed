#include "util.hpp"

using namespace ::cfg::util;

// Configuration
util::util()
{
	// Enable clocks for LED port and timer
	RCC_APB2PeriphClockCmd(LedPortClock, ENABLE);
	RCC_APB1PeriphClockCmd(DelayTimerClock, ENABLE);
	RCC_APB1PeriphClockCmd(TickTimerClock, ENABLE);

	// Get system frequency
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	// Setup timer for delays
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_DeInit(DelayTimer);
	TIM_DeInit(TickTimer);

	// Both timers tick every 1us
	TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.HCLK_Frequency / 1000000)
			- 1;
	TIM_TimeBaseStructure.TIM_Period = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(DelayTimer, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TickTimer, &TIM_TimeBaseStructure);

	// Delay timer runs in single pulse mode
	TIM_SelectOnePulseMode(DelayTimer, TIM_OPMode_Single);
	// Tick timer runs from 0 to 0xffff
	TIM_SetAutoreload(TickTimer, UINT16_MAX);
	TIM_Cmd(TickTimer, ENABLE);

	// Output pins
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = Led3Pin | Led4Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(LedPort, &GPIO_InitStructure);
}

void util::delay_us(uint16_t usec)
{
	DelayTimer->ARR = usec;
	DelayTimer->SR = ~TIM_FLAG_Update;
	DelayTimer->CR1 |= TIM_CR1_CEN;
	while (!(DelayTimer->SR & TIM_FLAG_Update))
		;
}


void util::delay_ms(uint16_t msec)
{
	while (msec--)
	{
		util::delay_us(1000);
	}
}
