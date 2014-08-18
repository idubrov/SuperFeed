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
	inline void set_ipm(uint32_t ipmby10, state& state)
	{
		bool set = stepper.set_speed(static_cast<uint64_t>(ipmby10) * state.conv.pulse_per_inch() / 600); // 60 seconds * scale of ipm
		if (!set)
		{
			// FIXME: message?
			stepper.stop();
			while (1)
				;
		}
	}

	inline void print_ipm(unsigned ipm, state& s)
	{
		s.lcd << hw::lcd::position(0, 0) << "Speed: " << format<>(ipm, 3, 1) << " IPM   ";
	}

	inline void print_position(state& s)
	{
		int32_t mills = s.conv.pulse_to_mils(stepper.current_position());
		s.lcd << hw::lcd::position(0, 1) << "Position: ";
		s.lcd << format<10, Right>(mills, 8, 3);
	}

	inline void print_status(state& s)
	{
		if (stepper.is_stopped())
		{
			s.lcd << hw::lcd::position(0, 3) << "                    ";
			return;
		}

		s.lcd << hw::lcd::position(0, 3) << (stepper.current_direction() ? "<<<<<<<< " : ">>>>>>>> ");
		uint32_t mills = (s.conv.pulse_to_mils(stepper.current_speed()) * 60)  >> 8;
		uint32_t ipm = (mills + 50) / 100; // rounding
		s.lcd << format<10, Right>(ipm, 8, 1);
	}

private:
	hw::driver& driver;
	stepper::controller& stepper;
	hw::eeprom& eeprom;
};
}
}

#endif /* __POWERFEED_HPP */
