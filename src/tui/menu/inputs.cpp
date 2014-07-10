#include "tui/menu/inputs.hpp"

using namespace ::hw;
using namespace ::tui;
using namespace ::tui::menu;

void inputs::run()
{
	auto state = _console.guard_state();

	_console.set_encoder_limit(100);
	auto& lcd = _console.lcd();

	lcd.clear();
	lcd << lcd::position(0, 0) << "Last event: ";
	lcd << lcd::position(0, 3) << "\xa5 twice to exit";
	bool enc_pressed = false;
	while (true)
	{
		auto ev = _console.read();

		lcd << lcd::position(0, 2) << "Encoder: " << _console.enc_position()
				<< " of 100  ";
		if (ev.kind == console::Nothing)
		{
			core::delay_ms(100);
			continue;
		}

		lcd << lcd::position(0, 1);
		if (ev.kind == console::EncoderMove)
		{
			lcd << "  encoder moved     ";
		}
		else if (ev.kind == console::ButtonPressed)
		{
			lcd << "  pressed " << ev.key << "         ";
			if (ev.key == console::EncoderButton && enc_pressed)
			{
				break;
			}
			enc_pressed = (ev.key == console::EncoderButton);
		}
		else if (ev.kind == console::ButtonUnpressed)
		{
			lcd << "  unpressed " << ev.key << "       ";
		}
	}
}
