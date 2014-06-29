#include "tui/menu/settings.hpp"
#include "tui/inputs.hpp"
#include <cstring>

using namespace lcd;

const uint16_t settings::Microsteps[] =
{ 1, 2, 4, 8, 10, 16, 32 };
const setting settings::Options[] =
{
{ "Microsteps", 1, 1, Microsteps, sizeof(Microsteps) / sizeof(Microsteps[0]) },
{ "Dummy", 1, 1, nullptr, 0 }
};
const uint16_t settings::OptionsCount = sizeof(Options) / sizeof(Options[0]);

void settings::run()
{
	_lcd << clear();
	redraw();

	while (true)
	{
		auto ev = _input.read();
		if (ev.kind == input::Nothing)
		{
			util::delay_ms(100);
			continue;
		}
		if (ev.kind == input::EncoderMove)
		{
			_selected = ev.position % OptionsCount;
			redraw();
		}
		if (ev.kind == input::EncoderButton && ev.pressed)
		{
			uint8_t x = strlen(Options[_selected].title) + 3;
			tui::spinner(_lcd, _input, x, _selected - _scroll);
		}
	}
}

void settings::redraw()
{
	for (uint16_t y = 0; y < 4; y++)
	{
		_lcd << position(0, y);
		uint16_t option = _scroll + y;
		if (option >= OptionsCount)
			break;

		if (_selected == option)
			_lcd << '>';
		else
			_lcd << ' ';
		_lcd << Options[option].title << ':';
	}
}
