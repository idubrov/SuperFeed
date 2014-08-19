#ifndef __CONTROLS_HPP
#define __CONTROLS_HPP

#include "settings.hpp"
#include "hw/eeprom.hpp"
#include "tui/console.hpp"

namespace tui
{
namespace menu
{
class numeric_base
{
public:
	numeric_base(hw::eeprom& eeprom, settings::numeric const& setting) :
			eeprom(eeprom), setting(setting)
	{
	}

	void print_label(tui::console& console)
	{
		console.lcd() << setting.label << ": " << setting.get(eeprom);
	}
protected:
	hw::eeprom& eeprom;
	settings::numeric const& setting;
};

class spinner: public numeric_base
{
public:
	spinner(hw::eeprom& eeprom, settings::numeric const& setting) :
			numeric_base(eeprom, setting)
	{
	}

	bool activate(tui::console& console, unsigned);
};

class numeric: public numeric_base
{
public:
	numeric(hw::eeprom& eeprom, settings::numeric const& setting) :
	numeric_base(eeprom, setting)
	{
	}

	bool activate(tui::console& console, unsigned);
};

class toggle
{
public:
	toggle(hw::eeprom& eeprom, settings::boolean const& setting) :
			eeprom(eeprom), label(setting.label), off_label(setting.off_label), on_label(
					setting.on_label), tag(setting.tag), def_value(
					setting.def_value)
	{
	}

	bool activate(tui::console& console, unsigned);
	void print_label(tui::console& console)
	{
		uint16_t value = def_value;
		eeprom.read(tag, value);
		console.lcd() << label << ": " << (value ? on_label : off_label);
	}
private:
	hw::eeprom& eeprom;
	char const* const label;
	char const* const off_label;
	char const* const on_label;
	uint16_t const tag;bool const def_value;
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
