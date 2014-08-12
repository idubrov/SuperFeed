#include "tui/menu/sampler.hpp"
#include "util.hpp"

using namespace ::hw;

bool tui::menu::sampler::activate(tui::console& console, unsigned)
{
	auto state = console.guard_state();

	console.set_encoder(BufferCapacity, buffer_size - 1);
	auto& lcd = console.lcd();
	lcd.clear();
	while (true)
	{
		auto ev = console.read();
		lcd << lcd::position(0, 0) << "Spindle delay: "
				<< format<10>(spindle.raw_delay(), 5);

		// Get local copy
		unsigned cap = captured;
		if (cap < buffer_size)
		{
			lcd << lcd::position(0, 1) << "Sampled " << cap << " of " << buffer_size;
			lcd << lcd::position(0, 3) << "Press \xa5 to stop";
			if (ev.kind == console::ButtonPressed
					&& ev.key == console::SelectButton)
			{
				captured = 0xffff; // Stop capturing
				lcd.clear();
			}
		}
		else if (cap == buffer_size)
		{
			lcd << lcd::position(0, 1) << "Writing to flash... ";
			FLASH_Status status = write_flash();
			if (status != FLASH_COMPLETE)
			{
				lcd << lcd::position(0, 1);
				lcd << "                    ";
				lcd << lcd::position(0, 1);
				lcd << "Error: " << status;
				while (1)
					;
			}
			captured = 0xffff;
			lcd.clear();
		}
		else if (cap == 0xffff)
		{
			lcd << lcd::position(0, 1) << "Capture size: "
					<< format<10>(buffer_size, 3);
			lcd << lcd::position(0, 2) << "Press \xa5 to capture";
			lcd << lcd::position(0, 3) << "Other key to exit";
			if (ev.kind == console::ButtonPressed
					&& ev.key == console::SelectButton)
			{
				captured = 0; // Start capturing
				lcd.clear();
			}
			else if (ev.kind == console::EncoderMove)
			{
				buffer_size = ev.position + 1;
			}
			else if (ev.kind == console::ButtonPressed)
			{
				// Any other button -- exit
				break;
			}
		}
	}
	return true;
}

FLASH_Status tui::menu::sampler::write_flash()
{
	FLASH_Unlock();
	FLASH_Status status;
	if ((status = FLASH_ErasePage(flash_start)) != FLASH_COMPLETE)
	{
		return status;
	}
	unsigned cnt = buffer_size;
	for (unsigned i = 0; i < cnt; i++)
	{
		uint32_t offset = flash_start + i * sizeof(uint16_t);
		if ((status = FLASH_ProgramHalfWord(offset, buffer[i]))
				!= FLASH_COMPLETE)
		{
			return status;
		}
	}
	FLASH_Lock();
	return FLASH_COMPLETE;
}

void tui::menu::sampler::index_pulse_handler()
{
	unsigned pos = captured;
	if (pos < buffer_size)
	{
		buffer[pos] = spindle.raw_delay();

		// Make sure we update captured after writing to buffer
		captured.store(pos + 1);
	}
}
