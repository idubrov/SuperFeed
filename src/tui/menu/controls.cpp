#include <cstring>

#include "tui/menu/controls.hpp"
#include "simstream.hpp"

using namespace hw;

bool tui::menu::spinner::activate(console& console, unsigned y)
{
	uint16_t x = 4 + std::strlen(label);

	uint8_t b = util::digits(max) + 1;
	auto& lcd = console.lcd();
	auto state(console.guard_state());

	console.set_encoder_limit(max - min + 1);

	lcd << lcd::position(x, y) << blanks(b);
	lcd << lcd::position(x, y) << value << (char)3;
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::ButtonPressed
				&& ev.key == console::EncoderButton)
			break;

		if (ev.kind == console::EncoderMove)
		{
			value = ev.position + 1;
			lcd << lcd::position(x, y) << blanks(b);
			lcd << lcd::position(x, y) << value << (char)3;
		}
	}

	uint16_t current = def_value;
	eeprom.read(tag, current);
	if (current != value)
	{
		eeprom.write(tag, value);
	}
	return true;
}
