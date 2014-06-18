#include "stepper.hpp"
#include "HD44780.hpp"

using namespace ::stepper;

uint16_t Source[] =
{ 20000, 2000, 20000 };
constexpr int DataCount = sizeof(Source) / sizeof(Source[0]);

extern lcd::HD44780* g_lcd;

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
//
//	// Start in reset mode
	_hw._port->BSRR = _hw._dir_pin | _hw._enable_pin | _hw._reset_pin;
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


	// First series of pulses
	start_stepgen();
}

void controller::start_stepgen()
{
	_step = 0;

	// Load first delay into ARR/CC1
	step_completed();
	// Load ARR/CC1 from preload registers, load second delay into ARR & CC1.
	TIM_GenerateEvent(_hw._timer, TIM_EventSource_Update);

	// Start pulse generation
	TIM_CtrlPWMOutputs(_hw._timer, ENABLE);
	TIM_Cmd(_hw._timer, ENABLE);
}

void controller::step_completed()
{
	TIM_ClearITPendingBit(_hw._timer, TIM_IT_Update);
	_offset++;
	uint32_t step = _step;
	if (step < DataCount)
	{
		// Load new step into ARR
		_hw._timer->ARR = Source[step];
		// Start STEP pulse in the end
		_hw._timer->CCR1 = Source[step] - _delays._step_len;
	}
	else if (step > DataCount)
	{
		// This is interrupt after we generated last delay, disable step generation.
		TIM_Cmd(_hw._timer, DISABLE);
	}
	// Otherwise, if step == Datacount, ARR was reloaded from preload register for the last time.
	// We would need to stop timer overflows for the next time.
	_step = step + 1;
}
