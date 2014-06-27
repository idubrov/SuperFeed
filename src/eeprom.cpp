#include "eeprom.hpp"
#include "stm32f10x_flash.h"

eeprom::Status eeprom::initialize()
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

int16_t eeprom::find_current()
{
	for (int16_t page = 0; page < _page_count; page++)
	{
		if (page_status(page) == CurrentPage)
			return page;
	}
	return -1;
}

bool eeprom::dirty(int16_t page)
{
	for (uint32_t offset = 0; offset < PageSize; offset += 2)
	{
		uint16_t value = *(__I uint16_t*) (_base + (page * PageSize) + offset);
		if (value != 0xffff)
			return true;
	}
	return false;
}

FLASH_Status eeprom::rescue_if_full(int16_t page)
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
	uint32_t tgt_first = _base + tgt_page * PageSize + 4;
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
	if ((status = FLASH_ProgramHalfWord(src_base, CurrentPage))
			!= FLASH_COMPLETE)
		return status;

	// Erase source page
	if ((status = FLASH_ErasePage(src_base)) != FLASH_COMPLETE)
		return status;

	return FLASH_COMPLETE;
}

eeprom::Status eeprom::read(uint16_t tag, uint16_t& data)
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

eeprom::Status eeprom::write(uint16_t tag, uint16_t data)
{
	int16_t current = find_current();
	if (current < 0)
		return NoPage;

	uint32_t base = _base + current * PageSize;
	uint32_t last = base + PageSize - 4;
	// Start scanning source page from the end (to get the latest value)
	for (uint32_t offset = base + 4; offset <= last; offset += 4)
	{
		uint16_t t = *(__I uint16_t*) offset;
		if (t == 0xffff)
		{
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
