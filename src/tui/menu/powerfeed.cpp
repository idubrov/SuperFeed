#include "tui/menu/powerfeed.hpp"

namespace
{
constexpr int32_t LEFTMOST = ::std::numeric_limits<int32_t>::min();
constexpr int32_t RIGHTMOST = ::std::numeric_limits<int32_t>::max();
}

void tui::menu::powerfeed::update_display(state& s)
{
	uint32_t speed = (s.conv.pulse_to_mils(stepper.current_speed()) * 60) >> 8;

	// Selected IPM
	s.lcd << hw::lcd::position(0, 1) << "Feed ";
	if (ipr)
		s.lcd << format<10, Right>(ipr_feed, 4) << " IPR (*)";
	else
		s.lcd << format<10, Right>(ipm_feed, 4, 1) << " IPM (*)";

	s.lcd << hw::lcd::position(0, 2) << "Rapid "
			<< format<10, Right>(ipm_rapid, 4, 1) << " IPM";

	// Current position
	int32_t mills = s.conv.pulse_to_mils(stepper.current_position());
	s.lcd << hw::lcd::position(0, 0) << "Pos " << format<10, Right>(mills, 6, 3)
			<< blanks(10);
	if (stepper.is_stopped())
	{
		s.lcd << hw::lcd::position(0, 3) << "Press A to 0 pos" << blanks(4);
	}
	else
	{
		char const* status =
				stepper.current_direction() ?
						(rapid ? ">>>>>>>>" : "> > > > ") :
						(rapid ? "<<<<<<<<" : "< < < < ");
		s.lcd << hw::lcd::position(0, 3) << status;
		s.lcd << " Speed "
				<< format<10, Right>((speed + 50) / 100 /* rounding */, 4, 1);
	}
}

bool tui::menu::powerfeed::activate(tui::console& console, unsigned)
{
	// Reload rapid from the settings
	ipm_rapid = settings::RapidFeed.get(eeprom);
	ipm_feed = settings::LastFeedIPM.get(eeprom);
	ipr_feed = settings::LastFeedIPR.get(eeprom);
	ipr = settings::LastFeedMode.get(eeprom);

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
		bool stopped = stepper.is_stopped();
		bool should_stop = true;

		if (ev.kind == console::EncoderMove)
		{
			auto range = util::ranged<uint32_t>(ipm_feed, 1, ipm_rapid);
			range += ev.delta;
			ipm_feed = range.get();
			update_ipm(st);
			update_display(st);
			continue;
		}
		else if (ev.kind == console::ButtonPressed
				&& ev.key == console::BackButton)
		{
			break;
		}
		else if (ev.kind == console::ButtonPressed && ev.key == 'A' && stopped)
		{
			stepper.zero_position();
			update_display(st);

		}
		else if (ev.kind == console::ButtonPressed
				&& (ev.key == console::LeftButton
						|| ev.key == console::RightButton))
		{
			bool dir = (ev.key == console::RightButton);
			if (stopped || (stepper.current_direction() == dir && !rapid))
			{
				// Either already moving in given direction or stopped
				rapid = !stopped;
				update_ipm(st);
				if (stopped)
					stepper.move_to(dir ? RIGHTMOST : LEFTMOST);
				should_stop = false;
			}

		}
		else if (ev.kind == console::ButtonPressed && stopped
				&& (ev.key >= '0' && ev.key <= '9'))
		{
			int32_t value = read_offset(console, ev);

			if (ev.kind == console::ButtonPressed
					&& (ev.key == console::SelectButton
							|| ev.key == console::LeftButton
							|| ev.key == console::RightButton))
			{
				// Note: we convert current position to mills to avoid situation when 1 mil + 1 mil
				// is not equal to 2 mills due to the rounding errors.
				int32_t mills = conv.pulse_to_mils(stepper.current_position());
				if (ev.key == console::LeftButton)
					mills = mills - value;
				else if (ev.key == console::RightButton)
					mills = mills + value;
				else
					mills = value;

				int32_t pulses = conv.mils_to_pulse(mills);
				rapid = false;
				update_ipm(st);
				stepper.move_to(pulses);
				should_stop = false;
			}
		}
		else if (ev.kind == console::ButtonPressed && stopped && ev.key == '*')
		{
			ipr = !ipr;
		}

		if (ev.kind == console::ButtonPressed && should_stop)
			stepper.stop();

		update_display(st);
	}

	stepper.stop();
	while (!stepper.is_stopped())
		;

	driver.reset();
	driver.disable();

	// Update last feed
	settings::LastFeedIPM.set(eeprom, ipm_feed);
	settings::LastFeedIPR.set(eeprom, ipr_feed);
	settings::LastFeedMode.set(eeprom, ipr);
	return true;
}

void tui::menu::powerfeed::print_offset(hw::lcd::HD44780 const& lcd, int offset)
{
	lcd << hw::lcd::position(5, 3) << blanks(5);
	lcd << hw::lcd::position(5, 3) << offset;
	lcd.display(hw::lcd::DisplayOn, hw::lcd::CursorOn, hw::lcd::BlinkOn);
}

int32_t tui::menu::powerfeed::read_offset(tui::console& console,
		tui::console::Event& ev)
{
	auto& lcd = console.lcd();
	lcd << hw::lcd::position(0, 3) << "Goto " << blanks(15);
	int32_t value = 0;
	while (1)
	{
		if (ev.kind == console::ButtonPressed)
		{
			if (ev.key == console::DeleteButton)
			{
				value /= 10;
				if (value == 0)
					break;
				print_offset(lcd, value);
			}
			else if (ev.key >= '0' && ev.key <= '9')
			{
				uint32_t newValue = value * 10 + (ev.key - '0');
				if (newValue <= 20000) // FIXME: Soft limit
				{
					value = newValue;
					print_offset(lcd, value);
				}
			}
			else if (ev.key == '*')
			{
				value = -value;
				print_offset(lcd, value);
			}
			else
			{
				break;
			}
		}

		// Read next event
		ev = console.read();
	}

	lcd.display(hw::lcd::DisplayOn, hw::lcd::CursorOff, hw::lcd::BlinkOff);
	lcd << hw::lcd::position(0, 2) << blanks(6);
	return value;
}
