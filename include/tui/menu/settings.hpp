#ifndef __SETTINGS_HPP
#define __SETTINGS_HPP

#include "config.hpp"
#include "input.hpp"
#include "tui/inputs.hpp"

struct setting
{
	char const* title;
	uint16_t tag;
	uint16_t default_value;
	uint16_t const* values;
	uint16_t values_cnt;
};

class settings
{
public:
	settings(input& input, lcd::HD44780& lcd) :
			_input(input), _lcd(lcd), _scroll(0), _selected(0)
	{
	}

	void run();
private:
	void redraw();
private:
	input& _input;
	lcd::HD44780& _lcd;

	uint16_t _scroll;
	uint16_t _selected;

	const static uint16_t Microsteps[];
	const static setting Options[];
	const static uint16_t OptionsCount;
};

#endif /* __SETTINGS_HPP */
