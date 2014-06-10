#ifndef __DELEGATE_HPP
#define __DELEGATE_HPP

namespace delegate
{

template<typename Func> class Delegate;

template<typename ReturnType, typename ... Params>
class Delegate<ReturnType(Params...)>
{
	typedef ReturnType (*Type)(void* callee, Params...);
public:
	Delegate() :
			fpCallee(nullptr), fpCallbackFunction(nullptr)
	{
	}

	Delegate(void* callee, Type function) :
			fpCallee(callee), fpCallbackFunction(function)
	{
	}

	template<class C, ReturnType (C::*method_ptr)(Params...)>
	static Delegate from(C* object_ptr) noexcept
	{
		return
		{	object_ptr, methodCaller<C, method_ptr>};
	}

	ReturnType operator()(Params ... xs) const
	{
		return (*fpCallbackFunction)(fpCallee, xs...);
	}

	bool empty() const
	{
		return !fpCallee;
	}

private:
	void* fpCallee;
	Type fpCallbackFunction;

	template<class T, ReturnType (T::*TMethod)(Params...)>
	static ReturnType methodCaller(void* callee, Params ... xs)
	{
		T* p = static_cast<T*>(callee);
		return (p->*TMethod)(xs...);
	}
};

template<typename T, typename ReturnType, typename ... Params>
struct DelegateMaker
{
	template<ReturnType (T::*foo)(Params...)>
	static ReturnType methodCaller(void* o, Params ... xs)
	{
		return (static_cast<T*>(o)->*foo)(xs...);
	}

	template<ReturnType (T::*foo)(Params...)>
	inline static Delegate<ReturnType(Params...)> Bind(T* o)
	{
		return Delegate<ReturnType(Params...)>(o,
				&DelegateMaker::methodCaller<foo>);
	}
};

template<typename T, typename ReturnType, typename ... Params>
DelegateMaker<T, ReturnType, Params...> makeDelegate(
		ReturnType (T::*)(Params...))
{
	return DelegateMaker<T, ReturnType, Params...>();
}

#define DELEGATE(foo, thisPrt) (::delegate::makeDelegate(foo).Bind<foo>(thisPrt))
}

#endif /* __DELEGATE_HPP */
