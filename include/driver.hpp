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

	static inline void step_on() {
		using namespace ::cfg::driver;
		Port->BSRR = StepPin;
	}

	static inline void step_off() {
		using namespace ::cfg::driver;
		Port->BRR = StepPin;
	}

	static inline void dir_on() {
		using namespace ::cfg::driver;
		Port->BSRR = DirPin;
	}

	static inline void dir_off() {
		using namespace ::cfg::driver;
		Port->BRR = DirPin;
	}

	static inline void reset()
	{
		using namespace ::cfg::driver;
		Port->BRR = ResetPin;
	}

	static inline void release()
	{
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
