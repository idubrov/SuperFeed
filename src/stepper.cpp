#include "stepper.hpp"
#include "config.hpp"

using namespace ::stepper;
using namespace ::cfg::stepper;

void controller::setup_port()
{
	// Control port, STEP pin (timer controlled)
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = _hw._step_pin;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_hw._port, &gpio);

	// Control port (manually controlled)
	gpio.GPIO_Pin = _hw._dir_pin | _hw._enable_pin | _hw._reset_pin;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_hw._port, &gpio);

	// Start in reset mode
	_hw._port->BSRR = _hw._reset_pin;
}

void controller::setup_timer()
{
	TIM_TimeBaseInitTypeDef init;
	// FIXME: should be 23 for real delay timer, to have 1usec period.
	init.TIM_Prescaler = 2399; // 100us period
	init.TIM_ClockDivision = TIM_CKD_DIV1;
	init.TIM_CounterMode = TIM_CounterMode_Up;
	init.TIM_Period = 0; // Configured later
	init.TIM_RepetitionCounter = 0; // Used for microstepping

	TIM_DeInit(_hw._timer);
	TIM_TimeBaseInit(_hw._timer, &init);

	// We load CC1 and ARR with next value while timer is running
	TIM_CCPreloadControl(_hw._timer, DISABLE);
	TIM_ARRPreloadConfig(_hw._timer, ENABLE);
	TIM_OC1PreloadConfig(_hw._timer, TIM_OCPreload_Enable);

	// Configure PWM
	TIM_OCInitTypeDef ocInit;
	ocInit.TIM_OCMode = TIM_OCMode_PWM2; // inactive till CCR1, then active
	ocInit.TIM_OutputState = TIM_OutputState_Enable;
	ocInit.TIM_OutputNState = TIM_OutputNState_Disable;
	ocInit.TIM_Pulse = 0; // Configured later
	ocInit.TIM_OCPolarity = TIM_OCPolarity_Low; // Step pulse on IM483 is '0'
	ocInit.TIM_OCNPolarity = TIM_OCNPolarity_High;
	ocInit.TIM_OCIdleState = TIM_OCIdleState_Set; // Set STEP to '1' when idle
	ocInit.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(_hw._timer, &ocInit);
	TIM_CtrlPWMOutputs(_hw._timer, DISABLE);

	// Enable interrupts
	TIM_ClearITPendingBit(_hw._timer, TIM_IT_Update);
	TIM_ITConfig(_hw._timer, TIM_IT_Update, ENABLE);
}

void controller::initialize()
{
	setup_timer();
	setup_port();

	// FIXME: check return value
	_stepgen.set_acceleration((Acceleration * Microsteps) << 8);
}

bool controller::move(uint32_t steps)
{
	// Timer is running already, we can't start new moves!
	if (_hw._timer->CR1 & TIM_CR1_CEN)
		return false;

	// If interrupt is pending, wait until it is cleared.
	// (for instance, we got move command just after last timer overflow
	// and it wasn't processed yet.
	while (TIM_GetITStatus(_hw._timer, TIM_IT_Update))
		;

	_stepgen.set_target_step(steps);
	_stop = false;

	// Load first delay into ARR/CC1, if not stopped
	if (load_delay() == 0)
	{
		// Not making even a single step
		return false;
	}

	// Load ARR/CC1 from preload registers
	TIM_GenerateEvent(_hw._timer, TIM_EventSource_Update);

	// Wait till interrupt finishes
	while (TIM_GetITStatus(_hw._timer, TIM_IT_Update))
		;

	// Load second delay into ARR & CC1.
	bool single = load_delay() == 0;

	// Start pulse generation
	TIM_SelectOnePulseMode(_hw._timer, single ? TIM_OPMode_Single : TIM_OPMode_Repetitive);
	TIM_CtrlPWMOutputs(_hw._timer, ENABLE);
	TIM_Cmd(_hw._timer, ENABLE);

	return true;
}

uint32_t controller::load_delay()
{
	uint32_t delay = _stepgen.next();
	if (delay != 0)
	{
		// Load new step into ARR, start pulse at the end
		uint32_t d = (delay + 128) >> 8; // Delay is in 16.8 format
		_hw._timer->ARR = d;
		_hw._timer->CCR1 = d - _delays._step_len;
	}
	return delay;
}

void controller::step_completed()
{
	TIM_ClearITPendingBit(_hw._timer, TIM_IT_Update);

	if (!(_hw._timer->CR1 & TIM_CR1_CEN))
	{
		// If timer is stopped, do nothing!
		return;
	}

	if (_stop)
	{
		_stepgen.set_target_step(0);
		_stop = false;
	}

	if (load_delay() == 0)
	{
		// Stop on the next update, one pulse mode
		_hw._timer->CR1 |= TIM_CR1_OPM;
	}
}
