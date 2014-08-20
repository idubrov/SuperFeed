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
			driver(driver), stepper(stepper), eeprom(eeprom),
			ipm_feed(1), ipm_rapid(1), rapid(false)
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
	inline void update_ipm(state& state)
	{
		unsigned ipm = rapid ? ipm_rapid : ipm_feed;
		bool set = stepper.set_speed(
				static_cast<uint64_t>(ipm) * state.conv.pulse_per_inch()
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

	void print_offset(hw::lcd::HD44780 const& lcd, int offset);
	int32_t read_offset(tui::console& console, tui::console::Event& ev);
private:
	hw::driver& driver;
	stepper::controller& stepper;
	hw::eeprom& eeprom;

	unsigned ipm_feed;
	unsigned ipm_rapid;
	bool rapid;
};
}
}

#endif /* __POWERFEED_HPP */
