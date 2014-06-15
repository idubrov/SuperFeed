#include "stepper.hpp"

using namespace ::cfg::stepper;

uint16_t Source[] = { 750, 500, 250, 500, 500 };
uint16_t Delays[] =
{ 750, 500, 250 };

void stepper::setup_port() {
	// Control port
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
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

	// Configure as slave
	TIM_SelectInputTrigger(OutputTimer, TIM_TS_ITR3); // TIM4 (master) -> TIM1 (slave)
	TIM_SelectSlaveMode(OutputTimer, TIM_SlaveMode_Trigger);
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
	// Note: we don't use Update event as TRGO since we emit Update's manually
	// to trigger DMA to load first pulse delay. We don't want slave timer
	// to fire when we do it.
	// Note: set it before configuring timer, so slave is not fired when
	// we do TIM_TimeBaseInit
	TIM_SelectOutputTrigger(StepperTimer, TIM_TRGOSource_OC1);
	TIM_SetCompare1(StepperTimer, 0);

	// Initialize timer
	TIM_UpdateRequestConfig(StepperTimer, TIM_UpdateSource_Regular);
	TIM_TimeBaseInit(StepperTimer, &init);
	TIM_UpdateRequestConfig(StepperTimer, TIM_UpdateSource_Global);

	// Check if we need to stop
	TIM_ClearITPendingBit(StepperTimer, TIM_IT_Update);
	//TIM_ITConfig(StepperTimer, TIM_IT_Update, ENABLE);

	// Load new delay on update
	TIM_DMACmd(StepperTimer, TIM_DMA_Update, ENABLE);

	// Output timer interrupts
	NVIC_InitTypeDef timerIT;
	timerIT.NVIC_IRQChannel = TIM4_IRQn;
	timerIT.NVIC_IRQChannelPreemptionPriority = 0;
	timerIT.NVIC_IRQChannelSubPriority = 1;
	timerIT.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&timerIT);
}

void stepper::setup_dma()
{
	// Configure DMA channel
	DMAChannel->CCR = DMA_IT_TC |
			DMA_DIR_PeripheralDST | DMA_Mode_Normal |
			DMA_PeripheralInc_Disable | DMA_PeripheralDataSize_HalfWord |
			DMA_MemoryInc_Enable | DMA_MemoryDataSize_HalfWord |
			DMA_Priority_High | DMA_M2M_Disable;
	DMAChannel->CNDTR = 0; // Will be set after data is loaded
	DMAChannel->CMAR = 0; // Will be set after data is loaded
	DMAChannel->CPAR = (uint32_t) &(TIM4->ARR);

	// DMA interrupts
	NVIC_InitTypeDef dmaIT;
	dmaIT.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	dmaIT.NVIC_IRQChannelPreemptionPriority = 0;
	dmaIT.NVIC_IRQChannelSubPriority = 0;
	dmaIT.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&dmaIT);
}

void stepper::initialize()
{
	TIM_DeInit(OutputTimer);
	TIM_DeInit(StepperTimer);

	setup_port();
	setup_output_timer();
	setup_step_timer();
	setup_dma();

	// First series of pulses
	load_data();
	start_stepgen();

	// Wait for step timer to finish
	while(StepperTimer->CR1 & TIM_CR1_CEN);

	// Second series of pulses
	load_data();
	start_stepgen();
}

void stepper::start_stepgen() {
	// Timer might have pending DMA request latched.
	// Need to toggle the flag to reset the latch.
	// Otherwise, a transfer would happen once we re-enable DMA channel!
	TIM_DMACmd(StepperTimer, TIM_DMA_Update, DISABLE);
	TIM_DMACmd(StepperTimer, TIM_DMA_Update, ENABLE);

	// Disable timer update interrupt generation
	// We use it for stopping the timer after last chunk is transmitted
	TIM_ITConfig(StepperTimer, TIM_IT_Update, DISABLE);
	// Enable DMA channel
	DMA_Cmd(DMAChannel, ENABLE);
	// Load first chunk of data
	TIM_GenerateEvent(StepperTimer, TIM_EventSource_Update);
	// Start pulse generation
	TIM_Cmd(StepperTimer, ENABLE);
}

void stepper::load_data() {
	DMA_Cmd(DMAChannel, DISABLE);
	DMAChannel->CMAR = (uint32_t) Delays;
	DMAChannel->CNDTR = sizeof(Delays) / sizeof(Delays[0]);
}

extern "C" void __attribute__ ((section(".after_vectors")))
TIM4_IRQHandler(void)
{
	// Stop step generation timer and clear all pending interrupt flags
	TIM_Cmd(StepperTimer, DISABLE);
	TIM_ITConfig(StepperTimer, TIM_IT_Update, DISABLE);
	TIM_ClearITPendingBit(StepperTimer, TIM_IT_Update);
}

extern "C" void __attribute__ ((section(".after_vectors")))
DMA1_Channel7_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_TC7))
	{
		// No more data to send: allow timer to stop itself after the last cycle
		TIM_ClearITPendingBit(StepperTimer, TIM_IT_Update);
		TIM_ITConfig(StepperTimer, TIM_IT_Update, ENABLE);
	}
	DMA_ClearITPendingBit(DMA1_IT_GL7);
}

