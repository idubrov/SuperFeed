#include "tui/menu/menu.hpp"

void tui::menu::menu_base::redraw(tui::console& console)
{
	auto& lcd = console.lcd();
	lcd.clear();

	if (scroll > 0)
	{
		lcd << hw::lcd::position(0, 0) << console::UpArrowCharacter;
	}

	unsigned selected = selected_item(console);
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

bool tui::menu::menu_base::activate(tui::console& console, unsigned)
{
	auto state = console.guard_state();
	console.set_encoder_limit(actions_count);
	offset = 0;

	redraw(console);
	while (true)
	{
		auto ev = console.read();
		bool moved = false;
		if (ev.kind == console::EncoderMove)
		{
			moved = true;
		}
		else if (ev.kind == console::ButtonPressed
				&& ev.key == console::RightButton)
		{
			offset++;
			if (offset == actions_count)
				offset = 0;
			moved = true;
		}
		else if (ev.kind == console::ButtonPressed
				&& ev.key == console::LeftButton)
		{
			if (offset == 0)
				offset = actions_count;
			offset--;
			moved = true;
		}
		else if (ev.kind == console::ButtonPressed
				&& ev.key == console::SelectButton)
		{
			unsigned selected = selected_item(console);
			if (!activate_action(console, selected, selected - scroll))
				break; // false returned -- exit from the menu
			redraw(console);
		}
		if (moved)
		{
			unsigned selected = selected_item(console);
			if (selected < scroll)
			{
				scroll = selected;
				redraw(console);
			}
			else if (selected >= scroll + console.lcd().lines())
			{
				scroll = selected - console.lcd().lines() + 1;
				redraw(console);
			}

			for (unsigned i = 0; i < 4; i++)
			{
				bool current = (scroll + i == selected);
				console.lcd() << hw::lcd::position(1, i)
						<< (current ? '>' : ' ');
			}
		}
	}
	return true;
}
