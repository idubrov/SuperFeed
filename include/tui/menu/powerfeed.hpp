#ifndef __POWERFEED_HPP
#define __POWERFEED_HPP

#include "tui/console.hpp"
#include "stepper/stepper.hpp"

namespace tui
{
namespace menu
{

class powerfeed
{
public:
	powerfeed(stepper::controller& stepper) : stepper(stepper)
	{
	}

	bool activate(tui::console& console, unsigned);
	void print_label(tui::console& console)
	{
		console.lcd() << "Powerfeed";
	}
private:
	stepper::controller& stepper;
};
}
}

#endif /* __POWERFEED_HPP */
