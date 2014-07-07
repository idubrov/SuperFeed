#ifndef __SETTINGS_HPP
#define __SETTINGS_HPP

#include "tui/console.hpp"
#include "tui/widgets.hpp"

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
	settings(tui::console& console) :
			_console(console), _scroll(0), _selected(0)
	{
	}

	void run();
private:
	void redraw();
private:
	tui::console& _console;

	uint16_t _scroll;
	uint16_t _selected;

	const static uint16_t Microsteps[];
	const static setting Options[];
	const static uint16_t OptionsCount;
};

#endif /* __SETTINGS_HPP */
