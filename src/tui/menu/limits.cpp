#include <limits>

#include "settings.hpp"
#include "hw/driver.hpp"
#include "tui/menu/limits.hpp"


bool tui::menu::limits::activate(tui::console& console, unsigned)
{
	auto& lcd = console.lcd();

	lcd << hw::lcd::clear();
	uint32_t min_pulse = hw::driver::Frequency
			/ std::numeric_limits<uint16_t>::max();
	lcd << hw::lcd::position(0, 0) << "Min speed: " << min_pulse;

	// Get system frequency
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	// Our guestimation how many cycles one update could take
	constexpr uint32_t CyclesPerUpdate = 100;

	// Max speed -- CPU speed limitation
	uint32_t max_pulse_tim = (RCC_Clocks.HCLK_Frequency + CyclesPerUpdate - 1)
			/ CyclesPerUpdate;
	lcd << hw::lcd::position(0, 1) << "Max speed: " << max_pulse_tim;

	// Max speed -- driver limitation
	uint32_t step_len = settings::StepLen.get(eeprom);
	uint32_t step_space = settings::StepSpace.get(eeprom);
	uint32_t max_pulse_dr = 1000000000 / (step_len + step_space);
	lcd << hw::lcd::position(0, 2) << "Max speed: " << max_pulse_dr;


	//
	while (true)
		;
	return true;
}
//namespace limits
//{
//// Maximum amount of pulses per second (driver limitation)
//constexpr uint32_t MaxPulseDR = 1000000000 / (StepLen + StepSpace);
//
//// Our guestimation how many cycles one update could take
//constexpr uint32_t CyclesPerUpdate = 100;
//// Maximum amount of pulses per second (timer limitation)
//constexpr uint32_t MaxPulseTIM = (Clock + CyclesPerUpdate - 1) / CyclesPerUpdate;
//
//// Maximum full steps per second
//constexpr uint32_t MaxPulse = MaxPulseTIM < MaxPulseDR ? MaxPulseTIM : MaxPulseDR;
//
//// Minimum amount of pulses per second (assuming 1usec period timer), limited by timer
//constexpr uint32_t MaxDelay = 60000; // Maximum timer delay is around 2^16 ticks.
//constexpr uint32_t MinPulsePS = Frequency / MaxDelay;
