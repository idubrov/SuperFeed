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
			driver(driver), stepper(stepper), conv(
					conversions::converter(eeprom))
	{
	}

	bool activate(tui::console& console, unsigned);

	inline void print_label(tui::console& console)
	{
		console.lcd() << "Powerfeed";
	}

private:
	inline void set_ipm(uint32_t ipmby10)
	{
		bool set = stepper.set_speed(ipmby10 * conv.pulse_per_inch() / 600); // 60 seconds * scale of ipm
		if (!set)
		{
			// FIXME: message?
			stepper.stop();
			while (1)
				;
		}
	}

	inline void print_position(hw::lcd::HD44780 const& lcd)
	{
		uint32_t mills = conv.pulse_to_mils(stepper.current_position());
		lcd << hw::lcd::position(0, 1) << "Position: ";
		lcd << (mills / 1000) << '.' << format<10, Right, '0'>(mills % 1000, 3)
				<< "    ";
	}

	inline void print_speed(hw::lcd::HD44780 const& lcd)
	{
		uint32_t mills = conv.pulse_to_mils(stepper.current_speed());
		lcd << hw::lcd::position(0, 1) << "Speed: ";
		lcd << (mills / 1000) << '.' << format<10, Right, '0'>(mills % 1000, 3)
				<< "    ";
	}

private:
	hw::driver& driver;
	stepper::controller& stepper;
	conversions::converter conv;
};
}
}

#endif /* __POWERFEED_HPP */
