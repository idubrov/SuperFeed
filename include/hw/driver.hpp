#ifndef __DRIVER_HPP
#define __DRIVER_HPP

#include "stm32f10x.h"
#include "config.hpp"

namespace hw
{
class driver
{
public:
	constexpr driver(GPIO_TypeDef* port, uint16_t step_pin, uint16_t dir_pin,
			uint16_t enable_pin, uint16_t reset_pin, TIM_TypeDef* timer,
			uint32_t step_len) :
			_port(port), _step_pin(step_pin), _dir_pin(dir_pin), _enable_pin(
					enable_pin), _reset_pin(reset_pin), _timer(timer), _step_len(
					step_len)
	{
	}
	driver(driver const&) = default;

	inline void initialize() const
	{
		initialize_timer();
		initialize_port();
	}

	inline bool is_running() const
	{
		return _timer->CR1 & TIM_CR1_CEN;
	}

	inline void set_last_pulse() const
	{
		_timer->CR1 |= TIM_CR1_OPM;
	}

	inline void set_delay(uint32_t delay) const
	{
		_timer->ARR = delay;
		_timer->CCR1 = (delay >= _step_len) ? (delay - _step_len) : 0;
	}

	inline void start(bool is_last) const
	{
		TIM_SelectOnePulseMode(_timer,
				is_last ? TIM_OPMode_Single : TIM_OPMode_Repetitive);
		TIM_CtrlPWMOutputs(_timer, ENABLE);
		TIM_Cmd(_timer, ENABLE);
	}

	/// Wait until interrupt is processed by the handler
	inline void wait_flag() const
	{
		while (TIM_GetITStatus(_timer, TIM_IT_Update))
			;
	}

	inline void manual_update() const
	{
		// Load ARR/CC1 from preload registers
		TIM_GenerateEvent(_timer, TIM_EventSource_Update);

		// Wait till interrupt finishes
		while (TIM_GetITStatus(_timer, TIM_IT_Update))
			;
	}

private:
	void initialize_port() const;
	void initialize_timer() const;
private:
	// Port and pins
	GPIO_TypeDef* const _port;
	uint16_t const _step_pin;
	uint16_t const _dir_pin;
	uint16_t const _enable_pin;
	uint16_t const _reset_pin;

	// Timers
	TIM_TypeDef* const _timer;

	// Step length
	uint32_t const _step_len;
};
}

#endif /* __DRIVER_HPP */
