#include "tui/inputs.hpp"

uint16_t tui::spinner(lcd::HD44780& lcd, input& input, uint8_t x, uint8_t y)
{
	input.get_encoder().limit(9);
	input.get_encoder().position(0);
	input.reset();

	lcd << lcd::position(x, y) << "1 ";

	lcd << lcd::position(x, y);
	lcd.display(lcd::DisplayOn, lcd::CursorOn, lcd::BlinkOff);
	uint16_t pos = 1;
	while (true)
	{
		auto ev = input.read();
		if (ev.kind == input::Nothing)
		{
			util::delay_ms(100);
			continue;
		}
		if (ev.kind == input::EncoderMove)
		{
			pos = ev.position + 1;
			lcd.display(lcd::DisplayOn, lcd::CursorOff, lcd::BlinkOff);
			lcd << lcd::position(x, y) << "  ";
			lcd << lcd::position(x, y) << pos;
			lcd << lcd::position(x, y);
			lcd.display(lcd::DisplayOn, lcd::CursorOn, lcd::BlinkOff);
		}
		if (ev.kind == input::EncoderButton && ev.pressed)
		{
			return pos;
		}
	}

	return 0;
}
