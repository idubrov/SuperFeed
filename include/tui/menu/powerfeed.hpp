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

	inline void print_ipm(hw::lcd::HD44780 const& lcd, unsigned ipm)
	{
		unsigned frac = ipm % 10;
		lcd << hw::lcd::position(0, 0) << "Speed: " << format<>(ipm, 3, 1) << " IPM   ";
	}

	inline void print_position(hw::lcd::HD44780 const& lcd)
	{
		int32_t mills = conv.pulse_to_mils(stepper.current_position());
		lcd << hw::lcd::position(0, 1) << "Position: ";
		lcd << format<10, Right>(mills, 8, 3);
	}

	inline void print_status(hw::lcd::HD44780 const& lcd)
	{
		if (stepper.is_stopped())
		{
			lcd << hw::lcd::position(0, 3) << "                    ";
			return;
		}

		uint32_t mills = conv.pulse_to_mils(stepper.current_speed() * 60) >> 8;
		uint32_t ipm = (mills + 50) / 100; // rounding
		lcd << hw::lcd::position(0, 3) << (stepper.current_direction() ? "<<<<<<<< " : ">>>>>>>> ");
		lcd << format<10, Right>(ipm, 8, 1);
	}

private:
	hw::driver& driver;
	stepper::controller& stepper;
	conversions::converter conv;
};
}
}

#endif /* __POWERFEED_HPP */
