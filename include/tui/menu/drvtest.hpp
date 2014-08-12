#ifndef __DRVTEST_HPP
#define __DRVTEST_HPP

#include "settings.hpp"
#include "hw/eeprom.hpp"
#include "hw/driver.hpp"
#include "stepper/stepper.hpp"
#include "tui/console.hpp"

namespace tui
{
namespace menu
{

class rotatetest
{
public:
	rotatetest(hw::driver& driver, stepper::controller& stepper, hw::eeprom& eeprom) :
			driver(driver), stepper(stepper), eeprom(eeprom) {}

	bool activate(tui::console& console, unsigned);
	void print_label(tui::console& console)
	{
		console.lcd() << "Do full rotation";
	}
private:
	hw::driver& driver;
	stepper::controller& stepper;
	hw::eeprom& eeprom;
};
}
}

#endif /* __DRVTEST_HPP */
