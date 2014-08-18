#include "tui/menu/powerfeed.hpp"

void tui::menu::powerfeed::update_display(state& s)
{
	// Selected IPM
	s.lcd << hw::lcd::position(0, 0) << "Speed: " << format<>(ipm, 4, 1)
			<< ", fast: " << format<>(ipm, 4, 1);

	// Current position
	int32_t mills = s.conv.pulse_to_mils(stepper.current_position());
	s.lcd << hw::lcd::position(0, 1) << "Position: ";
	s.lcd << format<10, Right>(mills, 8, 3);

	// Current state and speed
	if (stepper.is_stopped())
	{
		s.lcd << hw::lcd::position(0, 3) << "                    ";
		return;
	}

	s.lcd << hw::lcd::position(0, 3)
			<< (stepper.current_direction() ? "<<<<<<<< " : ">>>>>>>> ");
	uint32_t speed = (s.conv.pulse_to_mils(stepper.current_speed()) * 60) >> 8;
	s.lcd << format<10, Right>((speed + 50) / 100 /* rounding */, 8, 1);
}

bool tui::menu::powerfeed::activate(tui::console& console, unsigned)
{
	auto& lcd = console.lcd();
	state st =
	{ console.lcd(), eeprom };
	conversions::converter conv(eeprom);


	lcd << hw::lcd::clear();
	stepper.reset();
	driver.enable();
	driver.release();

	update_display(st);
	set_ipm(ipm, st);
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::EncoderMove)
		{
			auto range = util::ranged<uint32_t>(ipm, 1, 500);
			range += ev.delta;
			ipm = range.get();
			set_ipm(ipm, st);
		}

		if (ev.kind == console::ButtonPressed)
		{
			if (stepper.is_stopped())
			{
				if (ev.key == console::LeftButton)
					stepper.move_to(::std::numeric_limits<int32_t>::min());
				else if (ev.key == console::RightButton)
					stepper.move_to(::std::numeric_limits<int32_t>::max());
			}
			else
				stepper.stop();

			if (ev.key == '#')
				break;
		}

		update_display(st);
	}

	stepper.stop();
	while (!stepper.is_stopped())
		;

	driver.reset();
	driver.disable();
	return true;
}
