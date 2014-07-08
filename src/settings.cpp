#include <cstring>

#include "tui/menu/settings.hpp"
#include "tui/widgets.hpp"

using namespace ::hw;
using namespace ::tui;

const uint16_t settings::Microsteps[] =
{ 1, 2, 4, 8, 10, 16, 32 };
const setting settings::Options[] =
{
{ "Microsteps", 1, 1, Microsteps, sizeof(Microsteps) / sizeof(Microsteps[0]) },
{ "Dummy", 1, 1, nullptr, 0 },
{ "Dummy2", 1, 1, nullptr, 0 }
};
const uint16_t settings::OptionsCount = sizeof(Options) / sizeof(Options[0]);

void settings::run()
{
	auto& lcd = _console.lcd();
	lcd << lcd::clear();
	_console.set_encoder_limit(sizeof(Options) / sizeof(Options[0]));
	redraw();

	while (true)
	{
		auto ev = _console.read();
		if (ev.kind == console::Nothing)
		{
			core::delay_ms(100);
			continue;
		}
		if (ev.kind == console::EncoderMove)
		{
			_selected = ev.position;
			redraw();
		}
		if (ev.kind == console::ButtonPressed)
		{
			uint8_t x = strlen(Options[_selected].title) + 3;
			tui::spinner(_console, x, _selected - _scroll,
					1, 10, 5);
		}
	}
}

void settings::redraw()
{
	auto& lcd = _console.lcd();
	for (uint16_t y = 0; y < 4; y++)
	{
		lcd << lcd::position(0, y);
		uint16_t option = _scroll + y;
		if (option >= OptionsCount)
			break;

		if (_selected == option)
			lcd << '>';
		else
			lcd << ' ';
		lcd << Options[option].title << ':';
	}
}
