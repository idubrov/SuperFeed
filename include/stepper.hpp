#ifndef __STEPPER_HPP
#define __STEPPER_HPP

#include "config.hpp"
#include "util.hpp"
#include "stepgen.hpp"

namespace stepper
{
// Stepper hardware configuration (pins and their timers)
class hw
{
public:
	constexpr hw(GPIO_TypeDef* port, uint16_t step_pin, uint16_t dir_pin,
			uint16_t enable_pin, uint16_t reset_pin, TIM_TypeDef* timer) :
			_port(port), _step_pin(step_pin), _dir_pin(dir_pin), _enable_pin(
					enable_pin), _reset_pin(reset_pin), _timer(timer)
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
	TIM_TypeDef* const _timer;
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
public:
	enum State
	{
		Stopped, Accelerating, Slewing, Decelerating
	};
public:
	controller(hw&& hw, delays&& delays) :
			_hw(hw), _delays(delays), _stepgen(::cfg::stepper::TicksPerSec), _stop(
					false)
	{
	}

	void initialize();

	// Step complete, should be called from the timer update IRQ
	void step_completed();

	void stop()
	{
		_stop = true;
	}

	/// Move to given position. Note that no new move commands will be accepted
	/// while stepper is running. However, other target parameter, target speed,
	/// could be changed any time.
	bool move(uint32_t steps);

	inline void set_speed(uint32_t speed)
	{
		_stepgen.set_target_speed(speed);
	}

	template<typename S>
	S const& dump(S const& sink) const
	{
		sink << "S:" << _stepgen.step();
		sink << " T:" << _stepgen.target_step();
		sink << " Sp:" << _stepgen.speed() << "TSp: " << _stepgen.target_delay();
		return sink;
	}
private:
	// Hardware setup
	void setup_port();
	void setup_timer();

	uint32_t load_delay();

private:
	hw const _hw; // Hardware configuration
	delays const _delays; // Delays required by the stepper driver

	// Current state
	stepgen::stepgen _stepgen;

	// Stop signal
	volatile bool _stop;
};
}

template<typename S>
inline S const& operator<<(S const& l, stepper::controller const& ctrl)
{
	ctrl.dump(l);
	return l;
}

#endif /* __STEPPER_HPP */
