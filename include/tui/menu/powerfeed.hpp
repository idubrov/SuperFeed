#ifndef __POWERFEED_HPP
#define __POWERFEED_HPP

#include "tui/console.hpp"
#include "hw/driver.hpp"
#include "hw/spindle.hpp"
#include "stepper/stepper.hpp"
#include "stepper/conversions.hpp"

namespace tui
{
namespace menu
{

class powerfeed
{
public:
	powerfeed(hw::driver& driver, stepper::controller& stepper,
			hw::spindle& spindle,
			hw::eeprom& eeprom) :
			driver(driver), stepper(stepper), spindle(spindle), eeprom(eeprom),
			ipm_feed(1), ipr_feed(1), ipm_rapid(1), ipr(false), rapid(false)
	{
	}

	bool activate(tui::console& console, unsigned);

	inline void print_label(tui::console& console)
	{
		console.lcd() << "Powerfeed";
	}

private:
	struct state
	{
		hw::lcd::HD44780 const& lcd;
		conversions::converter conv;
	};
private:
	bool update_ipm(state& state);
	void update_display(state& s);

	void print_offset(hw::lcd::HD44780 const& lcd, int offset);
	int32_t read_offset(tui::console& console, tui::console::Event& ev);
private:
	hw::driver& driver;
	stepper::controller& stepper;
	hw::spindle& spindle;
	hw::eeprom& eeprom;

	unsigned ipm_feed;
	unsigned ipr_feed;
	unsigned ipm_rapid;
	bool ipr;
	bool rapid;
};
}
}

#endif /* __POWERFEED_HPP */
