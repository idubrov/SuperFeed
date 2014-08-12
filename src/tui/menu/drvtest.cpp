#include "tui/menu/drvtest.hpp"
#include "util.hpp"

using namespace ::hw;

bool tui::menu::rotatetest::activate(tui::console& console, unsigned row)
{

	auto state = console.guard_state();

	driver.enable();
	driver.release();
	stepper.reset();

	uint16_t microsteps = settings::Microsteps.get(eeprom);
	stepper.set_speed((microsteps * 200) << 8);
	int32_t pos = stepper.current_position();
	stepper.move_to(pos + (microsteps * 200));

	auto& lcd = console.lcd();
	lcd << lcd::position(2, row) << "Rotating...       ";
	while (!stepper.is_stopped());

	driver.reset();
	driver.disable();
	return true;
}
