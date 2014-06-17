#ifndef __STEPPER_HPP
#define __STEPPER_HPP

#include "config.hpp"
#include "util.hpp"

namespace stepper
{

extern "C" void TIM4_IRQHandler();

// Stepper hardware configuration (pins and their timers)
class hw
{
public:
	constexpr hw(GPIO_TypeDef* port, uint16_t step_pin, uint16_t dir_pin,
			uint16_t enable_pin, uint16_t reset_pin, TIM_TypeDef* pulse_tim,
			uint16_t pulse_tim_trigger_source, TIM_TypeDef* step_tim) :
			_port(port), _step_pin(step_pin), _dir_pin(dir_pin), _enable_pin(
					enable_pin), _reset_pin(reset_pin), _pulse_tim(pulse_tim), _pulse_tim_trigger_source(
					pulse_tim_trigger_source), _step_tim(step_tim)
	{
	}
	hw(hw const&) = default;

	// Port and pins
	GPIO_TypeDef* const _port;
	uint16_t const _step_pin;
	uint16_t const _dir_pin;
	uint16_t const _enable_pin;
	uint16_t const _reset_pin;

	// Timers
	TIM_TypeDef* const _pulse_tim;
	uint16_t const _pulse_tim_trigger_source;
	TIM_TypeDef* const _step_tim;
};

// Stepper delays configuration
// All delays are in nanoseconds
class delays
{
public:
	constexpr delays(uint32_t step_len, uint32_t step_space, uint32_t dir_setup,
			uint32_t dir_hold) :
			_step_len(step_len), _step_space(step_space), _dir_setup(dir_setup), _dir_hold(
					dir_hold)
	{
	}

	uint32_t const _step_len;
	uint32_t const _step_space;
	uint32_t const _dir_setup;
	uint32_t const _dir_hold;

};

// Stepper mechanics configuration
// FIXME: implement...

// Stepper motor controller
class controller
{
	friend void TIM4_IRQHandler();
public:
	enum State
	{
		Stopped, Accelerating, Slewing, Decelerating
	};
public:
	controller(hw&& hw, delays&& delays) :
			_step(0), _hw(hw), _delays(delays)
	{
	}

	void initialize();

	bool move_to(int32_t position, uint32_t max_speed = 0);
	bool move(int32_t speed);
	bool stop();

	// Spindle synchronized motion. Start moving when index pulse is received with the speed
	// define by given TPI.
	bool move_sync_tpi(uint32_t tpi);

	// Step complete
	inline void step_completed()
	{
		update();
	}

	// Step signal pulse complete
	inline void step_pulsed()
	{
		TIM_ClearITPendingBit(_hw._pulse_tim, TIM_IT_Update);
		_offset++;
	}

private:
	// Hardware setup
	void setup_port();
	void setup_pulse_timer();
	void setup_step_timer();
	void start_stepgen();

private:
	// Pulse generation control
	volatile uint32_t _step;

	// Update state machine and load new delay into timer
	void update();

private:
	hw const _hw; // Hardware configuration
	delays const _delays; // Delays required by the stepper driver

	// Current state

	// Offset against base position current move started on
	// Note that it is always positive. The current position is
	// _dir ? (_base + _offset : _base - _offset)
	volatile uint32_t _offset;
};
}

#endif /* __STEPPER_HPP */
