#ifndef __STEPPER_HPP
#define __STEPPER_HPP

#include "config.hpp"
#include "driver.hpp"
#include "util.hpp"

class stepper
{
public:
	enum State
	{
		Stopped, Accelerating, Slewing, Decelerating
	};
public:
	stepper(TIM_TypeDef* slave_tim, TIM_TypeDef* master_tim) :
		_slave_tim(slave_tim), _master_tim(master_tim), _state(Stopped), _position(0)
	{
		_acceleration = 20; // IPM/sec
		_deceleration = 20; // IPM/sec
		_max_speed = 30; // IPM
	}

	void initialize();

	State state() const
	{
		return _state;
	}

	int32_t position() const
	{
		return _position;
	}

	bool move_to(int32_t position, uint32_t max_speed = 0);
	bool move(int32_t speed);
	bool stop();

	// Spindle synchronized motion. Start moving when index pulse is received with the speed
	// define by given TPI.
	bool move_sync_tpi(uint32_t tpi);
private:
	inline void update()
	{

	}
private:
	// Configuration
	static constexpr uint32_t MicrostepsPerInch = ::cfg::stepper::StepsPerRev
			* ::cfg::stepper::Microsteps * ::cfg::stepper::LeadscrewTPI;

	// Timers
	TIM_TypeDef* const _slave_tim;
	TIM_TypeDef* const _master_tim;

	// Speed settings
	uint32_t _acceleration;
	uint32_t _deceleration;
	uint32_t _max_speed;

	volatile State _state;
	volatile int32_t _position;
};

#endif /* __STEPPER_HPP */
