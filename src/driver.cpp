#include "driver.hpp"

using namespace ::cfg::driver;

driver::driver()
{
	// Control port
	RCC_APB2PeriphClockCmd(PortClock, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = StepPin | DirPin | EnablePin | ResetPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Port, &GPIO_InitStructure);

	// Start in reset mode
	reset();
}
