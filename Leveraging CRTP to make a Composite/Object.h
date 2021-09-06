#ifndef OBJECT_H
#define OBJECT_H

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <assert.h>
#include <concepts>
#include <functional>

template<class _Ty>
concept base_like = requires(_Ty t) {
	t.invoke_any_methods([] {});
};

template<class _Ty>
class object {

public:
	object(_Ty x) : m_self{ base<_Ty>{std::move(x)} } {}

private:
	template<base_like base_impl>
	class base final : public base_impl {
	public:
		//custom implementation of invoke_r standard impl for c++23
		template<class ReturnType, class Func, class... Args>
		requires std::is_invocable_r_v<ReturnType, Func, Args...>
			constexpr ReturnType operator()(Func&& f, Args&&... args)
			noexcept(std::is_nothrow_invocable_r_v<ReturnType, Func, Args...>) {
			if constexpr (std::is_void_v<ReturnType>)
				std::invoke(std::forward<Func>(f), static_cast<base_impl&>(*this), std::forward<Args>(args)...);
			else
				return std::invoke(std::forward<Func>(f), static_cast<base_impl&>(*this), std::forward<Args>(args)...);
		}


	};

	base<_Ty> m_self;
};

template<class _Ty>
class composite {
	constexpr decltype(auto) current(std::vector<_Ty>const&);
	constexpr void copy_last(std::vector<_Ty>&);
	constexpr void remove(std::vector<_Ty>&);
};

class my_class {
	
};

#endif


template<class _Ty>
inline constexpr decltype(auto) composite<_Ty>::current(std::vector<_Ty> const& x)
{
	assert(x.size());
	return x.back();
}

template<class _Ty>
inline constexpr void composite<_Ty>::copy_last(std::vector<_Ty>& x)
{
	assert(x.size());
	x.push_back(std::move(x.back()));
}

template<class _Ty>
inline constexpr void composite<_Ty>::remove(std::vector<_Ty>& x)
{
	assert(x.size());
	x.pop_back();
}