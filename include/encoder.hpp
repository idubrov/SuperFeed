#ifndef __ENCODER_HPP
#define __ENCODER_HPP

#include "config.hpp"

class encoder {
public:
	static inline bool pressed() {
		return _pressed;
	}

	// Should be called from SysTick interrupt handler.
	static inline void scan()
	{
		bool unpressed = ::cfg::encoder::Port->IDR & ::cfg::encoder::ButtonPin;
		_state = (_state << 1) | (unpressed ? 0 : 1);
		if (_state == UINT32_MAX) {
			_pressed = true;
		} else if (_state == 0) {
			_pressed = false;
		}
	}

	static inline uint16_t position()
	{
		return (::cfg::encoder::Timer->CNT >> 1);
	}

	static inline void position(uint16_t pos) {
		::cfg::encoder::Timer->CNT = (pos << 1);
	}

	static inline void limit(uint16_t limit) {
		::cfg::encoder::Timer->ARR = (limit << 1);
		::cfg::encoder::Timer->CNT = 0;
	}

private:
	static uint32_t _state;
	static volatile bool _pressed;

	static encoder g_instance;
	encoder();
};

#endif /* __ENCODER_HPP */
