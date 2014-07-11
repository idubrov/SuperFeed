#ifndef __SAMPLER_HPP
#define __SAMPLER_HPP

#include <algorithm>
#include <atomic>

#include "hw/spindle.hpp"
#include "tui/console.hpp"

namespace tui
{
namespace menu
{

class sampler
{
public:
	sampler(hw::spindle& spindle, uint32_t flash_start = FLASH_BASE + 125 * 0x400) :
			spindle(spindle), flash_start(flash_start),
			captured(0xffff), buffer_size(BufferCapacity)
	{
		static_assert(BufferCapacity * sizeof(buffer[0]) <= 1024,
				"Buffer must fit into one flash page");
		std::fill_n(buffer, BufferCapacity, 0);
	}
	sampler(sampler const& other) : spindle(other.spindle),
			flash_start(other.flash_start), captured(0xffff),
			buffer_size(other.buffer_size)
	{
		std::fill_n(buffer, BufferCapacity, 0);
	}

	void activate(tui::console& console);
	void print_label(tui::console& console)
	{
		console.lcd() << "Spindle sampling";
	}
	void index_pulse_handler();
private:
	FLASH_Status write_flash();
private:
	hw::spindle& spindle;
	uint32_t const flash_start;

	constexpr static unsigned BufferCapacity = 512;
	std::atomic_uint_fast16_t captured;
	unsigned buffer_size;
	uint16_t buffer[BufferCapacity]; // Buffer for temporary storage
};
}
}

#endif /* __SAMPLER_HPP */
