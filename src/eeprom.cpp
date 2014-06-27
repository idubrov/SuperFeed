#include "eeprom.hpp"
#include "stm32f10x_flash.h"

eeprom::Status eeprom::initialize()
{
	Unlock unlock; // Unlock flash

	int16_t current = -1;
	for (int16_t page = 0; page < _page_count; page++)
	{
		uint16_t status = page_status(page);
		if (status == CurrentPage)
		{
			if (current == -1)
			{
				// Found page marked as current
				current = page;
			}
			else if (current == -2)
			{
				// Got at least two pages marked as current, erase all pages.
				if (erase_page(page) != FLASH_COMPLETE)
					return Error;
			}
			else
			{
				// We've got at least two pages, marked as current. Something is wrong.
				// Clear all the pages and reinitialize from scratch.
				if (erase_page(current) != FLASH_COMPLETE)
					return Error;
				if (erase_page(page) != FLASH_COMPLETE)
					return Error;
				current = -2;
			}
		}
		else if (status == ErasedPage || status == TargetPage)
		{
			if (erase_page(page) != FLASH_COMPLETE)
				return Error;
		}
	}

	if (current < 0)
	{
		// We don't have current page, make first page one.
		if (FLASH_ProgramHalfWord(_base, CurrentPage) != FLASH_COMPLETE)
			return Error;
	} else
	{
		// Check if current page is full
		// FIXME:...
	}
	return Ok;
}

bool eeprom::dirty(int16_t page)
{
	for (uint32_t offset = 0; offset < PageSize; offset += 2)
	{
		uint16_t value = *(__IO uint16_t*) (_base + (page * PageSize) + offset);
		if (value != 0xffff)
			return true;
	}
	return false;
}
