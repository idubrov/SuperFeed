#ifndef __ENCODER_HPP
#define __ENCODER_HPP

#include "stm32f10x.h"

namespace hw {

class encoder {
public:
	constexpr encoder(GPIO_TypeDef* port, TIM_TypeDef* timer, uint16_t button_pin, uint16_t encoder_pins) :
			_port(port), _timer(timer), _button_pin(button_pin), _encoder_pins(encoder_pins),
			_state(0), _pressed(false)
	{
	}

	encoder(encoder const&) = delete;

	void initialize();

	inline bool pressed() const {
		return _pressed;
	}

	inline uint16_t position() const
	{
		return _timer->CNT >> 1;
	}

	inline void position(uint16_t pos) const {
		_timer->CNT = (pos << 1);
	}

	inline void limit(uint16_t limit) const {
		_timer->ARR = limit << 1;
		_timer->CNT = 0;
	}
private:
	// Should be called from SysTick interrupt handler.
	void scan();

private:
	GPIO_TypeDef* const _port;
	TIM_TypeDef* const _timer;
	uint16_t const _button_pin;
	uint16_t const _encoder_pins;

	uint32_t _state; // Current debounce status
	volatile bool _pressed; // Current button state
};
}

#endif /* __ENCODER_HPP */
