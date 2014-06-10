#ifndef __DELEGATE_HPP
#define __DELEGATE_HPP

namespace delegate
{

template<typename Func> class Delegate;

template<typename Return, typename ... Params>
class Delegate<Return(Params...)>
{
	typedef Return (*InvokerType)(void* callee, Params...);
public:
	Delegate() :
			_instance(nullptr), _invoker(nullptr)
	{
	}

	Delegate(Delegate const& that) = default;
	Delegate(Delegate&& that) = default;
	Delegate& operator=(const Delegate& t) = default;


	template<Return (*Function)(Params...)>
	Delegate& bind() {
		*this = from<Function>();
		return *this;
	}

	template<class C, Return (C::*Function)(Params...)>
	Delegate& bind(C* c) {
		*this = from<C, Function>(c);
		return *this;
	}

	template<Return (*Function)(Params...)>
	static Delegate from() {
		return Delegate(nullptr, &staticCaller<Function>);
	}

	template<typename C, Return (C::*Function)(Params...)>
	static Delegate from(C* c) {
		return Delegate(c, &memberCaller<C, Function>);
	}

	Return operator()(Params ... xs) const
	{
		return (*_invoker)(_instance, xs...);
	}

	bool empty() const
	{
		return !_invoker;
	}
private:
	Delegate(void* instance, InvokerType invoker) :
				_instance(instance), _invoker(invoker)
		{
		}

private:
	void* _instance;
	InvokerType _invoker;

	// Caller functions
	template<Return (*Function)(Params...)>
	static inline Return staticCaller(void*, Params... params){
		return (Function)(params...);
	}
	template<class C, Return (C::*Function)(Params...)>
	static inline Return memberCaller(void* instance, Params... params){
		return (static_cast<C*>(instance)->*Function)(params...);
	}
};

template<typename T, typename Return, typename ... Params>
struct DelegateMaker
{
	template<Return (T::*Member)(Params...)>
	inline static Delegate<Return(Params...)> bind(T* o)
	{
		return Delegate<Return(Params...)>::template from<T, Member>(o);
	}
};

template<typename T, typename Return, typename ... Params>
DelegateMaker<T, Return, Params...> makeDelegate(Return (T::*)(Params...))
{
	return DelegateMaker<T, Return, Params...>();
}
}

//#define DELEGATE(foo, thisPrt) (::delegate::detail::makeDelegate(foo).bind<foo>(thisPrt))

#endif /* __DELEGATE_HPP */
