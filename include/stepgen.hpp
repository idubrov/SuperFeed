#ifndef __STEPGEN_HPP
#define __STEPGEN_HPP

#include <cstdint>

namespace stepgen
{

class stepgen
{
public:
	constexpr stepgen() :
			_step(0), _speed(0), _delay(0), _slewing(false), _target_step(0), _slew_delay(
					0), _start_delay(0)
	{
	}

	/// \param steps amount of steps to make. If steps is 0, stepgen would reach
	///	slew speed and run infinitely (until explicit stop is called)
	constexpr stepgen(uint32_t target_step, uint32_t start_delay,
			uint32_t slew_delay) :
			_step(0), _speed(0), _delay(0), _slewing(false), _target_step(
					target_step), _slew_delay(slew_delay), _start_delay(
					start_delay)
	{
	}

	stepgen& operator=(const stepgen &) = default;

	// Returns '0' if should stop
	// Otherwise, timer delay in 24.8 format
	uint32_t next();

	inline void stop()
	{
		_target_step = 0;
	}

	inline bool stopped() const {
		return _step >= _target_step && _speed <= 1;
	}

	/// \param frequency timer frequency, ticks per second
	/// \param slew_speed slew speed, steps per second
	/// \return slew delay (delay between steps while running at maximum speed), in 24.8 format
	static uint32_t slew(uint32_t frequency, uint32_t slew_speed);

	/// \param frequency timer frequency, ticks per second
	/// \param acceleration acceleration, in steps per second per second
	/// \return first step delay (delay between the first and the second steps), in 24.8 format
	static uint32_t first(uint32_t frequency, uint32_t acceleration);
private:
	static uint64_t sqrt(uint64_t x);

	inline void speedup() {
		uint32_t denom = 4 * _speed + 1;
		_delay -= ((2 * _delay + denom / 2) / denom);
		_speed++;
	}

	inline void slowdown() {
		_speed--;
		uint32_t denom = 4 * _speed - 1;
		_delay += ((2 * _delay + denom / 2) / denom);
	}

private:
	// State
	uint32_t _step;	// Current step
	uint32_t _speed; // Amount of acceleration steps we've taken so far
	uint32_t _delay; // Previously calculated delay

	// Slewing, do not accelerate or decelerate even if delay != slew_delay.
	// Switched to this mode once we overshoot target speed.
	bool _slewing;

	// Parameters
	uint32_t _target_step; // Target step
	uint32_t _slew_delay; // Slew step delay
	uint32_t _start_delay; // First step delay

};
}

#endif /* __STEPGEN_HPP */
