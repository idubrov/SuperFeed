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

	// Tick every 0.1ms
	TIM_TimeBaseStructure.TIM_Prescaler = (RCC_Clocks.HCLK_Frequency / 10000)
			- 1;
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
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

	// Configure slave mode, reset on capture
	TIM_SelectSlaveMode(_index_timer, TIM_SlaveMode_Reset);
	TIM_SelectInputTrigger(_index_timer, TIM_TS_TI1FP1);

	// Configure interrupts
	TIM_ClearITPendingBit(_index_timer, TIM_IT_Update | TIM_IT_CC1);
	TIM_ITConfig(_index_timer, TIM_IT_Update | TIM_IT_CC1, ENABLE);

	TIM_Cmd(_index_timer, ENABLE);
}

void hw::spindle::overflow_handler()
{
	// Ignore next reading: timer overflowed!
	_overflowed = true;
	_captured = 0; // No data

	// FIXME: increase ARR if it is not 0xffff already?
}

void hw::spindle::index_pulse_hanlder()
{
	if (_overflowed)
	{
		_overflowed = false;
	}
	else
	{
		_captured = TIM_GetCapture1(_index_timer);
		// FIXME: we probably want to set ARR to something like _captured * 1.2, so
		// we catch fast spindle speed changes early
	}
}
