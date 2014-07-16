#ifndef __SIMSTREAM_HPP
#define __SIMSTREAM_HPP

#include "util.hpp"

template<typename S>
S const& operator<<(S const& sink, char c)
{
	sink.write(c);
	return sink;
}

template<typename S>
S const& operator<<(S const& sink, char const* str)
{
	while (*str)
	{
		sink << *str++;
	}
	return sink;
}

enum Alignment
{
	Left, Right
};

template<typename N, int R, Alignment A>
struct print_format
{
	N value;
	int padding;
};

template<int Radix = 10, Alignment Align = Left>
print_format <int, Radix, Align> format(int value, int padding = 0)
{
	return
	{	value, padding};
}

template<typename S, typename N>
S const& operator<<(S const& sink, N n)
{
	sink << format<>(n);
	return sink;
}

template<typename N, int R>
constexpr int buf_size(N value = std::numeric_limits<int>::max(),
		int result = 0)
{
	return value == 0 ? result : buf_size<N, R>(value / R, result + 1);
}

template<typename S, typename N, int R = 10, Alignment A>
S const& operator<<(S const& sink, print_format <N, R, A> nn)
{
	constexpr int size = util::digits<N, R>();
	char buf[size];
	int pos = size;
	int n = nn.value;
	int padding = nn.padding;

	// FIXME: won't work for min_value
	if (n < 0)
	{
		sink << '-';
		n = -n;
	}
	do
	{
		int m = n;
		n /= R;
		char c = m - R * n;
		buf[--pos] = c < 10 ? c + '0' : c + 'a' - 10;
	} while (n);

	int printed = size - pos;
	if (A == Right)
	{
		for (int i = printed; i < padding; i++)
			sink << ' ';
	}
	while (pos < size)
	{
		sink << buf[pos++];
	}
	if (A == Left)
	{
		for (int i = printed; i < padding; i++)
			sink << ' ';
	}

	return sink;
}

struct print_blanks
{
	unsigned blanks;
};

inline print_blanks blanks(unsigned blanks)
{
	return
	{	blanks};
}

template<typename S>
S const& operator<<(S const& sink, print_blanks b)
{
	for (unsigned i = 0; i < b.blanks; i++)
		sink << ' ';
	return sink;
}
#endif /* __SIMSTREAM_HPP */
