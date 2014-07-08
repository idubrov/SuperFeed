#include "hw/spindle.hpp"

void hw::spindle::initialize()
{
	// Index pin
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = _index_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(_index_port, &GPIO_InitStructure);

	// Get system frequency
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	// Setup timer for index pin
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_DeInit(_index_timer);

	// Tick every 1us
	TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.HCLK_Frequency / 1000000)
			- 1;
	TIM_TimeBaseStructure.TIM_Period = 10000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(_index_timer, &TIM_TimeBaseStructure);

	// Configure capture
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(_index_timer, &TIM_ICInitStructure);

	TIM_Cmd(_index_timer, ENABLE);
}
