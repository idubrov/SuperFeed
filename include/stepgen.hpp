#ifndef __STEPGEN_HPP
#define __STEPGEN_HPP

#include <cstdint>

namespace stepgen
{

enum State
{
	Accelerating, Decelerating, Slewing, Stopped, MidStep
};
class stepgen
{
public:
	constexpr stepgen() :
			_step(0), _to_stop(0), _midstep(0), _steps(0), _denom(0), _delay(
					0), _slew_delay(0), _state(Stopped)
	{
	}
	constexpr stepgen(uint32_t steps, uint32_t start_delay, uint32_t slew_delay) :
			_step(0), _to_stop(0), _midstep((steps + 1) / 2), _steps(steps), _denom(
					1), _delay(start_delay), _slew_delay(slew_delay), _state(
					Accelerating)
	{
	}

	stepgen& operator=(const stepgen &) = default;

	// Returns '0' if should stop
	// Otherwise, timer delay in 24.8 format
	uint32_t next();

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
	void calc_delay();

private:
	uint32_t _step;
	uint32_t _to_stop;
	uint32_t _midstep;
	uint32_t _steps;
	uint32_t _denom;
	uint32_t _delay;
	uint32_t _slew_delay;
	State _state;
};
}

#endif /* __STEPGEN_HPP */
