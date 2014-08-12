#ifndef __DRIVER_HPP
#define __DRIVER_HPP

#include "stm32f10x.h"
#include "stepper/stepgen.hpp"

namespace hw
{
class driver
{
public:
	constexpr static uint32_t Clock = 24000000; // Clock frequency
	constexpr static uint32_t Prescaler = 23; // Driver timer prescaler
	constexpr static uint32_t Frequency = Clock / (Prescaler + 1);

public:
	constexpr driver(GPIO_TypeDef* port, uint16_t step_pin, uint16_t dir_pin,
			uint16_t enable_pin, uint16_t reset_pin, TIM_TypeDef* timer) :
			_port(port), _step_pin(step_pin), _dir_pin(dir_pin), _enable_pin(
					enable_pin), _reset_pin(reset_pin), _timer(timer)
	{
	}
	driver(driver const&) = default;

	inline void initialize() const
	{
		initialize_timer();
		initialize_port();
	}

	inline void set_direction(bool dir) const
	{
		if (dir)
			_port->BSRR = _dir_pin;
		else
			_port->BRR = _dir_pin;
	}

	inline void reset() const {
		_port->BRR = _reset_pin;
	}

	inline void release() const {
		_port->BSRR = _reset_pin;
	}

	inline void disable() const {
		_port->BRR = _enable_pin;
	}

	inline void enable() const {
		_port->BSRR = _enable_pin;
	}

	inline bool check_stopped() const
	{
		// Step generation is still running
		if (_timer->CR1 & TIM_CR1_CEN)
			return false;

		// If there is a pending interrupt, wait until it is cleared.
		// (for instance, we got here just after last timer overflow and it wasn't processed yet).
		while (TIM_GetITStatus(_timer, TIM_IT_Update))
			;
		return true;
	}

	inline void set_last_pulse() const
	{
		_timer->CR1 |= TIM_CR1_OPM;
	}

	inline void set_delay(uint16_t delay, uint16_t step_len) const
	{
		_timer->ARR = delay;
		_timer->CCR1 = (delay >= step_len) ? (delay - step_len) : 0;
	}

	inline void start(bool is_last) const
	{
		TIM_SelectOnePulseMode(_timer,
				is_last ? TIM_OPMode_Single : TIM_OPMode_Repetitive);
		TIM_CtrlPWMOutputs(_timer, ENABLE);
		TIM_Cmd(_timer, ENABLE);
	}

	inline void manual_update() const
	{
		// Load ARR/CC1 from preload registers
		TIM_GenerateEvent(_timer, TIM_EventSource_Update);

		// Wait till interrupt finishes
		while (TIM_GetITStatus(_timer, TIM_IT_Update))
			;
	}

	/// Convert nanoseconds to driver ticks, with ceil rounding.
	inline static uint16_t ns2ticks(uint16_t ns)
	{
		constexpr uint32_t NanosecondsInTick = 1000000000 / Frequency;

		static_assert(Clock == (Prescaler + 1) * 1000000,
				"Driver clock must run with 1us intervals");
		static_assert(NanosecondsInTick == 1000, "");
		return (ns + NanosecondsInTick - 1) / NanosecondsInTick;
	}
private:
	void initialize_port() const;
	void initialize_timer() const;
private:
	// Port and pins
	GPIO_TypeDef* const _port;

	// Driver delays (in driver timer ticks)
	uint16_t const _step_pin;
	uint16_t const _dir_pin;
	uint16_t const _enable_pin;
	uint16_t const _reset_pin;

	// Timers
	TIM_TypeDef* const _timer;

	// Step length
	//uint32_t const _step_len;
};
}

#endif /* __DRIVER_HPP */
