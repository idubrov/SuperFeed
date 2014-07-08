#include "hw/buttons.hpp"

void hw::buttons::initialize()
{
	// 3 buttons
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 7 << _first_pin; // We use 3 consecutive pins.
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(_port, &GPIO_InitStructure);
}
