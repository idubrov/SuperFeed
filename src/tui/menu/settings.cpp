#include <cstring>

#include "tui/menu/settings.hpp"
#include "tui/widgets.hpp"

using namespace ::hw;
using namespace ::tui;

const uint16_t tui::menu::settings::Microsteps[] =
{ 1, 2, 4, 8, 10, 16, 32 };
const tui::menu::setting tui::menu::settings::Options[] =
{
{ "Microsteps", 1, 1, Microsteps, sizeof(Microsteps) / sizeof(Microsteps[0]) },
{ "Dummy", 1, 1, nullptr, 0 },
{ "Dummy2", 1, 1, nullptr, 0 }
};
const uint16_t tui::menu::settings::OptionsCount = sizeof(Options) / sizeof(Options[0]);

bool tui::menu::settings::activate(tui::console& console, unsigned)
{
	auto& lcd = console.lcd();
	lcd << lcd::clear();
	console.set_encoder_limit(sizeof(Options) / sizeof(Options[0]));
	redraw(console);

	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::Nothing)
		{
			core::delay_ms(100);
			continue;
		}
		if (ev.kind == console::EncoderMove)
		{
			_selected = ev.position;
			redraw(console);
		}
		if (ev.kind == console::ButtonPressed && ev.key == console::EncoderButton)
		{
			uint8_t x = strlen(Options[_selected].title) + 3;
			tui::spinner(console, x, _selected - _scroll,
					1, 10, 5);
		}
	}
	return true;
}

void tui::menu::settings::redraw(tui::console& console)
{
	auto& lcd = console.lcd();
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
