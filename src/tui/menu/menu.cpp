#include "tui/menu/menu.hpp"

void tui::menu::menu_base::redraw(tui::console& console)
{
	auto& lcd = console.lcd();
	lcd.clear();

	if (scroll > 0)
	{
		lcd << hw::lcd::position(0, 0) << console::UpArrowCharacter;
	}

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
	selected = 0;

	redraw(console);
	while (true)
	{
		auto ev = console.read();
		bool moved = false;
		if (ev.kind == console::EncoderMove)
		{
			auto ranged = util::ranged<unsigned>(selected, 0, actions_count - 1);
			ranged += ev.delta;
			selected = ranged.get();
			moved = true;
		}
		else if (ev.kind == console::ButtonPressed
				&& ev.key == console::RightButton)
		{
			selected++;
			if (selected == actions_count)
				selected = 0;
			moved = true;
		}
		else if (ev.kind == console::ButtonPressed
				&& ev.key == console::LeftButton)
		{
			if (selected == 0)
				selected = actions_count;
			selected--;
			moved = true;
		}
		else if (ev.kind == console::ButtonPressed
				&& ev.key == console::SelectButton)
		{
			if (!activate_action(console, selected, selected - scroll))
				break;
			redraw(console);
		}
		else if (ev.kind == console::ButtonPressed && ev.key == console::BackButton)
		{
			break;
		}
		else if (ev.kind == console::ButtonPressed)
		{
			auto ptr = std::strchr(hotkeys, ev.key);
			if (ptr != nullptr)
			{
				// Make item current
				unsigned selected = ptr - hotkeys;
				if (!activate_action(console, selected, 0)) // FIXME: we are giving wrong row here!
					break;
				redraw(console);
			}
		}
		if (moved)
		{
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
