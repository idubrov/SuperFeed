#include "stepper.hpp"

void stepper::initialize()
{
	// Tick every 65536 CPU clock
	TIM_TimeBaseInitTypeDef master;
	master.TIM_Prescaler = 0xffff;
	master.TIM_Period = 20;
	master.TIM_ClockDivision = TIM_CKD_DIV1;
	master.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_DeInit(_master_tim);
	TIM_TimeBaseInit(_master_tim, &master);
	TIM_SelectOnePulseMode(_master_tim, TIM_OPMode_Single);
	TIM_ClearITPendingBit(_master_tim, TIM_IT_CC1);
	TIM_ITConfig(_master_tim, TIM_IT_CC1, ENABLE);
	//_master_tim->DIER = 0;

	// Tick every clock
	TIM_TimeBaseInitTypeDef slave;
	slave.TIM_Prescaler = 0;
	slave.TIM_Period = 0;
	slave.TIM_ClockDivision = TIM_CKD_DIV1;
	slave.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_DeInit(_slave_tim);
	TIM_TimeBaseInit(_slave_tim, &slave);
	TIM_SelectOnePulseMode(_slave_tim, TIM_OPMode_Single);
	TIM_SelectInputTrigger(_slave_tim, TIM_TS_ITR2);
	TIM_SelectSlaveMode(_slave_tim, TIM_SlaveMode_Trigger);
	TIM_SelectMasterSlaveMode(_slave_tim, TIM_MasterSlaveMode_Enable);
	TIM_ClearITPendingBit(_slave_tim, TIM_IT_Update);
	TIM_ITConfig(_slave_tim, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure; //create NVIC structure
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Some testing...
	TIM_SetAutoreload(_slave_tim, 1);
	TIM_SetCompare1(_master_tim, 1);
	TIM_Cmd(_master_tim, ENABLE);
	//TIM_Cmd(_slave_tim, ENABLE);
}

extern "C"
void __attribute__ ((section(".after_vectors")))
TIM1_BRK_TIM15_IRQHandler(void)
{
	util::led4_on();
}

extern "C"
void __attribute__ ((section(".after_vectors")))
TIM1_UP_TIM16_IRQHandler(void)
{
	if (TIM16->SR & TIM_FLAG_CC1) {
		util::led3_on();
		//TIM_Cmd(TIM15, ENABLE);
	}
}


