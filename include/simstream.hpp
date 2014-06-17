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

template<typename N, int Radix>
struct __radix
{
	N value;
};

template<int Radix>
__radix<int, Radix> radix(int value)
{
	return { value };
}

template<typename S, typename N>
S const& operator<<(S const& sink, N n) {
	sink << radix<10>(n);
	return sink;
}

template<typename N, int Radix>
constexpr int buf_size(N value = std::numeric_limits<int>::max(), int result = 0)
{
	return value == 0 ? result : buf_size<N, Radix>(value / Radix, result + 1);
}

template<typename S, typename N, int Radix = 10>
S const& operator<<(S const& sink, __radix<N, Radix> nn) {
	constexpr int size = buf_size<N, Radix>();
	char buf[size];
	int pos = size;
	int n = nn.value;

	if (n < 0) {
		sink << '-';
		n = -n;
	}
	do {
		int m = n;
		n /= Radix;
		char c = m - Radix * n;
		buf[--pos] = c < 10 ? c + '0' : c + 'a' - 10;
	} while(n);

	while (pos < size) {
		sink << buf[pos++];
	}

	return sink;
}

#endif /* __SIMSTREAM_HPP */
