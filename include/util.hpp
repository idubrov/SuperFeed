#ifndef __UTIL_HPP
#define __UTIL_HPP

#include <limits>

namespace util
{
template<typename N, int R = 10>
constexpr int digits(N value = ::std::numeric_limits<N>::max(), int result = 0)
{
	return value == 0 ? result : digits<N, R>(value / R, result + 1);
}
}

#endif /* __UTIL_HPP */
