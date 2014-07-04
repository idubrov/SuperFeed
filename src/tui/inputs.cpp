#include "tui/inputs.hpp"
#include "tui/console.hpp"

using namespace ::hw;

uint16_t tui::spinner(console& console, uint8_t x, uint8_t y)
{
	auto state = console.get_encoder_state();
	console.set_encoder_limit(9);
	auto& lcd = console.lcd();

	lcd << lcd::position(x, y) << "1 ";
	lcd << lcd::position(x + 1, y);
	lcd.display(lcd::DisplayOn, lcd::CursorOn, lcd::BlinkOff);
	uint16_t pos = 1;
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::Nothing)
		{
			util::delay_ms(100);
			continue;
		}
		if (ev.kind == console::EncoderMove)
		{
			pos = ev.position + 1;
			lcd << lcd::position(x, y) << "  ";
			lcd << lcd::position(x, y) << pos;
		}
		if (ev.kind == console::EncoderButton && ev.pressed)
		{
			lcd.display(lcd::DisplayOn, lcd::CursorOff, lcd::BlinkOff);
			console.set_encoder_state(state);
			return pos;
		}
	}

	console.set_encoder_state(state);
	return 0;
}
