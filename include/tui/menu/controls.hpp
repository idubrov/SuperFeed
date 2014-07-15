#ifndef __CONTROLS_HPP
#define __CONTROLS_HPP

#include "hw/eeprom.hpp"
#include "tui/console.hpp"

namespace tui
{
namespace menu
{

class spinner
{
public:
	spinner(hw::eeprom& eeprom, char const* label, uint_fast16_t tag,
			uint16_t def_value = 0) :
			eeprom(eeprom), label(label), tag(tag), def_value(def_value), value(
					def_value)
	{
		eeprom.read(tag, value);
	}

	bool activate(tui::console& console, unsigned);
	void print_label(tui::console& console)
	{
		console.lcd() << label << ": " << value;
	}
private:
	hw::eeprom& eeprom;
	char const* label;
	uint16_t const tag;
	uint16_t const def_value;
	uint16_t value;
};
}
}

#endif /* __CONTROLS_HPP */
