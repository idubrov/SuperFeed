#include "tui/menu/inputs.hpp"

using namespace ::hw;
using namespace ::tui;
using namespace ::tui::menu;

bool inputs::activate(tui::console& console, unsigned)
{
	auto& lcd = console.lcd();
	auto encoder = util::ranged(1, 1, 100, false);

	lcd.clear();
	lcd << lcd::position(0, 0) << "Last event: ";
	lcd << lcd::position(0, 3) << "  Back" << blanks(13) << tui::console::BackCharacter;
	while (true)
	{
		auto ev = console.read();

		lcd << lcd::position(0, 2) << "Encoder: " << encoder.get() << " of 100    ";
		if (ev.kind == console::Nothing)
		{
			core::delay_ms(100);
			continue;
		}

		lcd << lcd::position(0, 1);
		if (ev.kind == console::EncoderMove)
		{
			lcd << "  encoder moved     ";
			encoder += ev.delta;
		}
		else if (ev.kind == console::ButtonPressed)
		{
			lcd << "  pressed " << ev.key << blanks(9);
			if (ev.key == console::BackButton)
				break;
		}
		else if (ev.kind == console::ButtonUnpressed)
		{
			lcd << "  unpressed " << ev.key << blanks(7);
		}
	}
	return true;
}
