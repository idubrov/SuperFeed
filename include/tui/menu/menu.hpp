#ifndef __MENU_HPP
#define __MENU_HPP

#include <utility>
#include <tuple>
#include <cstring>

#include "tuple_util.hpp"
#include "tui/console.hpp"

namespace tui
{
namespace menu
{
class menu_base
{
public:
	menu_base(char const* label, char const* hotkeys, std::size_t actions_count) :
			label(label), hotkeys(hotkeys), hotkeys_count(std::strlen(hotkeys)), actions_count(
					actions_count), scroll(0), selected(0)
	{
	}
	menu_base(menu_base const&) = default;
	menu_base& operator=(menu_base const&) = default;

	virtual ~menu_base()
	{
	}

	bool activate(tui::console& console, unsigned row);
	void print_label(tui::console& console)
	{
		console.lcd() << label;
	}
protected:
	void redraw(tui::console& console);

	virtual void print_actions(tui::console& console) = 0;
	virtual bool activate_action(tui::console& console, unsigned idx,
			unsigned row) = 0;

protected:
	char const* label;
	char const* hotkeys;
	std::size_t hotkeys_count;
	std::size_t actions_count;
	unsigned scroll;
	unsigned selected;
};

template<typename ... Actions>
class menu: public menu_base
{
	template<typename I, typename ...Param>
	struct Print
	{
		static bool apply(I& item, Param&&... param)
		{
			item.print_label(param...);
			return true;
		}
	};

	template<typename I, typename ... Param>
	struct Activate
	{
		static bool apply(I& item, Param&&... param)
		{
			return item.activate(param...);
		}
	};

	typedef std::tuple<Actions...> actions_tuple;
	constexpr static std::size_t actions_count =
			std::tuple_size<actions_tuple>::value;
	constexpr static unsigned lines_count = 4;
public:
	menu(char const* label, char const* hotkeys, Actions&&... actions) :
			menu_base(label, hotkeys, std::tuple_size<actions_tuple>::value), actions(
					std::forward<Actions>(actions)...)
	{
	}
private:
	void print_actions(tui::console& console)
	{
		for (unsigned i = scroll; i < scroll + lines_count && i < actions_count; i++)
		{
			console.lcd() << hw::lcd::position(2, i - scroll);
			util::make_tuple_applicator(actions, console).template apply_to<bool, Print>(i);
			if (i < hotkeys_count)
			{
				console.lcd() << hw::lcd::position(19, i - scroll) << hotkeys[i];
			}
		}
	}

	bool activate_action(tui::console& console, unsigned idx, unsigned row)
	{
		return util::make_tuple_applicator(actions, console, row).template apply_to<
		bool, Activate>(idx);
	}
private:
	actions_tuple actions;
};

template<typename ... Actions>
menu<Actions...> create(char const* label, Actions&&... actions)
{
	return menu<Actions...>(label, nullptr, std::forward<Actions>(actions)...);
}

template<typename ... Actions>
menu<Actions...> create(char const* label, char const* hotkeys,
		Actions&&... actions)
{
	return menu<Actions...>(label, hotkeys, std::forward<Actions>(actions)...);
}

// Simple actions

class back
{
public:
	bool activate(tui::console&, unsigned)
	{
		return false;
	}
	void print_label(tui::console& console)
	{
		console.lcd() << "Back" << tui::console::BackCharacter << "            #";
	}
};

class label
{
public:
	label(char const* label) :
			l(label)
	{
	}

	bool activate(tui::console&, unsigned)
	{
		return true;
	}
	void print_label(tui::console& console)
	{
		console.lcd() << l;
	}

private:
	char const* l;
};
}
}

#endif /* __MENU_HPP */
