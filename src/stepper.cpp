#include "stepper.hpp"
#include "lcd.hpp"

using namespace ::cfg::stepper;

uint16_t Source[] =
{ 200, 200, 200 };
constexpr int DataCount = sizeof(Source) / sizeof(Source[0]);

void stepper::setup_port()
{
	// Control port
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = _step_pin;
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_port, &gpio);
}

void stepper::setup_output_timer()
{
	TIM_TimeBaseInitTypeDef init;
	init.TIM_Prescaler = 239; // 10us period
	init.TIM_Period = 10000; // 100ms delay
	init.TIM_ClockDivision = TIM_CKD_DIV1;
	init.TIM_CounterMode = TIM_CounterMode_Up;
	init.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(OutputTimer, &init);
	TIM_CCPreloadControl(OutputTimer, DISABLE);
	TIM_ARRPreloadConfig(OutputTimer, DISABLE);
	TIM_SelectOnePulseMode(OutputTimer, TIM_OPMode_Single);

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
	TIM_OC1Init(OutputTimer, &ocInit);
	TIM_CtrlPWMOutputs(OutputTimer, ENABLE);

	// Configure input trigger
	TIM_SelectInputTrigger(OutputTimer, TIM_TS_ITR3); // TIM4 (master) -> TIM1 (slave)
}

void stepper::setup_step_timer()
{
	TIM_TimeBaseInitTypeDef init;
	init.TIM_Prescaler = 23999; // 1ms period
	init.TIM_Period = 0;
	init.TIM_ClockDivision = TIM_CKD_DIV1;
	init.TIM_CounterMode = TIM_CounterMode_Up;
	init.TIM_RepetitionCounter = 0;

	// Start STEP pulse once timer overflows.
	TIM_SelectOutputTrigger(StepperTimer, TIM_TRGOSource_Update);

	// Initialize timer
	TIM_TimeBaseInit(StepperTimer, &init);

	TIM_ARRPreloadConfig(StepperTimer, ENABLE);
	TIM_ClearITPendingBit(StepperTimer, TIM_IT_Update);
	TIM_ITConfig(StepperTimer, TIM_IT_Update, ENABLE);

	// Output timer interrupts
	NVIC_InitTypeDef timerIT;
	timerIT.NVIC_IRQChannel = TIM4_IRQn;
	timerIT.NVIC_IRQChannelPreemptionPriority = 0;
	timerIT.NVIC_IRQChannelSubPriority = 1;
	timerIT.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&timerIT);
}

void stepper::initialize()
{
//	TIM_DeInit(OutputTimer);
//	TIM_DeInit(StepperTimer);
//
	setup_output_timer();
	setup_step_timer();
	setup_port();

		// First series of pulses
		start_stepgen();

		// Wait for step timer to finish
		while(StepperTimer->CR1 & TIM_CR1_CEN);
		while(OutputTimer->CR1 & TIM_CR1_CEN);

		// Second series of pulses
		start_stepgen();

		while(1);

}

void stepper::start_stepgen()
{
	_step = 0;

	// Don't trigger pulse generation.
	// It should only be generated when timer overflows.
	TIM_SelectSlaveMode(OutputTimer, TIM_SlaveMode_Reset);
	// Load first delay
	update();
	// Load ARR from preload register, load second delay into ARR.
	TIM_GenerateEvent(StepperTimer, TIM_EventSource_Update);
	TIM_SelectSlaveMode(OutputTimer, TIM_SlaveMode_Trigger);

	// Start pulse generation
	TIM_Cmd(StepperTimer, ENABLE);
}

void stepper::update()
{
	TIM_ClearITPendingBit(StepperTimer, TIM_IT_Update);
	uint32_t step = _step;
	if (step < DataCount)
	{
		// Load new step into ARR
		StepperTimer->ARR = Source[step];
	}
	else if (step > DataCount)
	{
		// This is interrupt after we generated last delay, disable step generation.
		TIM_Cmd(StepperTimer, DISABLE);
	}
	// Otherwise, if step == Datacount, ARR was reloaded from preload register for the last time.
	// We would need to stop timer overflows for the next time.
	_step = step + 1;
}
