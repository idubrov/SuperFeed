#ifndef __STEPPER_HPP
#define __STEPPER_HPP

#include <atomic>

#include "hw/driver.hpp"
#include "hw/eeprom.hpp"
#include "stepgen.hpp"
#include "util.hpp"

namespace stepper
{
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
	controller(hw::eeprom& eeprom, hw::driver const& driver, delays&& delays) : eeprom(eeprom),
			_driver(driver), _delays(delays), _stepgen(::cfg::stepper::TicksPerSec), _stop(
					false)
	{
	}

	void reset();

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

	/// Print state for debugging purposes
	template<typename S>
	S const& dump(S const& sink) const
	{
		sink << "S:" << _stepgen.step();
		sink << " T:" << _stepgen.target_step();
		sink << " Sp:" << _stepgen.speed() << "TSp: " << _stepgen.target_delay();
		return sink;
	}
private:
	uint32_t load_delay();

private:
	hw::eeprom& eeprom; // Settings
	hw::driver const& _driver; // Low-level driver control
	delays const _delays; // Delays required by the stepper driver

	// Current state
	stepgen::stepgen _stepgen;

	// Stop signal
	std::atomic_bool _stop;
};
}

template<typename S>
inline S const& operator<<(S const& l, stepper::controller const& ctrl)
{
	ctrl.dump(l);
	return l;
}

#endif /* __STEPPER_HPP */
