#ifndef __5SWITCH_HPP
#define __5SWITCH_HPP

#include "config.hpp"

class switch5
{
public:
	enum Position
	{
		NONE = 0,	// invalid state
		LL = 1,		// left-left
		L = 2,		// left
		M = 3,		// middle
		R = 4,		// right
		RR = 5		// right-right
	};

	static Position position();
private:
	static switch5 g_instance;
	switch5();
};

#endif /* __5SWITCH_HPP */
