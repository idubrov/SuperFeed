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
	inputs(tui::console& console) : _console(console)
	{
	}

	void run();
private:
	tui::console& _console;
};
}
}

#endif /* __INPUTS_HPP */
