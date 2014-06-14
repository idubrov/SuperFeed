#include "stepper.hpp"
#include "lcd.hpp"

using namespace ::cfg::stepper;

#define TIM4_ARR_Address    0x4000082C

uint16_t Delays[] =
{ 1000, 500, 500 };

void setup_slave_timer()
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
	TIM_ClearITPendingBit(OutputTimer, TIM_IT_CC1 | TIM_IT_Update);
	TIM_ITConfig(OutputTimer, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
	TIM_SelectOnePulseMode(OutputTimer, TIM_OPMode_Single);
	TIM_SetCompare1(OutputTimer, 1);

	TIM_SelectInputTrigger(OutputTimer, TIM_TS_ITR3); // TIM4 (master) -> TIM1 (slave)
	TIM_SelectSlaveMode(OutputTimer, TIM_SlaveMode_Trigger);

	// Output timer interrupts
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
}

void setup_master_timer()
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

	TIM_TimeBaseInit(StepperTimer, &init);
	TIM_SelectOnePulseMode(StepperTimer, TIM_OPMode_Repetitive);

	// DMA is configured to transfer on update
	TIM_ClearITPendingBit(OutputTimer, TIM_IT_Update);
	TIM_ITConfig(StepperTimer, TIM_IT_Update, ENABLE);
	TIM_DMACmd(StepperTimer, TIM_DMA_Update, ENABLE); // Load new delay on compare match


	// Output timer interrupts
	NVIC_InitTypeDef timerIT;
	timerIT.NVIC_IRQChannel = TIM4_IRQn;
	timerIT.NVIC_IRQChannelPreemptionPriority = 0;
	timerIT.NVIC_IRQChannelSubPriority = 0;
	timerIT.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&timerIT);
}

void setup_master_dma()
{
	// DMA configuration
	DMA_InitTypeDef dma;
	DMA_DeInit(DMAChannel);
	dma.DMA_PeripheralBaseAddr = (uint32_t) TIM4_ARR_Address;
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
	DMA_ITConfig(DMAChannel, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMAChannel, ENABLE);

	// DMA interrupt
	NVIC_InitTypeDef dmaIT;
	dmaIT.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	dmaIT.NVIC_IRQChannelPreemptionPriority = 0;
	dmaIT.NVIC_IRQChannelSubPriority = 1;
	dmaIT.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&dmaIT);
}

void stepper::initialize()
{
	TIM_DeInit(OutputTimer);
	TIM_DeInit(StepperTimer);

	setup_slave_timer();
	setup_master_timer();
	setup_master_dma();

	// Load data and start timer!
	TIM_GenerateEvent(StepperTimer, TIM_EventSource_Update);
	TIM_Cmd(StepperTimer, ENABLE);
}

extern "C" void __attribute__ ((section(".after_vectors")))
TIM1_UP_TIM16_IRQHandler(void)
{
	util::led4_off();
	TIM_ClearITPendingBit(OutputTimer, TIM_IT_Update);
}

extern "C" void __attribute__ ((section(".after_vectors")))
TIM1_CC_IRQHandler(void)
{
	util::led4_on();
	TIM_ClearITPendingBit(OutputTimer, TIM_IT_CC1);
}

static bool stop = false;
extern "C" void __attribute__ ((section(".after_vectors")))
TIM4_IRQHandler(void)
{
	if (stop) {
		TIM_Cmd(StepperTimer, DISABLE);
	}
	static bool flag = true;
	TIM_ClearITPendingBit(StepperTimer, TIM_IT_Update);
	if (flag) {
		util::led3_on();
	} else {
		util::led3_off();
	}
	flag = !flag;

}

extern "C" void __attribute__ ((section(".after_vectors")))
DMA1_Channel7_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_TC7))
	{
		//stop = true;
	}
	DMA_ClearITPendingBit(DMA1_IT_GL7);
}

