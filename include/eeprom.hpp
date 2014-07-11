#ifndef __EEPROM_HPP
#define __EEPROM_HPP

#include "config.hpp"

namespace hw
{
class eeprom
{
public:
	enum Status
	{
		Ok = 0,
		NoPage,
		NotFound,
		Error
	};
public:
	constexpr eeprom(uint32_t base, uint16_t page_count) :
			_base(base), _page_count(page_count)
	{
	}
	Status initialize();

	Status read(uint16_t tag, uint16_t& data);
	Status write(uint16_t tag, uint16_t data);

private:
	FLASH_Status rescue_if_full(int16_t page);
	int16_t find_current();

	uint16_t page_status(int16_t page)
	{
		return *(__I uint16_t*) (_base + page * PageSize);
	}

	FLASH_Status set_page_status(int16_t page, uint16_t status)
	{
		return FLASH_ProgramHalfWord(_base + page * PageSize, status);
	}

	/// Erase given page, but only if it's "dirty" (has any non-'1' bit).
	FLASH_Status erase_page(int16_t page)
	{
		return dirty(page) ?
				FLASH_ErasePage(_base + page * PageSize) : FLASH_COMPLETE;
	}

	bool dirty(int16_t page);
	bool search(uint32_t start, uint32_t end, uint16_t tag);

private:
	uint32_t const _base;
	uint16_t const _page_count;

	// Empty page, could be used for rescueing data from the current page.
	// Once rescue process is started, the page is marked as 'rescue target'
	static constexpr uint16_t ErasedPage = 0xffff;
	// This is the current page we are writing data to.
	static constexpr uint16_t CurrentPage = 0xABCD;

#if defined (STM32F10X_LD) || defined (STM32F10X_LD_VL) || \
	defined (STM32F10X_MD) || defined(STM32F10X_MD_VL)
	static constexpr uint32_t PageSize = 0x400;
	// 1KByte
#elif defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || \
	defined (STM32F10X_XL) || defined (STM32F10X_CL)
	static constexpr uint32_t PageSize = 0x800; // 2KByte
#endif
	class FlashUnlock
	{
	public:
		FlashUnlock()
		{
			FLASH_Unlock();
		}
		~FlashUnlock()
		{
			FLASH_Lock();
		}
	};
};
}

#endif /* __EEPROM_HPP */
