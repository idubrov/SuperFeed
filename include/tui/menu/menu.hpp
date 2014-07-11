#ifndef __MENU_HPP
#define __MENU_HPP

#include <utility>
#include <tuple>
#include <type_traits>

#include "util.hpp"
#include "tuple_util.hpp"
#include "tui/console.hpp"

namespace tui
{
namespace menu
{
namespace traits
{

template<typename A>
constexpr auto has_print_label(
		A*) -> decltype(((A*)nullptr)->print_label(*(tui::console*)nullptr),
				bool())
{
	return true;
}
template<typename A>
constexpr bool has_print_label(...)
{
	return false;
}

template<typename A>
constexpr auto has_activate(
		A*) -> decltype(((A*)nullptr)->activate(*(tui::console*)nullptr), bool())
{
	return true;
}
template<typename A>
constexpr bool has_activate(...)
{
	return false;
}

constexpr bool assert_valid_actions()
{
	return true;
}

template<typename H, typename ... T>
constexpr bool assert_valid_actions(H*, T*... tail)
{
	static_assert(has_print_label<H>(nullptr),
			"Not a valid action: doesn't have print_label(tui::console&) has_print_label");
	static_assert(has_activate<H>(nullptr),
			"Not a valid action: doesn't have activate(tui::console&) activate");
	return assert_valid_actions(tail...);
}
}

template<typename ... Actions>
class menu
{
	template<typename I, typename ...Param>
	struct Print
	{
		static void apply(I& item, Param&&... param)
		{
			item.print_label(param...);
		}
	};

	template<typename I, typename ... Param>
	struct Activate
	{
		static void apply(I& item, Param&&... param)
		{
			item.activate(param...);
		}
	};

	typedef std::tuple<Actions&&...> actions_tuple;
	constexpr static std::size_t actions_count =
			std::tuple_size<actions_tuple>::value;
	constexpr static unsigned lines_count = 4;
public:
	menu(tui::console& console, Actions&&... actions) :
			console(console), actions(std::forward<Actions>(actions)...), scroll(
					0)
	{
		static_assert(traits::assert_valid_actions<
				typename std::remove_reference<Actions>::type...>(
						(typename std::remove_reference<Actions>::type*)nullptr...),
				"Not a valid action list");
	}

	void run()
	{
		auto state = console.guard_state();
		console.set_encoder_limit(actions_count);

		redraw();
		while (true)
		{
			auto ev = console.read();
			if (ev.kind == console::EncoderMove)
			{
				unsigned selected = ev.position;
				if (selected < scroll)
				{
					scroll = selected;
					redraw();
				}
				else if (selected >= scroll + lines_count)
				{
					scroll = selected - lines_count + 1;
					redraw();
				}

				for (unsigned i = 0; i < 4; i++)
				{
					bool current = (scroll + i == selected);
					console.lcd() << hw::lcd::position(1, i)
							<< (current ? '>' : ' ');
				}
			}
		}
	}
private:
	void redraw()
	{
		auto& lcd = console.lcd();
		lcd.clear();

		if (scroll > 0)
		{
			lcd << hw::lcd::position(0, 0) << console::UpArrowCharacter;
		}

		unsigned selected = console.enc_position();
		for (unsigned i = 0; i < lines_count && i + scroll < actions_count; i++)
		{
			lcd << hw::lcd::position(1, i);
			bool current = (scroll + i == selected);
			lcd << (current ? '>' : ' ');
			util::make_tuple_applicator(actions, console).template apply_to<
					Print>(i + scroll);
		}

		if (scroll + lines_count < actions_count)
		{
			lcd << hw::lcd::position(0, lines_count - 1) << console::DownArrowCharacter;
		}
	}
private:
	tui::console& console;

	actions_tuple actions;
	unsigned scroll;
};

template<typename ... Actions>
menu<Actions...> create(tui::console& console, Actions&&... actions)
{
	return menu<Actions...>(console, std::forward<Actions>(actions)...);
}
}
}

#endif /* __MENU_HPP */
