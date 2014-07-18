#include "hw/driver.hpp"

void hw::driver::initialize_port() const
{
	// Control port, STEP pin (timer controlled)
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = _step_pin;
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_port, &gpio);

	// Control port (manually controlled)
	gpio.GPIO_Pin = _dir_pin | _enable_pin | _reset_pin;
	gpio.GPIO_Mode = GPIO_Mode_Out_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_port, &gpio);

	// Start in reset mode
	_port->BRR = _reset_pin;
}

void hw::driver::initialize_timer() const
{
	TIM_TimeBaseInitTypeDef init;
	init.TIM_Prescaler = driver::Prescaler;
	init.TIM_ClockDivision = TIM_CKD_DIV1;
	init.TIM_CounterMode = TIM_CounterMode_Up;
	init.TIM_Period = 0; // Configured later
	init.TIM_RepetitionCounter = 0; // Used for microstepping

	TIM_DeInit(_timer);
	TIM_TimeBaseInit(_timer, &init);

	// We load CC1 and ARR with next value while timer is running
	TIM_CCPreloadControl(_timer, DISABLE);
	TIM_ARRPreloadConfig(_timer, ENABLE);
	TIM_OC1PreloadConfig(_timer, TIM_OCPreload_Enable);

	// Configure PWM
	TIM_OCInitTypeDef ocInit;
	TIM_OCStructInit(&ocInit);
	ocInit.TIM_OCMode = TIM_OCMode_PWM2; // inactive till CCR1, then active
	ocInit.TIM_OutputNState = TIM_OutputNState_Enable; // We use CH1N
	ocInit.TIM_Pulse = 0; // Will be configured later
	ocInit.TIM_OCNPolarity = TIM_OCNPolarity_Low; // Step pulse is '0'
	ocInit.TIM_OCNIdleState = TIM_OCNIdleState_Set; // Set STEP to '1' when idle
	TIM_OC1Init(_timer, &ocInit);
	TIM_CtrlPWMOutputs(_timer, DISABLE);

	// Enable interrupts
	TIM_ClearITPendingBit(_timer, TIM_IT_Update);
	TIM_ITConfig(_timer, TIM_IT_Update, ENABLE);
}
