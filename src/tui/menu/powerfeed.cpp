#include "tui/menu/powerfeed.hpp"

bool tui::menu::powerfeed::activate(tui::console& console, unsigned)
{
	auto& lcd = console.lcd();
	state st = { console.lcd(), eeprom };
	conversions::converter conv(eeprom);
	auto state = console.guard_state();
	unsigned ipm = 1;


	lcd << hw::lcd::clear();
	console.set_encoder_limit(500);
	stepper.reset();
	driver.enable();
	driver.release();

	print_ipm(ipm, st);
	print_position(st);
	print_status(st);

	set_ipm(ipm, st);
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::EncoderMove)
		{
			ipm = ev.position + 1;
			print_ipm(ipm, st);
			set_ipm(ipm, st);
		}

		if (ev.kind == console::ButtonPressed)
		{
			if (stepper.is_stopped())
			{
				if (ev.key == console::LeftButton)
				{
					stepper.move_to(::std::numeric_limits<int32_t>::min());
				}
				else if (ev.key == console::RightButton)
				{
					stepper.move_to(::std::numeric_limits<int32_t>::max());
				}
			}
			else
			{
				stepper.stop();
			}

			if (ev.key == '#')
				break;
		}

		print_position(st);
		print_status(st);
	}

	stepper.stop();
	while (!stepper.is_stopped())
		;

	driver.reset();
	driver.disable();
	return true;
}
