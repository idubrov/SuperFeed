#ifndef __SETTINGS_HPP
#define __SETTINGS_HPP

#include "tui/console.hpp"
#include "tui/widgets.hpp"

namespace tui
{
namespace menu
{
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
	settings() : _scroll(0), _selected(0)
	{
	}

	void activate(tui::console& console, unsigned);
	void print_label(tui::console& console)
	{
		console.lcd() << "Settings";
	}
private:
	void redraw(tui::console& console);
private:
	uint16_t _scroll;
	uint16_t _selected;

	const static uint16_t Microsteps[];
	const static setting Options[];
	const static uint16_t OptionsCount;
};
}
}
#endif /* __SETTINGS_HPP */
