#include "stepper.hpp"

using namespace ::stepper;

uint16_t Source[] =
{ 2000, 2000, 2000 };
constexpr int DataCount = sizeof(Source) / sizeof(Source[0]);

void controller::setup_port()
{
	// Control port
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = _hw._step_pin;
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_hw._port, &gpio);

	// Control port
	gpio.GPIO_Pin = _hw._dir_pin | _hw._enable_pin | _hw._reset_pin;
	gpio.GPIO_Mode = GPIO_Mode_Out_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_hw._port, &gpio);

	// Start in reset mode
	_hw._port->BRR = _hw._reset_pin;
}

void controller::setup_pulse_timer()
{
	TIM_TimeBaseInitTypeDef init;
	// FIXME: should be 23 for real delay timer, to have 1usec period.
	init.TIM_Prescaler = 2399; // 100us period
	// FIXME: should be (x + 999) / 1000
	init.TIM_Period = _delays._step_len;
	init.TIM_ClockDivision = TIM_CKD_DIV1;
	init.TIM_CounterMode = TIM_CounterMode_Up;
	init.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(_hw._pulse_tim, &init);
	TIM_CCPreloadControl(_hw._pulse_tim, DISABLE);
	TIM_ARRPreloadConfig(_hw._pulse_tim, DISABLE);
	TIM_SelectOnePulseMode(_hw._pulse_tim, TIM_OPMode_Single);

	// Configure PWM
	TIM_OCInitTypeDef ocInit;
	ocInit.TIM_OCMode = TIM_OCMode_PWM2; // '0' till CCR1, then '1'
	ocInit.TIM_OutputState = TIM_OutputState_Enable;
	ocInit.TIM_OutputNState = TIM_OutputNState_Disable;
	ocInit.TIM_Pulse = 1; // Start step pulse right on the next cycle
	ocInit.TIM_OCPolarity = TIM_OCPolarity_Low; // Step pulse on IM483 is '0'
	ocInit.TIM_OCNPolarity = TIM_OCNPolarity_High;
	ocInit.TIM_OCIdleState = TIM_OCIdleState_Set; // Set STEP to '1' when idle
	ocInit.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(_hw._pulse_tim, &ocInit);
	TIM_CtrlPWMOutputs(_hw._pulse_tim, ENABLE);

	// Configure input trigger
	// _step_tim (master) -> _pulse_tim (slave)
	TIM_SelectInputTrigger(_hw._pulse_tim, _hw._pulse_tim_trigger_source);

	// Enable interrupts
	TIM_ClearITPendingBit(_hw._pulse_tim, TIM_IT_Update);
	TIM_ITConfig(_hw._pulse_tim, TIM_IT_Update, ENABLE);
}

void controller::setup_step_timer()
{
	TIM_TimeBaseInitTypeDef init;
	// FIXME: should be 23 for real delay timer, to have 1usec period.
	init.TIM_Prescaler = 2399; // 100us period
	init.TIM_Period = 0;
	init.TIM_ClockDivision = TIM_CKD_DIV1;
	init.TIM_CounterMode = TIM_CounterMode_Up;
	init.TIM_RepetitionCounter = 0;

	// Start STEP pulse once timer overflows.
	TIM_SelectOutputTrigger(_hw._step_tim, TIM_TRGOSource_Update);

	// Initialize timer
	TIM_TimeBaseInit(_hw._step_tim, &init);
	TIM_ARRPreloadConfig(_hw._step_tim, ENABLE);

	// Enable interrupt
	TIM_ClearITPendingBit(_hw._step_tim, TIM_IT_Update);
	TIM_ITConfig(_hw._step_tim, TIM_IT_Update, ENABLE);
}

void controller::initialize()
{
	setup_pulse_timer();
	setup_step_timer();
	setup_port();

	// First series of pulses
	start_stepgen();

	// Wait for step timer to finish
	while (_hw._step_tim->CR1 & TIM_CR1_CEN)
		;
	while (_hw._pulse_tim->CR1 & TIM_CR1_CEN)
		;

	// Second series of pulses
	start_stepgen();

	while (1)
		;

}

void controller::start_stepgen()
{
	_step = 0;

	// Don't trigger pulse generation.
	// It should only be generated when timer overflows.
	TIM_SelectSlaveMode(_hw._pulse_tim, TIM_SlaveMode_Reset);
	// Load first delay
	update();
	// Load ARR from preload register, load second delay into ARR.
	TIM_GenerateEvent(_hw._step_tim, TIM_EventSource_Update);
	TIM_SelectSlaveMode(_hw._pulse_tim, TIM_SlaveMode_Trigger);

	// Start pulse generation
	TIM_Cmd(_hw._step_tim, ENABLE);
}

void controller::update()
{
	TIM_ClearITPendingBit(_hw._step_tim, TIM_IT_Update);
	uint32_t step = _step;
	if (step < DataCount)
	{
		// Load new step into ARR
		_hw._step_tim->ARR = Source[step];
	}
	else if (step > DataCount)
	{
		// This is interrupt after we generated last delay, disable step generation.
		TIM_Cmd(_hw._step_tim, DISABLE);
	}
	// Otherwise, if step == Datacount, ARR was reloaded from preload register for the last time.
	// We would need to stop timer overflows for the next time.
	_step = step + 1;
}
