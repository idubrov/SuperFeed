#ifndef __STEPGEN_HPP
#define __STEPGEN_HPP

#include <cstdint>
#include <atomic>

namespace stepgen
{
class stepgen
{
public:
	// How many timer ticks it would take for one update
	constexpr static uint32_t TicksPerUpdate = 10;
public:
	stepgen(uint32_t frequency) :
			step(0), speed(0), delay(0), slewing_delay(0), frequency(
					frequency), first_delay(0), tgt_step(0), tgt_delay(
					0)
	{
	}

	stepgen& operator=(const stepgen &) = default;

	///
	/// Configuration methods. Should not be called while motor is running.
	///

	/// Set stepper motor acceleration. Note that this method is computation intensive,
	/// so it's best to set acceleration once and never change it.
	/// \param acceleration acceleration, in steps per second per second, in 24.8 format
	/// \return false if acceleration is too slow and thus delay of the first
	/// step would not fit into 16 bit timer counter.
	bool set_acceleration(uint32_t acceleration);

	///
	/// Motor control API. Could be called while motor is running.
	///

	// Returns '0' if should stop
	// Otherwise, returns timer delay in 24.8 format
	uint32_t next();

	/// Set destination step for the stepper motor pulse generator. This is
	/// the primary constraint used to determine necessary action to take.
	/// If current step > target step, stepper motor would slow down until stop
	/// if running or stay stopped if not running.
	/// Setting target step to 0 will always force stepper to decelerate and stop.
	/// \param step step to stop at
	void set_target_step(uint32_t step)
	{
		tgt_step.store(step, std::memory_order_relaxed);
	}

	/// Set slew speed (maximum speed stepper motor would run). Note that stepper
	/// motor would only reach this speed if target step is far enough, so there is
	/// enough time for deceleration.
	/// \param speed target slew speed to reach, in steps per second, 24.8 format
	/// \return false if target speed is either too slow (doesn't fit into timer counter)
	/// or too fast.
	bool set_target_speed(uint32_t speed);

	///
	/// State query commands, should only be used for displaying purposes.
	/// These shouldn't be used for making decisions, as they are not thread-safe
	///

	inline uint32_t current_step() const
	{
		return step.load(std::memory_order_relaxed);
	}

	inline uint32_t target_step() const
	{
		return tgt_step.load(std::memory_order_relaxed);
	}

	inline uint32_t current_speed() const
	{
		return speed;
	}

	inline uint32_t target_delay() const
	{
		return tgt_delay.load(std::memory_order_relaxed);
	}

private:
	static uint64_t sqrt(uint64_t x);

	void speedup();
	void slowdown();
private:
	// State, updated in the IRQ handler
	std::atomic_uint_fast32_t step;	// Current step

	// These two are not used outside of the IRQ handler
	uint32_t speed; // Amount of acceleration steps we've taken so far
	uint32_t delay; // Previously calculated delay

	// If slewing, this will be the slewing delay. Switched to this mode once
	// we overshoot target speed.
	uint32_t slewing_delay;

	// Parameters
	uint32_t frequency; // Timer frequency
	uint32_t first_delay; // First step delay

	// These two could be changed from outside
	std::atomic_uint_fast32_t tgt_step; // Target step
	std::atomic_uint_fast32_t tgt_delay; // Target speed delay
};
}

#endif /* __STEPGEN_HPP */
