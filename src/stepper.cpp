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
	// Timer is running already, we can't start new moves!
	if (driver.is_running())
		return false;

	// If interrupt is pending, wait until it is cleared.
	// (for instance, we got move command just after last timer overflow
	// and it wasn't processed yet.
	driver.wait_flag();

	stepgen.set_target_step(steps);
	is_stopped = false;

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

void stepper::controller::step_completed()
{
	if (!driver.is_running())
	{
		// If timer is stopped, do nothing!
		return;
	}

	if (is_stopped)
	{
		stepgen.set_target_step(0);
		is_stopped = false;
	}

	if (load_delay() == 0)
	{
		// Stop on the next update, one pulse mode
		// Note that load_delay() should have already loaded ARR and
		// CCR1 with idle values.
		driver.set_last_pulse();
	}
}
