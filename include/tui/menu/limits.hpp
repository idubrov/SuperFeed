#ifndef __LIMITS_HPP
#define __LIMITS_HPP

#include "tui/console.hpp"
#include "hw/eeprom.hpp"

namespace tui
{
namespace menu
{

class limits
{
public:
	limits(hw::eeprom& eeprom) :
			eeprom(eeprom)
	{
	}

	bool activate(tui::console& console, unsigned);
	void print_label(tui::console& console)
	{
		console.lcd() << "Speed limits";
	}
private:
	hw::eeprom const& eeprom;
};
}
}

#endif /* __INPUTS_HPP */
