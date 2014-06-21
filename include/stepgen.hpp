#ifndef __STEPGEN_HPP
#define __STEPGEN_HPP

#include <cstdint>

namespace stepgen
{
class stepgen
{
public:
	stepgen(uint32_t frequency) :
			_step(0), _speed(0), _delay(0), _slewing_delay(0), _frequency(
					frequency), _first_delay(0), _target_step(0), _target_delay(
					0)
	{
	}

	stepgen& operator=(const stepgen &) = default;

	// Returns '0' if should stop
	// Otherwise, returns timer delay in 24.8 format
	uint32_t next();

	/// Set destination step for the stepper motor pulse generator. This is
	/// the primary constraint used to determine necessary action to take.
	/// If current step > target step, stepper motor would slow down until stop
	/// if running or stay stopped if not running.
	/// Setting target step to 0 will always force stepper to decelerate and stop.
	/// \param step step to stop at
	inline void set_target_step(uint32_t step)
	{
		_target_step = step;
	}

	/// Set slew speed (maximum speed stepper motor would run). Note that stepper
	/// motor would only reach this speed if target step is far enough, so there is
	/// enough time for deceleration.
	/// \param speed target slew speed to reach, in steps per second, 24.8 format
	/// \return false if target speed is either too slow (doesn't fit into timer counter)
	/// or too fast.
	inline bool set_target_speed(uint32_t speed)
	{
		uint64_t delay = (static_cast<uint64_t>(_frequency) << 16) / speed;
		if (delay >> 24)
		{
			return false; // Doesn't fit in in 16.8 format, our timer is only 16 bit.
		}
		if (delay <= 10 * (1 << 8))
		{
			// Too slow, less than 10 ticks of a timer. 10 is an arbitrary number,
			// just to make sure we have enough time to calculate next delay.
			return false;
		}
		_target_delay = delay;
		return true;
	}

	/// Set stepper motor acceleration. Note that this method is computation intensive,
	/// so it's best to set acceleration once and never change it.
	/// \param acceleration acceleration, in steps per second, in 24.8 format
	/// \return false if acceleration is too slow and thus delay of the first
	/// step would not fit into 16 bit timer counter.
	inline bool set_acceleration(uint32_t acceleration)
	{
		// c0 = F*sqrt(2/a)*.676 = F*sqrt(2/a)*676/1000 =
		//      F*sqrt(2*676*676/a)/1000 =
		//      F*sqrt(2*676*676*1^16)/(1000*1^8)
		// We bring as much as we can under square root, to increase accuracy of division
		// sqrt(1 << 16) is (1 << 8), which is to convert to 24.8
		// We shift 24 bits to the left to adjust for acceleration in 24.8 format plus to convert
		// result into 24.8 format, so the resulting shift is 40 bits.
		uint64_t c0long = ((2LL * 676 * 676) << 40) / acceleration;
		uint64_t c0 = (_frequency * sqrt(c0long) / 1000) >> 8;
		if (c0 >> 24)
			return false; // Doesn't fit in in 16.8 format, our timer is only 16 bit.
		_first_delay = static_cast<uint32_t>(c0);
		return true;
	}

	inline bool stopped() const
	{
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

	// State commands

	inline uint32_t step() const
	{
		return _step;
	}

	inline uint32_t target_step() const
	{
		return _target_step;
	}

	inline uint32_t speed() const
	{
		return _speed;
	}

	inline uint32_t target_delay() const
	{
		return _target_delay;
	}

private:
	static uint64_t sqrt(uint64_t x);

	inline void speedup()
	{
		uint32_t denom = 4 * _speed + 1;
		_delay -= ((2 * _delay + denom / 2) / denom);
		_speed++;
	}

	inline void slowdown()
	{
		_speed--;
		uint32_t denom = 4 * _speed - 1;
		_delay += ((2 * _delay + denom / 2) / denom);
	}

private:
	// State
	uint32_t _step;	// Current step
	uint32_t _speed; // Amount of acceleration steps we've taken so far
	uint32_t _delay; // Previously calculated delay

	// If slewing, this will be the slewing delay. Switched to this mode once
	// we overshoot target speed.
	uint32_t _slewing_delay;

	// Parameters
	uint32_t _frequency; // Timer frequency
	uint32_t _first_delay; // First step delay

	// These two could be changed from outside
	volatile uint32_t _target_step; // Target step
	volatile uint32_t _target_delay; // Target speed delay
};
}

#endif /* __STEPGEN_HPP */
