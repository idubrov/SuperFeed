#ifndef __MENU_HPP
#define __MENU_HPP

#include <utility>
#include <array>

#include "tui/console.hpp"
#include "tui/menu/action.hpp"

namespace tui
{
namespace menu
{
class menu: public action
{
	constexpr static unsigned lines_count = 4;
public:
	menu(action* const * actions, std::size_t actions_count) :
			actions(actions), actions_count(actions_count), scroll(0)
	{
	}
	menu(menu const&) = delete;
	menu& operator=(menu const&) = delete;

	void activate(tui::console& console);
	void print_label(tui::console& console)
	{
		console.lcd() << "Main menu";
	}

private:
	void redraw(tui::console& console);

private:
	action* const * actions;
	std::size_t actions_count;

	unsigned scroll;
};

}
}

#endif /* __MENU_HPP */
