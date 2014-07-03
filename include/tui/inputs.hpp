#ifndef __INPUTS_HPP
#define __INPUTS_HPP

#include "input.hpp"
#include "hw/lcd.hpp"

namespace tui
{

uint16_t spinner(hw::lcd::HD44780& lcd, input& input, uint8_t x, uint8_t y);

}

#endif /* __INPUTS_HPP */
