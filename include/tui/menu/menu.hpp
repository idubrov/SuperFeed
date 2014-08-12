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
class menu_base
{
public:
	menu_base(char const* label, std::size_t actions_count) :
			label(label), actions_count(actions_count), scroll(0), offset(0)
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
	unsigned selected_item(tui::console& console) const {
		return (console.enc_position() + offset) % actions_count;
	}
	void redraw(tui::console& console);

	virtual void print_actions(tui::console& console) = 0;
	virtual bool activate_action(tui::console& console, unsigned idx,
			unsigned row) = 0;

protected:
	char const* label;
	std::size_t actions_count;
	unsigned scroll;
	unsigned offset;
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
	menu(char const* label, Actions&&... actions) :
			menu_base(label, std::tuple_size<actions_tuple>::value), actions(
					std::forward<Actions>(actions)...)
	{
	}
private:
	void print_actions(tui::console& console)
	{
		for (unsigned i = 0; i < lines_count && i + scroll < actions_count; i++)
		{
			console.lcd() << hw::lcd::position(2, i);
			util::make_tuple_applicator(actions, console).template apply_to<
			bool, Print>(i + scroll);
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
	return menu<Actions...>(label, std::forward<Actions>(actions)...);
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
		console.lcd() << "Back" << tui::console::BackCharacter;
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
