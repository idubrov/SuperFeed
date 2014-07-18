#include "tui/menu/powerfeed.hpp"

bool tui::menu::powerfeed::activate(tui::console& console, unsigned)
{
	constexpr int padding = util::digits(std::numeric_limits<uint32_t>::max());
	auto& lcd = console.lcd();
	lcd << hw::lcd::clear();


	stepper.reset();
	if (!stepper.set_speed(1000 << 8))
	{
		lcd << "Too fast!";
		while (1)
			;
	}
	lcd << hw::lcd::position(0, 0) << "Position: ";
	lcd << hw::lcd::position(0, 1) << "Speed: ";

	int32_t target = 10000;
	stepper.move_to(target);
	while (true)
	{
		lcd << hw::lcd::position(10, 0)
				<< format<10>(stepper.current_position(), padding);
		lcd << hw::lcd::position(7, 1)
				<< format<10>(stepper.current_speed(), padding);

		auto ev = console.read();
		if (ev.kind == console::ButtonPressed && ev.key == '#')
			break;

		if (stepper.is_stopped() && ev.kind == console::ButtonPressed
				&& ev.key == console::EncoderButton)
		{
			target = -target;
			stepper.move_to(target);
		}

	}
	return true;
}
