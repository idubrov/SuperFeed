#include "stepgen.hpp"
#include <iostream>

void stepgen::stepgen::do_stop()
{
	switch (_state)
	{
	case Starting:
		_state = Stopped;
		_delay = 0;
		break;
	case MidStep:
	case Accelerating:
		_state = Decelerating;
		// Switching from acceleration to deceleration
		_denom = _denom + 2;
		_steps = _step * 2 - 1; // We need one step less to stop
		break;
	case Slewing:
		_state = Decelerating;
		// Switching from acceleration to deceleration
		_denom = _denom + 2;
		_steps = _step + _to_stop;
		break;
	case Decelerating:
	case Stopped:
		// Nothing
		break;
	}
}

uint32_t stepgen::stepgen::next()
{
	if (_stop) {
		do_stop();
		_stop = false;
	}
	_step++;

	switch (_state)
	{
	case Stopped:
		_delay = 0;
		// Nothing to do!
		break;
	case MidStep:
		// Even move, return same delay as on previous step
		_state = Decelerating;
		break;
	case Starting:
		_delay = _start_delay;
		_denom = 1;
		_state = _steps == 1 ? Decelerating : Accelerating;
		if (_steps == 2)
		{
			// We are only starting, return the same step
			_state = MidStep;
		}
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
			_to_stop = _step - 1;
		}
		else if (_step == _midstep)
		{
			// Switching from acceleration to deceleration
			_denom = _denom + 2;
			// If we have even amount of steps, return same delay twice
			_state = _steps % 2 == 0 ? MidStep : Decelerating;
		}
		break;
	case Slewing:
		_delay = _slew_delay; // In case slew speed was updated
		if (_step >= _steps - _to_stop)
		{
			_state = Decelerating;
			// Switching from acceleration to deceleration
			_denom = _denom + 2;
		}
		break;
	case Decelerating:
		// Last step indicator: decelerating and denominator is <= 3 (actually, it should be == 3)
		if (_denom <= 3) {
			_state = Stopped;
			_delay = 0;
			break;
		}
		// Update speed
		_denom -= 4;
		_delay += ((2 * _delay + _denom / 2) / _denom);
		break;
	}

	// Round calculated delay
	return (_delay + 128) >> 8;
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

