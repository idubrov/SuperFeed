#include "tui/menu/menu.hpp"

//void tui::menu::menu::redraw(tui::console& console)
//{
//	auto& lcd = console.lcd();
//	lcd.clear();
//
//	if (scroll > 0)
//	{
//		lcd << hw::lcd::position(0, 0) << console::UpArrowCharacter;
//	}
//
//	unsigned selected = console.enc_position();
//	for (unsigned i = 0; i < lines_count && i + scroll < actions_count; i++)
//	{
//		lcd << hw::lcd::position(1, i);
//		bool current = (scroll + i == selected);
//		lcd << (current ? '>' : ' ');
//		actions[i + scroll]->print_label(console);
//	}
//
//	if (scroll + lines_count < actions_count)
//	{
//		lcd << hw::lcd::position(0, lines_count - 1)
//				<< console::DownArrowCharacter;
//	}
//}
//
//void tui::menu::menu::activate(tui::console& console)
//{
//	auto state = console.guard_state();
//	console.set_encoder_limit(actions_count);
//
//	redraw(console);
//	while (true)
//	{
//		auto ev = console.read();
//		if (ev.kind == console::EncoderMove)
//		{
//			unsigned selected = ev.position;
//			if (selected < scroll)
//			{
//				scroll = selected;
//				redraw(console);
//			}
//			else if (selected >= scroll + lines_count)
//			{
//				scroll = selected - lines_count + 1;
//				redraw(console);
//			}
//
//			for (unsigned i = 0; i < 4; i++)
//			{
//				bool current = (scroll + i == selected);
//				console.lcd() << hw::lcd::position(1, i)
//						<< (current ? '>' : ' ');
//			}
//		}
//		else if (ev.kind == console::ButtonPressed
//				&& ev.key == console::EncoderButton)
//		{
//			unsigned selected = console.enc_position();
//			actions[selected]->activate(console);
//			redraw(console);
//		}
//	}
//}
