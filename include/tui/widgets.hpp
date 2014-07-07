#ifndef __WIDGETS_HPP
#define __WIDGETS_HPP

#include "tui/console.hpp"

namespace tui
{
uint16_t spinner(console& console, uint8_t x, uint8_t y,
		uint16_t min, uint16_t max, uint16_t current);
}

#endif /* __INPUTS_HPP */
