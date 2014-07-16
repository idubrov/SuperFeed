#ifndef __SETTINGS_HPP
#define __SETTINGS_HPP

#include "hw/eeprom.hpp"

namespace settings
{

struct setting
{
	constexpr setting(char const* label, uint16_t tag, uint16_t def_value,
			uint16_t min, uint16_t max = 0xffff) : label(label),
			tag(tag), def_value(def_value), min(min), max(max)
	{
	}
	uint16_t get(hw::eeprom& eeprom) const
	{
		uint16_t result = def_value;
		eeprom.read(tag, result);
		return result;
	}

	char const* label;
	uint16_t const tag;
	uint16_t const def_value;
	uint16_t const min;
	uint16_t const max;
};

constexpr setting Microsteps("Microsteps", 1, 1, 1, 32);
constexpr setting LeadscrewTPI("Leadscrew TPI", 2, 1, 1, 40);
constexpr setting GearNominator("Gear Nom.", 3, 1, 1);
constexpr setting GearDenominator("Gear Denom.", 4, 1, 1);
constexpr setting Acceleration("Accel.", 5, 1, 1);

// Stepper driver settings
constexpr setting StepLen("Step length", 0x10, 1, 1, 50000); // in us
constexpr setting StepSpace("Step space", 0x10, 1, 1, 50000); // in us
constexpr setting DirectionSetup("Dir. setup", 0x10, 1, 1, 50000); // in us
constexpr setting DirectionHold("Dir. hold", 0x10, 1, 1, 50000); // in us

}

#endif /* __SETTINGS_HPP */
