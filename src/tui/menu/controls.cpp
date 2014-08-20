#include <cstring>

#include "tui/menu/controls.hpp"
#include "simstream.hpp"

using namespace hw;

bool tui::menu::spinner::activate(console& console, unsigned y)
{
	unsigned x = 4 + std::strlen(setting.label);
	unsigned b = util::digits(setting.max) + 1;

	auto& lcd = console.lcd();
	auto value = setting.ranged(eeprom);
	lcd << lcd::position(x, y) << blanks(b);
	lcd << lcd::position(x, y) << value << (char) 3;
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::ButtonPressed
				&& ev.key == console::SelectButton)
			break;

		bool updated = false;
		if (ev.kind == console::EncoderMove)
		{
			value += ev.delta;
			updated = true;
		}
		else if (ev.kind == console::ButtonPressed
				&& ev.key == console::RightButton)
		{
			value += 1;
			updated = true;
		}
		else if (ev.kind == console::ButtonPressed
				&& ev.key == console::LeftButton)
		{
			value += -1;
			updated = true;
		}

		if (updated)
		{
			lcd << lcd::position(x, y) << blanks(b);
			lcd << lcd::position(x, y) << value << (char) 3;
		}
	}

	setting.set(eeprom, value.get());
	return true;
}

bool tui::menu::numeric::activate(console& console, unsigned y)
{
	unsigned b = util::digits(setting.max) + 1;
	unsigned x = 4 + std::strlen(setting.label);

	auto& lcd = console.lcd();

	bool empty = false;
	uint16_t value = setting.get(eeprom);

	lcd.display(hw::lcd::DisplayOn, hw::lcd::CursorOn, hw::lcd::BlinkOn);
	lcd << lcd::position(x, y) << blanks(b);
	lcd << lcd::position(x, y) << value;
	while (true)
	{
		auto ev = console.read();
		if (ev.kind != console::ButtonPressed)
			continue;

		if (ev.key == console::SelectButton)
		{
			if (value < setting.min)
				value = setting.min;
			break;
		}
		else if (ev.key == console::DeleteButton)
		{
			if (value <= 9)
				empty = true;
			value /= 10;
		}
		else if (ev.key >= '0' && ev.key <= '9')
		{
			uint32_t newValue = value * 10 + (ev.key - '0');
			if (newValue <= setting.max)
			{
				empty = false;
				value = newValue;
			}
		}

		lcd << lcd::position(x, y) << blanks(b) << lcd::position(x, y);
		if (!empty)
			lcd << value;
	}

	setting.set(eeprom, value);

	lcd.display(hw::lcd::DisplayOn, hw::lcd::CursorOff, hw::lcd::BlinkOff);
	return true;
}

bool tui::menu::toggle::activate(console&, unsigned)
{
	bool value = def_value;
	uint16_t raw;
	if (eeprom.read(tag, raw) == eeprom::Ok)
	{
		value = static_cast<bool>(raw);
	}
	eeprom.write(tag, value ? 0 : 1); // Write inverted
	return true;
}

bool tui::menu::erase_settings::activate(console& console, unsigned)
{
	auto& lcd = console.lcd();
	lcd << hw::lcd::clear() << "Erasing...";
	if (eeprom.erase_settings() != hw::eeprom::Ok)
	{
		lcd << hw::lcd::clear() << "EEPROM error.";
		while (1)
			;
	}
	lcd << hw::lcd::clear() << "Settings erased.";
	lcd << hw::lcd::position(0, 1) << "Press . to exit.";
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::ButtonPressed)
			break;
	}
	return true;
}
