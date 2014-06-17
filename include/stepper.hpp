#ifndef __STEPPER_HPP
#define __STEPPER_HPP

#include "config.hpp"
#include "driver.hpp"
#include "util.hpp"

extern "C" {
void TIM4_IRQHandler();
}

class stepper
{
	friend void ::TIM4_IRQHandler();
public:
	enum State
	{
		Stopped, Accelerating, Slewing, Decelerating
	};
public:
	stepper(GPIO_TypeDef* port, uint16_t step_pin,
			TIM_TypeDef* slave_tim, TIM_TypeDef* master_tim) :
		_step(0),
		_port(port), _step_pin(step_pin),
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

	// Step generation timer interrupt tick
	inline void interrupt() {
		update();
	}
private:
	void setup_port();
	void setup_output_timer();
	void setup_step_timer();
	void start_stepgen();
private:
	// Pulse generation control
	volatile uint32_t _step;

	// Update state machine and load new delay into timer
	void update();

private:
	// Configuration
	static constexpr uint32_t MicrostepsPerInch = ::cfg::stepper::StepsPerRev
			* ::cfg::stepper::Microsteps * ::cfg::stepper::LeadscrewTPI;

	// Port and pins
	GPIO_TypeDef* const _port;
	uint16_t const _step_pin;

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
