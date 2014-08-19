#ifndef __SETTINGS_HPP
#define __SETTINGS_HPP

#include <limits>

#include "util.hpp"
#include "hw/eeprom.hpp"

namespace settings
{

template<typename Data>
struct setting
{
	constexpr setting(char const* label, uint16_t tag, Data def_value) :
			label(label), tag(tag), def_value(def_value)
	{
	}
	uint16_t get(hw::eeprom const& eeprom) const
	{
		uint16_t result = def_value;
		eeprom.read(tag, result);
		return result;
	}

	void set(hw::eeprom& eeprom, uint16_t value) const
	{
		uint16_t current = def_value;
		eeprom.read(tag, current);
		if (current != value)
			eeprom.write(tag, value);
	}

	char const* const label;
	uint16_t const tag;
	Data const def_value;

};

struct numeric: setting<uint16_t>
{
	constexpr numeric(char const* label, uint16_t tag, uint16_t def_value,
			uint16_t min, uint16_t max = std::numeric_limits<uint16_t>::max()) :
			setting<uint16_t>(label, tag, def_value), min(min), max(max)
	{
	}

	util::ranged_value<uint16_t> ranged(hw::eeprom const& eeprom) const
	{
		return util::ranged(get(eeprom), min, max);
	}

	uint16_t const min;
	uint16_t const max;
};

struct boolean: setting<bool>
{
	constexpr boolean(char const* label, uint16_t tag, bool def_value,
			char const* off_label, char const* on_label) :
			setting<bool>(label, tag, def_value), off_label(off_label), on_label(
					on_label)
	{
	}
	char const* const off_label;
	char const* const on_label;
};

// Hardware configuration
constexpr numeric Microsteps("Microsteps", 0x01, 1, 1, 32);
constexpr numeric Acceleration("Accel.", 0x02, 10000, 1, 50000);
constexpr boolean MetricLeadscrew("Leadscrew", 0x03, false, "(inch)", "(mm)");
constexpr numeric LeadscrewTPI("  TPI", 0x04, 1, 1, 40);
constexpr numeric LeadscrewPitch("  Pitch", 0x05, 1, 1, 40);
constexpr numeric LeadscrewGear("  A", 0x06, 1, 1, 1000); // Leadscrew gear
constexpr numeric StepperGear("  B", 0x07, 1, 1, 1000); // Stepper motor gear
constexpr boolean Reverse("Reverse", 0x08, false, "false", "true");
constexpr numeric RapidFeed("Rapid IPM*10", 0x09, 200, 1, 500);
constexpr numeric LastFeed("Feed IPM*10", 0x10, 50, 1, 500);

// Stepper driver timings
constexpr numeric StepLen("Step len.", 0x10, 1, 1, 50000); // in ns
constexpr numeric StepSpace("Step space", 0x11, 1, 1, 50000); // in ns
constexpr numeric DirectionSetup("Dir setup", 0x12, 1, 1, 50000); // in ns
constexpr numeric DirectionHold("Dir hold", 0x13, 1, 1, 50000); // in ns
}

#endif /* __SETTINGS_HPP */
