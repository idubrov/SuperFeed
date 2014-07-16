#ifndef __STEPPER_HPP
#define __STEPPER_HPP

#include <atomic>

#include "settings.hpp"
#include "hw/driver.hpp"
#include "hw/eeprom.hpp"
#include "stepgen.hpp"
#include "util.hpp"

namespace stepper
{

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
	controller(hw::eeprom& eeprom, hw::driver const& driver) :
			eeprom(eeprom), driver(driver), step_len(0), step_space(0), dir_setup(
					0), dir_hold(0), stepgen(hw::driver::Frequency), is_stopped(
			false)
	{
	}

	void reset();

	// Step complete, should be called from the timer update IRQ
	void step_completed();

	void stop()
	{
		is_stopped = true;
	}

	/// Move to given position. Note that no new move commands will be accepted
	/// while stepper is running. However, other target parameter, target speed,
	/// could be changed any time.
	bool move(uint32_t steps);

	inline void set_speed(uint32_t speed)
	{
		stepgen.set_target_speed(speed);
	}

	/// Print state for debugging purposes
	template<typename S>
	S const& dump(S const& sink) const
	{
		sink << "S:" << stepgen.step();
		sink << " T:" << stepgen.target_step();
		sink << " Sp:" << stepgen.speed() << "TSp: "
				<< stepgen.target_delay();
		return sink;
	}
private:
	uint32_t load_delay();

private:
	// Settings
	hw::eeprom& eeprom;

	// Low-level driver control
	hw::driver const& driver;

	// Delays required by the stepper driver, in ticks
	uint16_t step_len;
	uint16_t step_space;
	uint16_t dir_setup;
	uint16_t dir_hold;

	// Current state
	stepgen::stepgen stepgen;

	// Stop signal
	std::atomic_bool is_stopped;
};
}

template<typename S>
inline S const& operator<<(S const& l, stepper::controller const& ctrl)
{
	ctrl.dump(l);
	return l;
}

#endif /* __STEPPER_HPP */
