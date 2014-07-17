#include "stepper.hpp"
#include "settings.hpp"

void stepper::controller::reset()
{
	// Reload delays from the settings
	step_len = hw::driver::ns2ticks(settings::StepLen.get(eeprom));
	step_space = hw::driver::ns2ticks(settings::StepSpace.get(eeprom));
	dir_setup = hw::driver::ns2ticks(settings::DirectionSetup.get(eeprom));
	dir_hold = hw::driver::ns2ticks(settings::DirectionHold.get(eeprom));

	// FIXME: check return value
	uint16_t accel = settings::Acceleration.get(eeprom);
	uint16_t microsteps = settings::Microsteps.get(eeprom);
	stepgen.set_acceleration((accel * microsteps) << 8);
}

bool stepper::controller::move(uint32_t steps)
{
	if (!driver.check_stopped())
		return false;

	stepgen.set_target_step(steps);
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
		driver.set_delay(d, step_len);
	}
	else
	{
		// Load idle values. This is important to do on the last update
		// when timer is switched into one-pulse mode.
		driver.set_delay(0, step_len);
	}
	return delay;
}

bool stepper::controller::set_direction(bool dir)
{
	if (direction == dir)
		return true; // Nothing to do!

	if (!driver.check_stopped())
		return false;

	// Accumulate steps from the stepgen
	uint32_t step = stepgen.current_step();
	uint32_t offset = step - base_step;
	base_step = step;
	if (direction)
		total_steps += offset;
	else
		total_steps -= offset;

	direction = dir;
	return true;
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
