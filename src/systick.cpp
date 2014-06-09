#include "systick.hpp"

systick::TickHandler systick::_handlers[MaxHandlers];

extern "C"
void __attribute__ ((section(".after_vectors")))
SysTick_Handler(void)
{
	systick::tick();
}

// Configuration
systick::systick()
{
	// Get system frequency
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	// Configure systick, we use it for software debouncer
	// Run it at 10kHz (0.1msec delay)
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 8 / 10000);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}
