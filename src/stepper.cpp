#include "stepper.hpp"
#include "lcd.hpp"

using namespace ::cfg::stepper;

#define TIM1_ARR_Address    0x40012C2C

uint16_t Delays[] = { 3000, 500, 500 };

void stepper::initialize()
{
	TIM_TimeBaseInitTypeDef init;
	init.TIM_Prescaler = 23999; // 1ms period
	init.TIM_Period = 0;
	init.TIM_ClockDivision = TIM_CKD_DIV1;
	init.TIM_CounterMode = TIM_CounterMode_Down;
	init.TIM_RepetitionCounter = 0;
	TIM_DeInit(StepperTimer);
	TIM_TimeBaseInit(StepperTimer, &init);
	TIM_CCPreloadControl(StepperTimer, DISABLE);
	TIM_ARRPreloadConfig(StepperTimer, DISABLE);
	TIM_ClearITPendingBit(StepperTimer, TIM_IT_CC1 | TIM_IT_Update);
	TIM_ITConfig(StepperTimer, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
	TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE); // Load new delay on compare match

	// Timer interrupts
	NVIC_InitTypeDef timerIT;
	timerIT.NVIC_IRQChannel = TIM1_CC_IRQn;
	timerIT.NVIC_IRQChannelPreemptionPriority = 0;
	timerIT.NVIC_IRQChannelSubPriority = 0;
	timerIT.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&timerIT);
	timerIT.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn;
	timerIT.NVIC_IRQChannelPreemptionPriority = 0;
	timerIT.NVIC_IRQChannelSubPriority = 0;
	timerIT.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&timerIT);

	// DMA configuration
	DMA_InitTypeDef dma;
	DMA_DeInit(DMAChannel);
	dma.DMA_PeripheralBaseAddr = (uint32_t) TIM1_ARR_Address;
	dma.DMA_MemoryBaseAddr = (uint32_t) Delays;
	dma.DMA_DIR = DMA_DIR_PeripheralDST;
	dma.DMA_BufferSize = sizeof(Delays) / sizeof(Delays[0]);
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_High;
	dma.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMAChannel, &dma);
	DMA_ITConfig(DMAChannel, DMA_IT_HT | DMA_IT_TC, ENABLE);
	DMA_Cmd(DMAChannel, ENABLE);

	// DMA interrupt
	NVIC_InitTypeDef dmaIT;
	dmaIT.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	dmaIT.NVIC_IRQChannelPreemptionPriority = 0;
	dmaIT.NVIC_IRQChannelSubPriority = 1;
	dmaIT.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_Init(&dmaIT);


	// DMA transfer from buffer to CCR1

	// Some testing...
	TIM_SetCompare1(StepperTimer, 100);
	TIM_SetAutoreload(StepperTimer, 0);
	TIM_Cmd(StepperTimer, ENABLE);
	TIM_GenerateEvent(StepperTimer, TIM_EventSource_Update);

}

extern "C" void __attribute__ ((section(".after_vectors")))
TIM1_UP_TIM16_IRQHandler(void)
{
	util::led4_off();
	TIM_ClearITPendingBit(StepperTimer, TIM_IT_Update);
}

extern "C" void __attribute__ ((section(".after_vectors")))
TIM1_CC_IRQHandler(void)
{
	util::led4_on();
	TIM_ClearITPendingBit(StepperTimer, TIM_IT_CC1);
}

extern "C" void __attribute__ ((section(".after_vectors")))
DMA1_Channel5_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_HT5))
	{
		util::led3_on();
	}
	if (DMA_GetITStatus(DMA1_IT_TC5))
	{
		util::led3_off();
	}
	DMA_ClearITPendingBit(DMA1_IT_GL5);
}
