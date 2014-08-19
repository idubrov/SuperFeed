#include "tui/menu/powerfeed.hpp"

namespace
{
constexpr int32_t LEFTMOST = ::std::numeric_limits<int32_t>::min();
constexpr int32_t RIGHTMOST = ::std::numeric_limits<int32_t>::max();
}

void tui::menu::powerfeed::update_display(state& s)
{
	// Selected IPM
	s.lcd << hw::lcd::position(0, 0) << "Feed "
			<< format<10, Right>(ipm_feed, 4, 1) << " Rapid "
			<< format<10, Right>(ipm_rapid, 4, 1);

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

	char const* status =
			stepper.current_direction() ?
					(rapid ? ">>>>>>>> " : "> > > >  ") :
					(rapid ? "<<<<<<<< " : "< < < <  ");
	s.lcd << hw::lcd::position(0, 3) << status;
	uint32_t speed = (s.conv.pulse_to_mils(stepper.current_speed()) * 60) >> 8;
	s.lcd << format<10, Right>((speed + 50) / 100 /* rounding */, 8, 1);
}

bool tui::menu::powerfeed::activate(tui::console& console, unsigned)
{
	// Reload rapid from the settings
	ipm_rapid = settings::RapidFeed.get(eeprom);
	ipm_feed = settings::LastFeed.get(eeprom);

	auto& lcd = console.lcd();
	state st =
	{ console.lcd(), eeprom };
	conversions::converter conv(eeprom);

	lcd << hw::lcd::clear();
	stepper.reset();
	driver.enable();
	driver.release();

	update_display(st);
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::EncoderMove)
		{
			auto range = util::ranged<uint32_t>(ipm_feed, 1, ipm_rapid);
			range += ev.delta;
			ipm_feed = range.get();
			update_ipm(st);
		}

		if (ev.kind == console::ButtonPressed)
		{
			if (ev.key == '#')
				break;

			bool stop = true;
			if (ev.key == console::LeftButton || ev.key == console::RightButton)
			{
				bool dir = (ev.key == console::RightButton);
				bool stopped = stepper.is_stopped();
				if (stopped || (stepper.current_direction() == dir && !rapid))
				{
					// Either already moving in given direction or stopped
					rapid = !stopped;
					update_ipm(st);
					if (stopped)
						stepper.move_to(dir ? RIGHTMOST : LEFTMOST);
					stop = false;
				}
			}
			if (stop)
				stepper.stop();
		}

		update_display(st);
	}

	stepper.stop();
	while (!stepper.is_stopped())
		;

	driver.reset();
	driver.disable();

	// Update last feed
	settings::LastFeed.set(eeprom, ipm_feed);
	return true;
}
