#ifndef __MENU_HPP
#define __MENU_HPP

#include <utility>
#include <tuple>

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

class menu_base
{
public:
	menu_base(char const* label, std::size_t actions_count) :
			label(label), actions_count(actions_count), scroll(0)
	{
	}
	menu_base(menu_base const&) = default;
	menu_base& operator=(menu_base const&) = default;

	virtual ~menu_base()
	{
	}

	void activate(tui::console& console);
	void print_label(tui::console& console)
	{
		console.lcd() << label;
	}

protected:
	void redraw(tui::console& console);

	virtual void print_actions(tui::console& console) = 0;
	virtual void activate_action(tui::console& console, unsigned idx) = 0;

protected:
	char const* label;
	std::size_t actions_count;
	unsigned scroll;
};

template<typename ... Actions>
class menu: public menu_base
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

	typedef std::tuple<Actions...> actions_tuple;
	constexpr static std::size_t actions_count =
			std::tuple_size<actions_tuple>::value;
	constexpr static unsigned lines_count = 4;
public:
	menu(char const* label, Actions&&... actions) :
			menu_base(label, std::tuple_size<actions_tuple>::value), actions(
					std::forward<Actions>(actions)...)
	{
		static_assert(traits::assert_valid_actions<
				typename std::remove_reference<Actions>::type...>(
						(typename std::remove_reference<Actions>::type*)nullptr...),
				"Not a valid action list");
	}
private:
	void print_actions(tui::console& console)
	{
		for (unsigned i = 0; i < lines_count && i + scroll < actions_count; i++)
		{
			console.lcd() << hw::lcd::position(2, i);
			util::make_tuple_applicator(actions, console).template apply_to<
					Print>(i + scroll);
		}
	}

	void activate_action(tui::console& console, unsigned idx)
	{
		util::make_tuple_applicator(actions, console).template apply_to<Activate>(
				idx);
	}
private:
	actions_tuple actions;
};

template<typename ... Actions>
menu<Actions...> create(char const* label, Actions&&... actions)
{
	return menu<Actions...>(label, std::forward<Actions>(actions)...);
}

}
}

#endif /* __MENU_HPP */
