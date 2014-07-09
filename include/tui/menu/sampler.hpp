#ifndef __SAMPLER_HPP
#define __SAMPLER_HPP

#include "tui/console.hpp"
#include "hw/spindle.hpp"
#include <cstdint>
#include <algorithm>
#include <atomic>

namespace tui
{
namespace menu
{

class sampler
{
public:
	sampler(tui::console& console, hw::spindle& spindle, uint32_t flash_start) :
			_console(console), _spindle(spindle), _flash_start(flash_start),
			_captured(), _buffer_size(BufferCapacity)
	{
		static_assert(BufferCapacity * sizeof(_buffer[0]) <= 1024,
				"Buffer must fit into one flash page");
		std::fill_n(_buffer, BufferCapacity, 0);

		// Capturing is stopped
		_captured = 0xffff;
	}

	void run();
	void index_pulse_handler();
private:
	FLASH_Status write_flash();
private:
	tui::console& _console;
	hw::spindle& _spindle;
	uint32_t const _flash_start;

	constexpr static unsigned BufferCapacity = 512;
	std::atomic_uint_fast16_t _captured;
	unsigned _buffer_size;
	uint16_t _buffer[BufferCapacity]; // Buffer for temporary storage
};
}
}

#endif /* __SAMPLER_HPP */