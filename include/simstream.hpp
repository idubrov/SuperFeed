#ifndef __SIMSTREAM_HPP
#define __SIMSTREAM_HPP

#include <limits>

template<typename S>
S const& operator<<(S const& sink, char const* str)
{
	while (*str)
	{
		sink << *str++;
	}
	return sink;
}

template<typename S>
S const& operator<<(S const& sink, int n) {
	constexpr int base = 10;
	constexpr int size = sizeof(n) * 3; // 3 characters max per byte
	char buf[size];
	int pos = size;

	if (n < 0) {
		sink << '-';
		n = -n;
	}
	do {
		int m = n;
		n /= base;
		char c = m - base * n;
		buf[--pos] = c < 10 ? c + '0' : c + 'a' - 10;
	} while(n);

	while (pos < size) {
		sink << buf[pos++];
	}

	return sink;
}

#endif /* __SIMSTREAM_HPP */
