#include "tui/menu/powerfeed.hpp"

bool tui::menu::powerfeed::activate(tui::console& console, unsigned)
{
	auto state = console.guard_state();

	uint32_t const microsteps = settings::Microsteps.get(eeprom);
	uint32_t const nom = settings::GearNominator.get(eeprom);
	uint32_t const denom = settings::GearNominator.get(eeprom);
	uint32_t const ratio = ((200 * microsteps * nom) << 8) / (denom * 60);

	constexpr int padding = util::digits(std::numeric_limits<uint32_t>::max());
	unsigned rpm = 1;
	bool left = false;

	auto& lcd = console.lcd();
	lcd << hw::lcd::clear();
	console.set_encoder_limit(160);
	stepper.reset();
	driver.enable();
	driver.release();

	lcd << hw::lcd::position(0, 0) << "Speed: " << rpm << " RPM   ";
	stepper.set_speed(rpm * ratio);
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::EncoderMove)
		{
			rpm = ev.position + 1;
			if (!stepper.set_speed(rpm * ratio))
			{
				lcd << hw::lcd::clear() << "Too fast!";
				stepper.stop();
				while (1)
					;
			}
			lcd << hw::lcd::position(7, 0) << rpm << " RPM   ";
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

		uint32_t speed = stepper.current_speed();
		lcd << hw::lcd::position(0, 1) << format<10>((speed + 128) / ratio, padding);
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
