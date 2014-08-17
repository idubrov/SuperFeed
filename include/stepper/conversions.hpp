#ifndef __CONVERSIONS_HPP
#define __CONVERSIONS_HPP

#include "hw/eeprom.hpp"

namespace conversions
{
class converter
{
public:
	converter(hw::eeprom& eeprom) :
			geared_microsteps(
					settings::Microsteps.get(eeprom)
							* settings::LeadscrewGear.get(eeprom)), stepper_gear(
					settings::StepperGear.get(eeprom)), metric(
					settings::MetricLeadscrew.get(eeprom)),
					tpi(settings::LeadscrewTPI.get(eeprom)),
					pitch(settings::LeadscrewPitch.get(eeprom))
	{
		ppr = ((200 * geared_microsteps) << 8) / stepper_gear;
		ppi = metric ? ((ppr * 254) / (10 * pitch)) : ppr * tpi;
	}

	/// \return pulses per inch, in 24.8 format
	inline uint32_t pulse_per_inch() const
	{
		return ppi;
	}

	/// Following functions try to be as precise as possible


	inline uint32_t pulse_to_mils(uint32_t position)
	{
		// full leadscrew rotations multiplied by 1000 (5 = 1000 / 200 full steps)
		uint32_t rots = (5 * position * stepper_gear) / geared_microsteps;
		if (metric)
			return rots * 10 * pitch / 254;
		else
			return rots / tpi;
	}

private:
	uint16_t const geared_microsteps; // microsteps * nom
	uint16_t const stepper_gear;
	bool const metric;
	uint32_t ppr;
	uint32_t ppi;
	uint16_t tpi;
	uint16_t pitch;
};

}

#endif /* __CONVERSIONS_HPP */
