#include "util.hpp"

using namespace ::cfg::util;

// Configuration
util::util()
{
	// Enable clocks for LED port and timer
	RCC_APB2PeriphClockCmd(LedPortClock, ENABLE);
	RCC_APB1PeriphClockCmd(TimerClock, ENABLE);

	// Get system frequency
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	// Setup timer for delays
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_DeInit(Timer);

	// Tick every 1us
	TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.HCLK_Frequency / 1000000)
			- 1;
	TIM_TimeBaseStructure.TIM_Period = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(Timer, &TIM_TimeBaseStructure);
	TIM_SelectOnePulseMode(Timer, TIM_OPMode_Single); // Stop after single pulse

	// Output pins
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = Led3Pin | Led4Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(LedPort, &GPIO_InitStructure);

	// Configure systick, we use it for software debouncer
	// Run it at 10kHz (0.1msec delay)
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 8 / 10000);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}

void util::delay_us(uint16_t usec)
{
	Timer->ARR = usec;
	Timer->SR = ~TIM_FLAG_Update;
	Timer->CR1 |= TIM_CR1_CEN;
	while (!(Timer->SR & TIM_FLAG_Update))
		;
}


void util::delay_ms(uint16_t msec)
{
	while (msec--)
	{
		util::delay_us(1000);
	}
}
