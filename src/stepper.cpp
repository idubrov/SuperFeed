#include "stepper.hpp"
#include "lcd.hpp"

using namespace ::cfg::stepper;

#define TIM1_CCR1_Address    0x40012C34

uint16_t Delays[] = {100, 200, 1100, 1200, 1300, 1400, 2100, 2200, 2300, 2400, 2500, 2600};

void stepper::initialize()
{
	TIM_TimeBaseInitTypeDef init;
	init.TIM_Prescaler = 23999; // 1ms period
	init.TIM_Period = 3000;
	init.TIM_ClockDivision = TIM_CKD_DIV1;
	init.TIM_CounterMode = TIM_CounterMode_Up;
	init.TIM_RepetitionCounter = 0;
	TIM_DeInit(StepperTimer);
	TIM_TimeBaseInit(StepperTimer, &init);
	TIM_CCPreloadControl(StepperTimer, DISABLE);
	TIM_ClearITPendingBit(StepperTimer, TIM_IT_CC1);
	TIM_ITConfig(StepperTimer, TIM_IT_CC1, ENABLE);
	TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE); // Load new delay on compare match

	// DMA configuration
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel2);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) TIM1_CCR1_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) Delays;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = sizeof(Delays) / sizeof(Delays[0]);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel2, ENABLE);


	// Master interrupt
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// DMA transfer from buffer to CCR1

	// Some testing...
	TIM_GenerateEvent(StepperTimer, TIM_EventSource_CC1);
	TIM_Cmd(StepperTimer, ENABLE);
}

extern "C" void __attribute__ ((section(".after_vectors")))
TIM1_CC_IRQHandler(void)
{
	static bool flag = false;
	TIM_ClearITPendingBit(StepperTimer, TIM_IT_CC1);
	if (flag)
	{
		util::led4_on();
	}
	else
	{
		util::led4_off();
	}
	flag = !flag;
}
