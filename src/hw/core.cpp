#include "hw/core.hpp"

using namespace ::cfg::util;

// Configuration
void hw::core::setup()
{
	// Get system frequency
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	// Setup timer for delays
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_DeInit(DelayTimer);

	// Tick every 1us
	TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.HCLK_Frequency / 1000000)
			- 1;
	TIM_TimeBaseStructure.TIM_Period = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(DelayTimer, &TIM_TimeBaseStructure);

	// Delay timer runs in single pulse mode
	TIM_SelectOnePulseMode(DelayTimer, TIM_OPMode_Single);

	// Output pins
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = Led3Pin | Led4Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(LedPort, &GPIO_InitStructure);
}

void hw::core::delay_us(uint16_t usec)
{
	DelayTimer->ARR = usec;
	DelayTimer->SR = ~TIM_FLAG_Update;
	DelayTimer->CR1 |= TIM_CR1_CEN;
	while (!(DelayTimer->SR & TIM_FLAG_Update))
		;
}

void hw::core::delay_ms(uint16_t msec)
{
	while (msec--)
	{
		core::delay_us(1000);
	}
}
