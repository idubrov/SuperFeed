#include "stepgen.hpp"

uint32_t stepgen::stepgen::next()
{
	// Read latest values once
	uint32_t target_step = _target_step;
	uint32_t target_delay = _target_delay;
	if (stopped())
	{
		_speed = 0;
		return 0;
	}

	// Steps made so far
	_step++;
	if (_speed == 0)
	{
		// First step: load first delay
		_delay = _first_delay;
		_speed = 1;
		return _delay;
	}

	// Calculate the projected step we would stop at if we start decelerating right now
	uint32_t est_stop = _step + _speed - 1;
	if (est_stop == target_step)
	{
		// We would stop one step earlier than we want, so let's just
		// return the same delay as the current one and start deceleration
		// on the next step.
	}
	else if (est_stop > target_step)
	{
		// We need to stop at target step, slow down
		slowdown();

		// We are not slewing even though we could have slowed down below the slewing speed
		_slewing = false;
	}
	else if (!_slewing && _delay < target_delay)
	{
		// Not slewing and running too fast, slow down
		slowdown();

		// Switch to slewing if we slowed down enough
		_slewing = _delay >= target_delay;
	}
	else if (!_slewing && _delay > target_delay)
	{
		// Not slewing and running too slow, speed up
		speedup();

		// Switch to slewing if we accelerated enough
		_slewing = _delay <= target_delay;
	}

	// If slewing, return slew delay. _delay should be close enough, but could
	// be different due to the accumulated rounding errors
	return _slewing ? target_delay : _delay;

}

uint64_t stepgen::stepgen::sqrt(uint64_t x)
{
	uint64_t xr;  // result register
	uint64_t q2;  // scan-bit register
	xr = 0; // clear result
	q2 = 0x4000000000000000LL; // higest possible result bit
	do
	{
		if ((xr + q2) <= x)
		{
			x -= xr + q2;
			xr >>= 1;
			xr += q2; // test flag
		}
		else
		{
			xr >>= 1;
		}
	} while (q2 >>= 2); // shift twice
	return xr < x ? xr + 1 : xr; // add for rounding
}

