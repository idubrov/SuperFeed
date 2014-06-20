#include "stepgen.hpp"

uint32_t stepgen::stepgen::next()
{
	if (stopped())
	{
		return 0;
	}

	// Steps made so far
	_step++;
	if (_speed == 0)
	{
		// First step: load first delay
		_delay = _start_delay;
		_speed = 1;
		return (_delay + 128) >> 8;
	}

	// Calculate the projected step we would stop at if we start decelerating right now
	uint32_t est_stop = _step + _speed - 1;
	if (est_stop == _target_step)
	{
		// We would stop one step earlier than we want, so let's just
		// return the same delay as the current one and start deceleration
		// on the next step.
	}
	else if (est_stop > _target_step)
	{
		// We need to stop at target step, slow down
		slowdown();

		// We are not slewing even though we could have slowed down below the slewing speed
		_slewing = false;
	}
	else if (!_slewing && _delay < _slew_delay)
	{
		// Not slewing and running too fast, slow down
		slowdown();

		// Switch to slewing if we slowed down enough
		_slewing = _delay >= _slew_delay;
	}
	else if (!_slewing && _delay > _slew_delay)
	{
		// Not slewing and running too slow, speed up
		speedup();

		// Switch to slewing if we accelerated enough
		_slewing = _delay <= _slew_delay;
	}

	// If slewing, return slew delay. _delay should be close enough, but could
	// be different due to the accumulated rounding errors
	uint32_t delay = _slewing ? _slew_delay : _delay;
	return (delay + 128) >> 8;

}

uint32_t stepgen::stepgen::slew(uint32_t frequency, uint32_t slew_speed)
{
	return (frequency << 8) / slew_speed;
}

uint32_t stepgen::stepgen::first(uint32_t frequency, uint32_t acceleration)
{
	// c0 = F * sqrt(2/a) * 0.676
	// We bring as much as we can under square root, to increase accuracy of division
	// sqrt(1 << 16) is (1 << 8), which is to convert to 24.8
	uint64_t c0long = 2000000LL * 676 * 676 * (1 << 16) / acceleration;
	uint64_t c0 = frequency * sqrt(c0long) / 1000000;
	if (c0 >> 24)
	{
		return 0; // Doesn't fit in in 16.8 format, our timer is only 16 bit.
	}
	return static_cast<uint32_t>(c0); // Cast to uint32_t
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

