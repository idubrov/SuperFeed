#include "tui/widgets.hpp"

using namespace ::hw;

uint16_t tui::spinner(console& console, uint8_t x, uint8_t y, uint16_t min,
		uint16_t max, uint16_t current)
{
	assert_param(min < max);

	auto& lcd = console.lcd();
	auto state(console.guard_state());

	console.set_encoder_limit(max - min);
	lcd << lcd::position(x, y) << current;
	lcd.display(lcd::DisplayOn, lcd::CursorOn, lcd::BlinkOff);
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::EncoderPressed)
			break;

		if (ev.kind == console::EncoderMove)
		{
			current = ev.position + 1;
			lcd << lcd::position(x, y) << "  ";
			lcd << lcd::position(x, y) << current;
		}
	}

	lcd.display(lcd::DisplayOn, lcd::CursorOff, lcd::BlinkOff);
	return current;
}
