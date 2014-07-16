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
			uint16_t min, uint16_t max, uint16_t def_value = 0) :
			eeprom(eeprom), label(label), tag(tag), min(min), max(max), def_value(
					def_value)
	{
	}

	bool activate(tui::console& console, unsigned);
	void print_label(tui::console& console)
	{
		uint16_t value = def_value;
		eeprom.read(tag, value);
		console.lcd() << label << ": " << value;
	}
private:
	hw::eeprom& eeprom;
	char const* label;
	uint16_t const tag;
	uint16_t const min;
	uint16_t const max;
	uint16_t const def_value;
};

class erase_settings
{
public:
	erase_settings(hw::eeprom& eeprom) :
			eeprom(eeprom)
	{
	}

	bool activate(tui::console& console, unsigned);
	void print_label(tui::console& console)
	{
		console.lcd() << "Erase settings";
	}
private:
	hw::eeprom& eeprom;
};
}
}

#endif /* __CONTROLS_HPP */
