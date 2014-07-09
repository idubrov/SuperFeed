#include "tui/menu/sampler.hpp"
#include "util.hpp"

using namespace ::hw;
using namespace ::tui;
using namespace ::tui::menu;

void sampler::run()
{
	auto& lcd = _console.lcd();
	constexpr char const* SpindleSpeed = "Spindle speed: ";
	constexpr unsigned SpindleSpeedLen = util::c_strlen(SpindleSpeed);
	lcd.clear();

	// Draw text
	lcd << lcd::position(0, 0) << SpindleSpeed;

	while (true)
	{
		auto ev = _console.read();
		lcd << lcd::position(SpindleSpeedLen, 0)
				<< format<10>(_spindle.raw_speed(), 5);
		lcd << lcd::position(0, 1);
		if (_capturing)
		{
			lcd << _captured << " of " << _buffer_size;

			if (_captured == _buffer_size)
			{
				_capturing = false;
				lcd << lcd::position(0, 1);
				lcd << "Writing to flash... ";
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

				_captured = 0;
			}
		}
		else
		{
			lcd << "Press to capture";
			if (ev.kind == console::ButtonPressed
					&& ev.key == console::EncoderButton)
			{
				lcd << lcd::position(0, 1);
				lcd << "                    ";

				_captured = 0;
				_capturing = true;
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
	for (uint16_t i = 0; i < _buffer_size; i++)
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
	if (_capturing && _captured < _buffer_size)
	{
		_buffer[_captured++] = _spindle.raw_speed();
	}
}
