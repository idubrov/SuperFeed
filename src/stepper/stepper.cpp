#include <cstdint>

#include "settings.hpp"
#include "stepper/stepper.hpp"

void stepper::controller::reset()
{
	// Reload delays from the settings
	step_len_ticks = hw::driver::ns2ticks(settings::StepLen.get(eeprom));
	step_space_ticks = hw::driver::ns2ticks(settings::StepSpace.get(eeprom));
	dir_setup_ns = settings::DirectionSetup.get(eeprom);
	dir_hold_ns = settings::DirectionHold.get(eeprom);
	reverse = settings::Reverse.get(eeprom);

	// FIXME: check return value
	uint16_t accel = settings::Acceleration.get(eeprom);
	uint16_t microsteps = settings::Microsteps.get(eeprom);
	stepgen.set_acceleration((static_cast<uint32_t>(accel) * microsteps) << 8);

	// FIXME: reset position
}

bool stepper::controller::move_to(int32_t target)
{
	if (!driver.check_stopped())
		return false;

	int32_t pos = update_position();
	uint32_t delta;
	if (pos < target)
	{
		delta = target - pos;
		set_direction(true);
	}
	else if (pos > target)
	{
		delta = pos - target;
		set_direction(false);
	}
	else
	{
		// Nothing to do!
		return true;
	}
	stepgen.set_target_step(base_step + delta);
	stop_requested = false;

	// Load first delay into ARR/CC1, if not stopped
	if (load_delay() == 0)
	{
		// Not making even a single step
		return false;
	}

	driver.manual_update();

	// Load second delay into ARR & CC1.
	bool is_last = load_delay() == 0;

	// Start pulse generation
	driver.start(is_last);

	return true;
}

uint32_t stepper::controller::load_delay()
{
	uint32_t delay = stepgen.next();
	if (delay != 0)
	{
		// Load new step into ARR, start pulse at the end
		uint32_t d = (delay + 128) >> 8; // Delay is in 16.8 format
		driver.set_delay(d, step_len_ticks);
	}
	else
	{
		// Load idle values. This is important to do on the last update
		// when timer is switched into one-pulse mode.
		driver.set_delay(0, step_len_ticks);
	}
	return delay;
}

void stepper::controller::step_completed()
{
	if (driver.check_stopped())
	{
		// If timer is stopped, do nothing!
		return;
	}

	if (stop_requested)
	{
		stepgen.set_target_step(0);
		stop_requested = false;
	}

	if (load_delay() == 0)
	{
		// Stop on the next update, one pulse mode
		// Note that load_delay() should have already loaded ARR and
		// CCR1 with idle values.
		driver.set_last_pulse();
	}
}
