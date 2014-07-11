#ifndef __INPUTS_HPP
#define __INPUTS_HPP

#include "tui/console.hpp"

namespace tui
{
namespace menu
{

class inputs
{
public:
	void activate(tui::console& console, unsigned);
	void print_label(tui::console& console)
	{
		console.lcd() << "Verify inputs";
	}
};
}
}

#endif /* __INPUTS_HPP */
