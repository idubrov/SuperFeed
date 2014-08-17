#include "tui/menu/powerfeed.hpp"

bool tui::menu::powerfeed::activate(tui::console& console, unsigned)
{
	auto state = console.guard_state();
	unsigned ipm = 1;

	auto& lcd = console.lcd();
	lcd << hw::lcd::clear();
	console.set_encoder_limit(500);
	stepper.reset();
	driver.enable();
	driver.release();

	print_ipm(lcd, ipm);
	print_position(lcd);
	print_status(lcd);

	set_ipm(ipm);
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::EncoderMove)
		{
			ipm = ev.position + 1;
			print_ipm(lcd, ipm);
			set_ipm(ipm);
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

		print_position(lcd);
		print_status(lcd);
	}

	stepper.stop();
	while (!stepper.is_stopped())
		;

	driver.reset();
	driver.disable();
	return true;
}
