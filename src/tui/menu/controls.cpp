#include <cstring>

#include "tui/menu/controls.hpp"
#include "simstream.hpp"

using namespace hw;

bool tui::menu::spinner::activate(console& console, unsigned y)
{
	unsigned x = 4 + std::strlen(label);
	unsigned b = util::digits(max) + 1;

	auto& lcd = console.lcd();
	auto state(console.guard_state());

	console.set_encoder_limit(max - min + 1);

	uint16_t value = def_value;
	eeprom.read(tag, value);

	lcd << lcd::position(x, y) << blanks(b);
	lcd << lcd::position(x, y) << value << (char) 3;
	while (true)
	{
		auto ev = console.read();
		if (ev.kind == console::ButtonPressed
				&& ev.key == console::EncoderButton)
			break;

		if (ev.kind == console::EncoderMove)
		{
			value = ev.position + 1;
			lcd << lcd::position(x, y) << blanks(b);
			lcd << lcd::position(x, y) << value << (char) 3;
		}
	}

	uint16_t current = def_value;
	eeprom.read(tag, current);
	if (current != value)
	{
		eeprom.write(tag, value);
	}
	return true;
}

bool tui::menu::numeric::activate(console& console, unsigned y)
{
	unsigned max_len = util::digits(max);
	unsigned x = 4 + std::strlen(label);

	auto& lcd = console.lcd();

	uint16_t value = def_value;
	eeprom.read(tag, value);

	lcd << lcd::position(x, y) << blanks(max_len + 1);
	lcd << lcd::position(x, y) << value;

	lcd.display(hw::lcd::DisplayOn, hw::lcd::CursorOn, hw::lcd::BlinkOn);
	while (1)
	{

	}

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
