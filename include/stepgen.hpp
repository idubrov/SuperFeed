#ifndef __STEPGEN_HPP
#define __STEPGEN_HPP

namespace stepgen
{

enum State
{
	Accelerating, Decelerating, Slewing, Stopped, MidStep
};
class stepgen
{
public:
	constexpr stepgen(uint32_t steps, uint32_t start_delay, uint32_t slew_delay) :
			_step(0), _decel_step(0), _midstep((steps + 1) / 2), _steps(steps), _denom(
					1), _delay(start_delay), _slew_delay(slew_delay), _state(
					Accelerating)
	{
		//System.out.println("(simulated) Start speed: " + (c >> 8));
	}
	// Returns '0' if should stop
	// Otherwise, timer delay in 24.8 format
	uint32_t next()
	{
		uint32_t delay = _delay;
		uint32_t step = ++_step;
		if (step == _steps)
		{
			// Last step
			_state = Stopped;
			return delay;
		}

		State state = _state;
		switch (_state)
		{
		case Stopped:
			delay = 0;
			break;
		case MidStep:
			// Even move, return same delay
			_state = Decelerating;
			break;
		case Accelerating:
			// Update speed
			_denom += 4;
			_delay -= (2 * _delay / _denom);

			// Switch to slewing, if trying to run too fast
			if (_delay < _slew_delay)
			{
				_state = Slewing;
				_delay = _slew_delay;
				_decel_step = _steps - step - 1;
			}

			if (step == _midstep - 1)
			{
				_state = Decelerating;
				_denom = 4 * step + 3;
				// If we have even amount of steps, return same delay twice
				_state = _steps % 2 == 0 ? MidStep : Decelerating;
			}
			break;
		case Slewing:
			if (step == _decel_step)
			{
				_state = Decelerating;
				_denom = 4 * (_steps - _decel_step) - 1;
			}
			break;
		case Decelerating:
			// Update speed
			_denom -= 4;
			_delay += (2 * _delay / _denom);
			break;
		}

		return delay;
	}

private:
	volatile uint32_t _step;
	uint32_t _decel_step;
	uint32_t const _midstep;
	uint32_t const _steps;
	uint32_t _denom;
	uint32_t _delay;
	uint32_t const _slew_delay;
	volatile State _state;
};
}

#endif /* __STEPGEN_HPP */
