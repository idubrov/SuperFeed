#include "tui/menu/menu.hpp"

void tui::menu::menu_base::redraw(tui::console& console)
{
	auto& lcd = console.lcd();
	lcd.clear();

	if (scroll > 0)
	{
		lcd << hw::lcd::position(0, 0) << console::UpArrowCharacter;
	}

	unsigned selected = console.enc_position();
	for (unsigned i = 0; i < lcd.lines() && i + scroll < actions_count; i++)
	{
		lcd << hw::lcd::position(1, i);
		bool current = (scroll + i == selected);
		lcd << (current ? '>' : ' ');
	}
	print_actions(console);

	if (scroll + lcd.lines() < actions_count)
	{
		lcd << hw::lcd::position(0, lcd.lines() - 1)
				<< console::DownArrowCharacter;
	}

}

void tui::menu::menu_base::activate(tui::console& console)
{
	auto state = console.guard_state();
	console.set_encoder_limit(actions_count);

	redraw(console);
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::EncoderMove)
		{
			unsigned selected = ev.position;

			unsigned top_scroll = selected - console.lcd().lines() + 1;
			if (selected < scroll)
			{
				scroll = selected;
				redraw(console);
			}
			else if (top_scroll > scroll)
			{
				scroll = top_scroll;
				redraw(console);
			}

			for (unsigned i = 0; i < 4; i++)
			{
				bool current = (scroll + i == selected);
				console.lcd() << hw::lcd::position(1, i)
						<< (current ? '>' : ' ');
			}
		}
		else if (ev.kind == console::ButtonPressed
				&& ev.key == console::EncoderButton)
		{
			unsigned selected = console.enc_position();
			activate_action(console, selected);
			redraw(console);
		}
	}

}
