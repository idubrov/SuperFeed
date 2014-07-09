#ifndef __SAMPLER_HPP
#define __SAMPLER_HPP

#include "tui/console.hpp"
#include "hw/spindle.hpp"
#include <algorithm>

namespace tui
{
namespace menu
{

class sampler
{
public:
	sampler(tui::console& console, hw::spindle& spindle, uint32_t start,
			uint16_t buffer_size) :
			_console(console), _spindle(spindle), _flash_start(start), _buffer_size(
					buffer_size), _captured(0), _capturing(false)
	{
		constexpr unsigned BufferSize = sizeof(_buffer) / sizeof(_buffer[0]);
		assert_param(_buffer_size < BufferSize);
		std::fill_n(_buffer, BufferSize, 0);
	}

	void run();
	void index_pulse_handler();
private:
	FLASH_Status write_flash();
private:
	tui::console& _console;
	hw::spindle& _spindle;
	uint32_t const _flash_start;
	uint16_t const _buffer_size;

	uint16_t volatile _captured;
	bool volatile _capturing;

	uint16_t volatile _buffer[512];
};
}
}

#endif /* __SAMPLER_HPP */
