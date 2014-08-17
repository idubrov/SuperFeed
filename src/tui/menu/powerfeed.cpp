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

	lcd << hw::lcd::position(0, 0) << "Speed: " << ipm << " IPM   ";
	print_position(lcd);
	set_ipm(ipm);
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::EncoderMove)
		{
			ipm = ev.position + 1;
			set_ipm(ipm);
			lcd << hw::lcd::position(7, 0) << ipm << " IPM   ";
		}

		if (ev.kind == console::ButtonPressed)
		{
			if (stepper.is_stopped())
			{
				if (ev.key == console::LeftButton)
				{
					stepper.move_to(::std::numeric_limits<int32_t>::min());
					lcd << hw::lcd::position(0, 3) << "<<<<<<<<<";
				}
				else if (ev.key == console::RightButton)
				{
					stepper.move_to(::std::numeric_limits<int32_t>::max());
					lcd << hw::lcd::position(0, 3) << ">>>>>>>>>";
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
		if (stepper.is_stopped())
			lcd << hw::lcd::position(0, 3) << "         ";
	}

	stepper.stop();
	while (!stepper.is_stopped())
		;

	driver.reset();
	driver.disable();
	return true;
}
