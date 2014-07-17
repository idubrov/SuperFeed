#ifndef __STEPPER_HPP
#define __STEPPER_HPP

#include <atomic>

#include "settings.hpp"
#include "hw/core.hpp"
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
			eeprom(eeprom), driver(driver), step_len_ticks(0), step_space_ticks(0), dir_setup_ns(
					0), dir_hold_ns(0), stepgen(hw::driver::Frequency), direction(
			true), base_step(0), position(0), stop_requested(
			false)
	{
	}

	void reset();

	// Step complete, should be called from the timer update IRQ
	void step_completed();

	void stop()
	{
		stop_requested = true;
	}

	inline bool is_stopped()
	{
		return driver.check_stopped();
	}

	/// Move to given position. Note that no new move commands will be accepted
	/// while stepper is running. However, other target parameter, target speed,
	/// could be changed any time.
	bool move_to(int32_t target);

	inline void set_speed(uint32_t speed)
	{
		stepgen.set_target_speed(speed);
	}

	/// Return current stepper position.
	inline int32_t current_position()
	{
		uint32_t offset = stepgen.current_step() - base_step;
		return position + (direction ? offset : -offset);
	}

	/// Print state for debugging purposes
	template<typename S>
	S const& dump(S const& sink) const
	{
		sink << "S:" << stepgen.step();
		sink << " T:" << stepgen.target_step();
		sink << " Sp:" << stepgen.speed() << "TSp: " << stepgen.target_delay();
		return sink;
	}
private:
	uint32_t load_delay();

	// Incorporate outstanding steps from the stepgen into current position
	inline int32_t update_position()
	{
		uint32_t step = stepgen.current_step();
		uint32_t offset = step - base_step;
		base_step = step;
		if (direction)
			position += offset;
		else
			position -= offset;
		return position;
	}

	inline void set_direction(bool dir)
	{
		if (direction == dir)
			return; // Nothing to do!

		hw::core::delay_ns(dir_setup_ns);
		driver.set_direction(dir);
		direction = dir;
		hw::core::delay_ns(dir_hold_ns);
	}

private:
	// Settings
	hw::eeprom& eeprom;

	// Low-level driver control
	hw::driver const& driver;

	// Delays required by the stepper driver, in ticks
	uint16_t step_len_ticks;
	uint16_t step_space_ticks;
	uint16_t dir_setup_ns;
	uint16_t dir_hold_ns;

	// Current state
	stepgen::stepgen stepgen;bool direction;

	uint32_t base_step;
	int32_t position;

	// Stop signal
	std::atomic_bool stop_requested;
};
}

template<typename S>
inline S const& operator<<(S const& l, stepper::controller const& ctrl)
{
	ctrl.dump(l);
	return l;
}

#endif /* __STEPPER_HPP */
