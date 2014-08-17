#ifndef __POWERFEED_HPP
#define __POWERFEED_HPP

#include "tui/console.hpp"
#include "hw/driver.hpp"
#include "stepper/stepper.hpp"

namespace tui
{
namespace menu
{

class powerfeed
{
public:
	powerfeed(hw::driver& driver, stepper::controller& stepper, hw::eeprom& eeprom) :
		driver(driver), stepper(stepper), eeprom(eeprom)
	{
	}

	bool activate(tui::console& console, unsigned);
	void print_label(tui::console& console)
	{
		console.lcd() << "Powerfeed";
	}
private:
	hw::driver& driver;
	stepper::controller& stepper;
	hw::eeprom& eeprom;
};
}
}

#endif /* __POWERFEED_HPP */
