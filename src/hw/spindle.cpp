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

	// We don't want capture event to generate update interrupt.
	// Update interrupt is used to detect cases when spindle speed is too low
	// (or spindle is stopped)
	TIM_UpdateRequestConfig(_index_timer, TIM_UpdateSource_Regular);

	// Configure interrupts
	TIM_ClearITPendingBit(_index_timer, TIM_IT_Update | TIM_IT_CC1);
	TIM_ITConfig(_index_timer, TIM_IT_Update | TIM_IT_CC1, ENABLE);

	TIM_Cmd(_index_timer, ENABLE);
}

void hw::spindle::overflow_handler()
{
	// Ignore next reading: timer overflowed!
	_overflowed = true;
	_index_timer->ARR = 0xffff;
	_delay.store(0, std::memory_order_relaxed); // No data, spindle speed is unstable
}

void hw::spindle::index_pulse_hanlder()
{
	uint_fast16_t delay = TIM_GetCapture1(_index_timer);
	if (_overflowed)
	{
		// Delay timer overflowed last time, ignore the delay reading
		_overflowed = false;
	}
	else
	{
		// Allow spindle to slow down by 25% (delay is 1.25 the current one)
		uint_fast32_t top = delay + (delay / 4);
		_index_timer->ARR = top < 0xffff ? top : 0xffff;
		_delay.store(delay, std::memory_order_relaxed);
	}
}
