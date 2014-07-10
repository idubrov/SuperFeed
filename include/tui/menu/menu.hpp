#ifndef __MENU_HPP
#define __MENU_HPP

#include <cstddef>
#include <initializer_list>

#include "tui/console.hpp"

namespace tui
{
namespace menu
{
template<typename ... Items> class menu_items;

template<>
class menu_items<>
{
public:
	void print_label(unsigned)
	{
		assert_param(0);
	}
	constexpr static unsigned size = 0;
};

template<typename Item, typename ... Rest>
class menu_items<Item, Rest...>
{
public:
	menu_items(Item& first, Rest&... rest) :
			_first(first), _rest(rest...)
	{
	}

	void activate(unsigned i)
	{
		if (i > 0)
			_rest.activate(i - 1);
		else
			_first.run();
	}

	void print_label(unsigned i)
	{
		if (i > 0)
			_rest.print_label(i - 1);
		else
			_first.print_label();

	}

	constexpr static unsigned size = 1 + menu_items<Rest...>::size;
private:
	Item& _first;
	menu_items<Rest...> _rest;
};

template<typename ... Items>
class menu
{
public:
	menu(tui::console& console, menu_items<Items...> menu_items) :
			_console(console), _menu_items(menu_items), _scroll(0)
	{
	}

	void run()
	{
		auto state = _console.guard_state();

		_console.set_encoder_limit(100);
		auto& lcd = _console.lcd();

		redraw();
		while (true)
		{

		}
	}
private:
	void redraw()
	{
		auto& lcd = _console.lcd();
		lcd.clear();
		unsigned count = menu_items<Items...>::size;
		for (unsigned i = 0; i < 4 && i + _scroll < count; i++)
		{
			lcd << hw::lcd::position(0, i) << "  ";
			_menu_items.print_label(i + _scroll);
		}

	}
private:
	tui::console& _console;

	menu_items<Items...> _menu_items;
//	menu_item * const * const _menu_items;
//	size_t const _count;
//
	unsigned _scroll;
};
}
}

#endif /* __MENU_HPP */
