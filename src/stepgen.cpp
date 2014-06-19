#include "stepgen.hpp"

using namespace ::stepgen;

void stepgen::stepgen::calc_delay()
{
	switch (_state)
	{
	case Stopped:
		// Nothing to do!
		break;
	case MidStep:
		// Even move, return same delay as on previous step
		_state = Decelerating;
		break;
	case Accelerating:
		// Update speed
		_denom += 4;
		_delay -= ((2 * _delay + _denom / 2) / _denom);

		// Switch to slewing, if trying to run too fast
		if (_delay < _slew_delay)
		{
			_state = Slewing;
			_delay = _slew_delay;
			_decel_step = _steps - _step - 1;
		}

		if (_step == _midstep - 1)
		{
			_state = Decelerating;
			_denom = 4 * _step + 3;
			// If we have even amount of steps, return same delay twice
			_state = _steps % 2 == 0 ? MidStep : Decelerating;
		}
		break;
	case Slewing:
		if (_step == _decel_step)
		{
			_state = Decelerating;
			_denom = 4 * (_steps - _decel_step) - 1;
		}
		break;
	case Decelerating:
		// Update speed
		_denom -= 4;
		_delay += ((2 * _delay + _denom / 2) / _denom);
		break;
	}
}

uint32_t stepgen::stepgen::next()
{
	uint32_t delay = _delay;
	uint32_t step = ++_step;

	// Last step, return calculated delay an transition to stopped state
	if (step == _steps)
	{
		_state = Stopped;
		_delay = 0; // Next delay is zero, which is indication of completion.
	}
	else
	{
		calc_delay();
	}

	// Round previously calculated delay
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

