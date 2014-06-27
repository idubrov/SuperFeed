#ifndef __EEPROM_HPP
#define __EEPROM_HPP

#include "config.hpp"

class eeprom
{
public:
	enum Status
	{
		Ok = 0, Error
	};
public:
	constexpr eeprom(uint32_t base, uint16_t page_count) :
			_base(base), _page_count(page_count)
	{
	}
	Status initialize();

private:
	void rescue_data();
	uint16_t find_page();

	uint16_t page_status(int16_t page)
	{
		return *(__IO uint16_t*) (_base + (page * PageSize));
	}

	FLASH_Status eeprom::erase_page(int16_t page)
	{
		return dirty(page) ?
				FLASH_ErasePage(_base + page * PageSize) : FLASH_COMPLETE;
	}

	bool dirty(int16_t page);

private:
	uint32_t const _base;
	uint16_t const _page_count;

	// Empty page, could be used for rescueing data from the current page.
	// Once rescue process is started, the page is marked as 'rescue target'
	static constexpr uint16_t ErasedPage = 0xffff;
	// This is the page being used to rescue data from the current page.
	// During the initialization, all such pages are erased.
	static constexpr uint16_t TargetPage = 0xeeee;
	// This is the current page we are writing data to.
	static constexpr uint16_t CurrentPage = 0x0000;

#if defined (STM32F10X_LD) || defined (STM32F10X_LD_VL) || \
	defined (STM32F10X_MD) || defined(STM32F10X_MD_VL)
	static constexpr uint32_t PageSize = 0x400;
	// 1KByte
#elif defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || \
	defined (STM32F10X_XL) || defined (STM32F10X_CL)
	static constexpr uint32_t PageSize = 0x800; // 2KByte
#endif
	class Unlock
	{
	public:
		Unlock()
		{
			FLASH_Unlock();
		}
		~Unlock()
		{
			FLASH_Lock();
		}
	};
};

#endif /* __EEPROM_HPP */
