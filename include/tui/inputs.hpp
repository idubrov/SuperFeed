#ifndef __INPUTS_HPP
#define __INPUTS_HPP

#include "input.hpp"
#include "HD44780.hpp"

namespace tui
{

uint16_t spinner(lcd::HD44780& lcd, input& input, uint8_t x, uint8_t y);

}

#endif /* __INPUTS_HPP */
