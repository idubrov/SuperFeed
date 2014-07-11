#ifndef __INPUTS_HPP
#define __INPUTS_HPP

#include "tui/console.hpp"
#include "tui/menu/action.hpp"

namespace tui
{
namespace menu
{

class inputs : public action
{
public:
	void activate(tui::console& console);
	void print_label(tui::console& console)
	{
		console.lcd() << "Verify inputs";
	}
};
}
}

#endif /* __INPUTS_HPP */
