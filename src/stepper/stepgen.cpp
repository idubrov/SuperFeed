#include "stepper/stepgen.hpp"

void stepgen::stepgen::speedup()
{
	uint32_t denom = 4 * speed + 1;
	delay -= ((2 * delay + denom / 2) / denom);
	speed++;
}

void stepgen::stepgen::slowdown()
{
	speed--;
	uint32_t denom = 4 * speed - 1;
	delay += ((2 * delay + denom / 2) / denom);
}

bool stepgen::stepgen::set_target_speed(uint32_t speed)
{
	uint64_t delay = (static_cast<uint64_t>(frequency) << 16) / speed;
	if (delay >> 24)
		return false; // Too slow, doesn't fit in in 16.8 format, our timer is only 16 bit.
	if (delay <= stepgen::stepgen::TicksPerUpdate * (1 << 8))
	{
		// Too fast, less than 10 ticks of a timer. 10 is an arbitrary number,
		// just to make sure we have enough time to calculate next delay.
		return false;
	}
	tgt_delay.store(delay, std::memory_order_relaxed);
	return true;
}

bool stepgen::stepgen::set_acceleration(uint32_t acceleration)
{
	// c0 = F*sqrt(2/a)*.676 = F*sqrt(2/a)*676/1000 =
	//      F*sqrt(2*676*676/a)/1000 =
	//      F*sqrt(2*676*676*1^16)/(1000*1^8)
	// We bring as much as we can under square root, to increase accuracy of division
	// sqrt(1 << 16) is (1 << 8), which is to convert to 24.8
	// We shift 24 bits to the left to adjust for acceleration in 24.8 format plus to convert
	// result into 24.8 format, so the resulting shift is 40 bits.
	uint64_t c0long = ((2LL * 676 * 676) << 40) / acceleration;
	uint64_t c0 = (frequency * sqrt(c0long) / 1000) >> 8;
	if (c0 >> 24)
		return false; // Doesn't fit in in 16.8 format, our timer is only 16 bit.
	first_delay = static_cast<uint32_t>(c0);
	return true;
}

uint32_t stepgen::stepgen::current_speed() const
{
	uint32_t d = slewing_delay ? slewing_delay : delay;
	if (d == 0)
		return 0;
	uint64_t speed = (static_cast<uint64_t>(frequency) << 16) / d;
	return speed;
}

uint32_t stepgen::stepgen::next()
{
	// Read latest values once
	uint32_t target_step = tgt_step.load(std::memory_order_relaxed);
	uint32_t target_delay = tgt_delay.load(std::memory_order_relaxed);
	uint32_t st = current_step();
	if (st >= target_step && speed <= 1)
	{
		speed = 0;
		return 0;
	}

	// Stop slewing if target delay was changed
	if (slewing_delay && slewing_delay != target_delay)
	{
		slewing_delay = 0;
	}

	// Steps made so far
	step.fetch_add(1, std::memory_order_relaxed);
	if (speed == 0)
	{
		// First step: load first delay
		delay = first_delay;
		speed = 1;
		return delay;
	}

	// Calculate the projected step we would stop at if we start decelerating right now
	uint32_t est_stop = st + speed;
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
		slewing_delay = 0;
	}
	else if (!slewing_delay && delay < target_delay)
	{
		// Not slewing and running too fast, slow down
		slowdown();

		// Switch to slewing if we slowed down enough
		if (delay >= target_delay)
		{
			slewing_delay = target_delay;
		}

	}
	else if (!slewing_delay && delay > target_delay)
	{
		// Not slewing and running too slow, speed up
		speedup();

		// Switch to slewing if we have accelerated enough
		if (delay <= target_delay)
		{
			slewing_delay = target_delay;
		}
	}

	// If slewing, return slew delay. delay should be close enough, but could
	// be different due to the accumulated rounding errors
	return slewing_delay ? slewing_delay : delay;
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

