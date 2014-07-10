#ifndef __UTIL_HPP
#define __UTIL_HPP

#include <utility>
#include <limits>

namespace util
{
template<typename N, int R = 10>
constexpr int digits(N value = ::std::numeric_limits<N>::max(), int result = 0)
{
	return value == 0 ? result : digits<N, R>(value / R, result + 1);
}

constexpr unsigned c_strlen(char const* str, unsigned count = 0)
{
	return ('\0' == str[0]) ? count : c_strlen(str + 1, count + 1);
}

}

#endif /* __UTIL_HPP */
