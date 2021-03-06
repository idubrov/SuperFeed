#ifndef __TUPLE_UTIL_HPP
#define __TUPLE_UTIL_HPP

#include <tuple>
#include <type_traits>

namespace util
{
namespace detail
{

template<int...>
struct integer_sequence
{};

template<int N, int ...S>
struct make_integer_sequence
{
	using sequence = typename make_integer_sequence<N - 1, N - 1, S...>::sequence;
};

template<int ...S>
struct make_integer_sequence<0, S...>
{
	using sequence = integer_sequence<S...>;
};


template<typename Tuple>
struct apply_wrapper
{
	using RawTuple = typename std::remove_reference<Tuple>::type;
	constexpr static std::size_t arity = std::tuple_size<RawTuple>::value;

	template<unsigned N, typename ...Args>
	struct apply_wrapper2
	{
		using ArgTuple = std::tuple<Args...>;
		apply_wrapper2(Tuple&& tuple, Args&&... args) :
				tuple(tuple), args(args...)
		{
		}
		apply_wrapper2(Tuple&& tuple, ArgTuple& args) :
				tuple(tuple), args(args)
		{
		}

		template<typename Ret, template<typename, typename ... > class F>
		Ret apply_to(unsigned idx)
		{
			if (idx > 0)
			{
				return apply_wrapper2<N + 1, Args...>(tuple, args).template apply_to<Ret, F>(idx - 1);
			}
			else
			{
				return apply_wrapper3<Ret, F>::apply(typename make_integer_sequence<sizeof...(Args)>::sequence(), tuple, args);
			}
		}

		template<typename Ret, template<typename, typename ... > class F>
		struct apply_wrapper3
		{
			template<int ...S>
			static Ret apply(integer_sequence<S...>, Tuple&& tuple, ArgTuple& args)
			{
				using H = typename std::tuple_element<N, RawTuple>::type;
				return F<H, Args...>::apply(std::get<N>(tuple),
						std::forward<typename std::tuple_element<S, ArgTuple>::type>(std::get<S>(args))...);
			}
		};

		Tuple&& tuple;
		ArgTuple args;
	};

	template<typename ...Args>
	struct apply_wrapper2<arity, Args...>
	{
		apply_wrapper2(Tuple&&, Args&&...)
		{
		}
		apply_wrapper2(Tuple&&, std::tuple<Args...>&)
		{
		}

		template<typename Ret, template<typename, typename ... > class F>
		Ret apply_to(unsigned)
		{
			// FIXME: Fail at runtime?
			return Ret();
		}
	};
};
}

template<typename Tuple, typename ...Args>
detail::apply_wrapper<Tuple>::apply_wrapper2<0, Args...> make_tuple_applicator(Tuple&& tuple, Args&&... args)
{
	using Ret = typename detail::apply_wrapper<Tuple>::template apply_wrapper2<0, Args...>;
	return Ret(tuple, std::forward<Args>(args)...);
}
}

#endif /* __TUPLE_UTIL_HPP */
