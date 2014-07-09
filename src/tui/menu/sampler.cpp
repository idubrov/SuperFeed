#include "tui/menu/sampler.hpp"
#include "util.hpp"

using namespace ::hw;
using namespace ::tui;
using namespace ::tui::menu;

void sampler::run()
{
	auto state = _console.guard_state();

	_console.set_encoder_limit(BufferCapacity - 1);
	auto& lcd = _console.lcd();
	lcd.clear();
	while (true)
	{
		auto ev = _console.read();
		lcd << lcd::position(0, 0) << "Spindle speed: " << format<10>(_spindle.raw_speed(), 5);

		if (_captured < _buffer_size)
		{
			lcd << lcd::position(0, 1) << _captured << " of " << _buffer_size;
		}
		else if (_captured == _buffer_size)
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
			_captured = 0xffff;
			lcd.clear();
		}
		else if (_captured == 0xffff)
		{
			lcd << lcd::position(0, 1) << "Capture size: " << format<10>(_buffer_size, 3);
			lcd << lcd::position(0, 2) << "Press to capture";
			if (ev.kind == console::ButtonPressed
					&& ev.key == console::EncoderButton)
			{
				_captured = 0; // Start capturing
				lcd.clear();
			} else if (ev.kind == console::EncoderMove)
			{
				_buffer_size = ev.position + 1;
			}
		}
	}
}

FLASH_Status sampler::write_flash()
{
	FLASH_Unlock();
	FLASH_Status status;
	if ((status = FLASH_ErasePage(_flash_start)) != FLASH_COMPLETE)
	{
		return status;
	}
	uint16_t cnt = _buffer_size;
	for (uint16_t i = 0; i < cnt; i++)
	{
		uint32_t offset = _flash_start + i * sizeof(uint16_t);
		if ((status = FLASH_ProgramHalfWord(offset, _buffer[i]))
				!= FLASH_COMPLETE)
		{
			return status;
		}
	}
	FLASH_Lock();
	return FLASH_COMPLETE;
}

void sampler::index_pulse_handler()
{
	if (_captured < _buffer_size)
	{
		uint16_t pos = _captured;
		_buffer[pos] = _spindle.raw_speed();
		_captured = pos + 1; // Make sure we update _captured after writing to buffer
	}
}
