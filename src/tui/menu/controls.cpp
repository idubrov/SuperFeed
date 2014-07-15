#include <cstring>

#include "tui/menu/controls.hpp"
#include "simstream.hpp"

using namespace hw;

bool tui::menu::spinner::activate(console& console, unsigned y)
{
	uint16_t min = 1;
	uint16_t max = 32;
	uint16_t x = 4 + std::strlen(label);

	uint8_t padding = util::digits(max);
	auto& lcd = console.lcd();
	auto state(console.guard_state());

	console.set_encoder_limit(max - min);

	lcd << lcd::position(x, y);
	for (int i = 0; i < padding; i++)
		lcd << ' ';
	lcd << lcd::position(x, y) << value;
	lcd.display(lcd::DisplayOn, lcd::CursorOn, lcd::BlinkOn);
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::ButtonPressed
				&& ev.key == console::EncoderButton)
			break;

		if (ev.kind == console::EncoderMove)
		{
			value = ev.position + 1;
			lcd << lcd::position(x, y);
			for (int i = 0; i < padding; i++)
				lcd << ' ';
			lcd << lcd::position(x, y) << value;
		}
	}

	lcd.display(lcd::DisplayOn, lcd::CursorOff, lcd::BlinkOff);

	uint16_t current = def_value;
	eeprom.read(tag, current);
	if (current != value)
	{
		eeprom.write(tag, value);
	}
	return true;
}
