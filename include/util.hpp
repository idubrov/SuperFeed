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

// Our own tuple implementation
template<typename ... Items> class tuple;

template<>
class tuple<>
{
public:
	typedef void item_type;
	constexpr static unsigned arity = 0;
public:
	template<template<typename, typename... > class F, typename ...Args>
	void apply(unsigned, Args&&...)
	{
		// FIXME: Fail!
	}
};

template<typename H, typename ... T>
class tuple<H, T...>
{
public:
	typedef H item_type;
	constexpr static unsigned arity = 1 + tuple<T...>::arity;
public:
	tuple(H&& head, T&&... tail) :
			head(std::forward<H>(head)), tail(std::forward<T>(tail)...)
	{
	}

	template<template<typename, typename... > class F, typename ...Args>
	void apply(unsigned idx, Args&&... args)
	{
		if (idx > 0)
			tail.apply<F, Args...>(idx - 1, args...);
		else
			F<H, Args...>::apply(head, args...);
	}
private:
	H head;
	tuple<T...> tail;
};

}

#endif /* __UTIL_HPP */
