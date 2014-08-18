#ifndef __POWERFEED_HPP
#define __POWERFEED_HPP

#include "tui/console.hpp"
#include "hw/driver.hpp"
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
			hw::eeprom& eeprom) :
			driver(driver), stepper(stepper), eeprom(eeprom)
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
	inline void set_ipm(unsigned ipmby10, state& state)
	{
		bool set = stepper.set_speed(
				static_cast<uint64_t>(ipmby10) * state.conv.pulse_per_inch()
						/ 600); // 60 seconds * scale of ipm
		if (!set)
		{
			// FIXME: message?
			stepper.stop();
			while (1)
				;
		}
	}

	void update_display(state& s);
private:
	hw::driver& driver;
	stepper::controller& stepper;
	hw::eeprom& eeprom;

	// Currently selected ipm
	unsigned ipm;
};
}
}

#endif /* __POWERFEED_HPP */
