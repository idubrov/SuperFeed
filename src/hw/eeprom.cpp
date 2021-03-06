#include "stm32f10x_flash.h"
#include "hw/eeprom.hpp"

hw::eeprom::Status hw::eeprom::initialize()
{
	FlashUnlock unlock; // Unlock flash

	int16_t current = find_current();
	for (int16_t page = 0; page < _page_count; page++)
	{
		// Erase all pages except current. Note that although we might have several
		// pages marked as current (in case power was switched off while we were
		// rescuing data), only first one will be preserved.
		if (page != current && erase_page(page) != FLASH_COMPLETE)
			return Error;
	}

	if (current < 0)
	{
		// We don't have current page, mark first page as one.
		if (set_page_status(0, CurrentPage) != FLASH_COMPLETE)
			return Error;
	}
	else
	{
		rescue_if_full(current);
	}
	return Ok;
}

hw::eeprom::Status hw::eeprom::erase_settings()
{
	FlashUnlock unlock; // Unlock flash
	for (int16_t page = 0; page < _page_count; page++)
	{
		if (erase_page(page) != FLASH_COMPLETE)
			return Error;
	}

	// No we don't have current page, mark first page as one.
	if (set_page_status(0, CurrentPage) != FLASH_COMPLETE)
		return Error;
	return Ok;
}

int16_t hw::eeprom::find_current() const
{
	for (int16_t page = 0; page < _page_count; page++)
	{
		if (page_status(page) == CurrentPage)
			return page;
	}
	return -1;
}

bool hw::eeprom::dirty(int16_t page)
{
	for (uint32_t offset = 0; offset < PageSize; offset += 2)
	{
		uint16_t value = *(__I uint16_t*) (_base + (page * PageSize) + offset);
		if (value != 0xffff)
			return true;
	}
	return false;
}

bool hw::eeprom::search(uint32_t start, uint32_t end, uint16_t tag)
{
	for (uint32_t offset = start; offset < end; offset += 4)
	{
		if (*(__I uint16_t*) offset == tag)
			return true;
	}
	return false;
}

FLASH_Status hw::eeprom::rescue_if_full(int16_t page)
{
	// Check if last word of the page was written or not
	uint32_t src_base = _base + page * PageSize;
	uint32_t src_last = src_base + PageSize - 4;
	FLASH_Status status;

	// Page is not full yet
	if (*(__I uint32_t*) src_last == 0xffffffff)
		return FLASH_COMPLETE;

	// Last word is not 0xffffffff, we need to rescue to the next page
	int16_t tgt_page = page + 1;
	if (tgt_page >= _page_count)
		tgt_page -= _page_count;

	// Destination address
	uint32_t tgt_base = _base + tgt_page * PageSize;
	uint32_t tgt_first = tgt_base + 4;
	uint32_t tgt_addr = tgt_first;

	// Start scanning source page from the end (to get the latest value)
	for (uint32_t offset = src_last; offset > src_base; offset -= 4)
	{
		uint16_t tag = *(__I uint16_t*) offset;
		// Entry is empty or value is in the target page already, continue
		if (tag == 0xffff || search(tgt_first, tgt_addr, tag))
			continue;

		// Copy data
		uint16_t data = *(__I uint16_t*) (offset + 2);
		if ((status = FLASH_ProgramHalfWord(tgt_addr + 2, data))
				!= FLASH_COMPLETE)
			return status;
		if ((status = FLASH_ProgramHalfWord(tgt_addr, tag)) != FLASH_COMPLETE)
			return status;
		tgt_addr += 4;
	}

	// Mark target page as current
	if ((status = FLASH_ProgramHalfWord(tgt_base, CurrentPage))
			!= FLASH_COMPLETE)
		return status;

	// Erase source page
	if ((status = FLASH_ErasePage(src_base)) != FLASH_COMPLETE)
		return status;

	return FLASH_COMPLETE;
}

hw::eeprom::Status hw::eeprom::read(uint16_t tag, uint16_t& data) const
{
	int16_t current = find_current();
	if (current < 0)
		return NoPage;

	uint32_t base = _base + current * PageSize;
	uint32_t last = base + PageSize - 4;
	// Start scanning source page from the end (to get the latest value)
	for (uint32_t offset = last; offset > base; offset -= 4)
	{
		uint16_t t = *(__I uint16_t*) offset;
		if (tag == t)
		{
			data = *(__I uint16_t*) (offset + 2);
			return Ok;
		}
	}
	return NotFound;
}

hw::eeprom::Status hw::eeprom::write(uint16_t tag, uint16_t data)
{
	FlashUnlock unlock;

	int16_t current = find_current();
	if (current < 0)
		return NoPage;

	uint32_t base = _base + current * PageSize;
	uint32_t last = base + PageSize - 4;
	for (uint32_t offset = base + 4; offset <= last; offset += 4)
	{
		// Note: check both tag and data, in some cases only data could be written
		uint32_t t = *(__I uint32_t*) offset;
		if (t == 0xffffffff)
		{
			// Write data first, so in case of power lost we are safe
			if (FLASH_ProgramHalfWord(offset + 2, data) != FLASH_COMPLETE)
				return Error;
			if (FLASH_ProgramHalfWord(offset, tag) != FLASH_COMPLETE)
				return Error;
			break;
		}
	}

	if (rescue_if_full(current) != FLASH_COMPLETE)
		return Error;
	return Ok;
}
