#include "encoder.hpp"
#include "util.hpp"
#include "systick.hpp"

using namespace ::cfg::encoder;

encoder::encoder(GPIO_TypeDef* port, TIM_TypeDef* timer, uint16_t button_pin, uint16_t encoder_pins) :
		_port(port), _timer(timer), _button_pin(button_pin), _encoder_pins(encoder_pins),
		_state(0), _pressed(false)
{
	// Button
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = _button_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(_port, &GPIO_InitStructure);

	// Quadrature encoder on timer
	GPIO_InitTypeDef GPIO_InitStructure2;
	GPIO_InitStructure2.GPIO_Pin = _encoder_pins;
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(_port, &GPIO_InitStructure2);

	// Configure alternative functions
	// We don't actually need to remap anything right now

	// Configure timer as rotary encoder
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(_timer, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(_timer, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(_timer, ENABLE);

	systick::instance().bind(this, &encoder::scan);
}

void encoder::scan()
{
	bool unpressed = _port->IDR & _button_pin;
	_state = (_state << 1) | (unpressed ? 0 : 1);
	if (_state == UINT32_MAX) {
		_pressed = true;
	} else if (_state == 0) {
		_pressed = false;
	}
}

