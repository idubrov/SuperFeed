#ifndef __ACTION_HPP
#define __ACTION_HPP

#include "tui/console.hpp"

namespace tui
{
namespace menu
{

class action
{
public:
	virtual ~action()
	{
	}

	virtual void print_label(tui::console&) = 0;
	virtual void activate(tui::console&) = 0;
};
}
}

#endif /* __ACTION_HPP */
