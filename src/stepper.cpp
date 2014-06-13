#include "stepper.hpp"
#include "lcd.hpp"

void stepper::initialize()
{
	// Master configuration
	// Tick every 65536 CPU clock
	TIM_TimeBaseInitTypeDef master;
	master.TIM_Prescaler = 0xffff;
	master.TIM_Period = 0;
	master.TIM_ClockDivision = TIM_CKD_DIV1;
	master.TIM_CounterMode = TIM_CounterMode_Up;
	master.TIM_RepetitionCounter = 0;
	TIM_DeInit(TIM15);
	TIM_TimeBaseInit(TIM15, &master);
	TIM_SelectOnePulseMode(TIM15, TIM_OPMode_Single);
	TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
	TIM_ITConfig(TIM15, TIM_IT_Update, ENABLE);
	TIM_SelectOutputTrigger(TIM15, TIM_TRGOSource_Update);

	// Master interrupt
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Slave configuration
	TIM_TimeBaseInitTypeDef slave;
	slave.TIM_Prescaler = 0;
	slave.TIM_Period = 0;
	slave.TIM_ClockDivision = TIM_CKD_DIV1;
	slave.TIM_CounterMode = TIM_CounterMode_Up;
	slave.TIM_RepetitionCounter = 0;
	TIM_DeInit(TIM4);
	TIM_TimeBaseInit(TIM4, &slave);
	TIM_SelectOnePulseMode(TIM4, TIM_OPMode_Single);
	TIM_SelectInputTrigger(TIM4, TIM_TS_ITR3);
	TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Trigger);
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	// Slave interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	// Some testing...
	TIM_SetAutoreload(TIM4, 366); // LSB
	TIM_SetAutoreload(TIM15, 366); // HSB
	TIM_Cmd(TIM15, ENABLE);
}


extern "C"
void __attribute__ ((section(".after_vectors")))
TIM4_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	util::led4_on();
}

extern "C"
void __attribute__ ((section(".after_vectors")))
TIM1_BRK_TIM15_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
	util::led3_on();
}


