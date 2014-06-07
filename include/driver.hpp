#ifndef __DRIVER_HPP
#define __DRIVER_HPP

#include "config.hpp"
#include "util.hpp"

class driver
{
public:
	enum Direction
	{
		UP = 0, DOWN = 1
	};

	static inline void step()
	{
		// FIXME: For CN0162 setup time is 10usec at least, need to check...

		using namespace ::cfg::driver;
		Port->BSRR = StepPin;
		// Wait at least 75ns (for IMS483). Each NOP should be ~40ns (1/24000000 sec)
		__NOP();
		__NOP();

		// for CN0162, wait time should be 300ns at least
		//__NOP(); __NOP(); __NOP(); __NOP();
		//__NOP(); __NOP(); __NOP(); __NOP();

		Port->BRR = StepPin;
	}

	static inline void direction(Direction dir)
	{
		using namespace ::cfg::driver;
		if (dir)
		{
			Port->BSRR = DirPin;
		}
		else
		{
			Port->BRR = DirPin;
		}
	}

	static inline void reset()
	{
		using namespace ::cfg::driver;
		Port->BRR = ResetPin;
	}

	static inline void release() {
		using namespace ::cfg::driver;
		// Minimum reset pulse width is 500nS, so let's wait 1 usec here.
		util::delay_us(1);
		Port->BSRR = ResetPin;
	}

	static inline void enable()
	{
		using namespace ::cfg::driver;
		Port->BSRR = EnablePin;
	}

	static inline void disable()
	{
		using namespace ::cfg::driver;
		Port->BRR = EnablePin;
	}
private:
	static driver g_instance;
	driver();
};

#endif /* __DRIVER_HPP */
