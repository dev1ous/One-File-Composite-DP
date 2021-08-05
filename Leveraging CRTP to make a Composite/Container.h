#ifndef CONTAINER_H
#define CONTAINER_H

#include <utility>                // for pair
#include <any>
#include <tuple>
#include <memory>

template<class _Ty>
struct make {
	using maker_t = _Ty(*)(std::any);

	template<class Tuple>
	static constexpr maker_t make_tuple_maker() {
		return [](std::any vtup)->_Ty {
			return std::make_from_tuple<_Ty>(std::forward<Tuple>(std::any_cast<std::remove_reference_t<Tuple>&>(vtup)));
		};
	}

	template<class U>
	static constexpr maker_t make_element_maker() {
		return [](std::any velem)->_Ty {
			return _Ty{ std::forward<U>(std::any_cast<std::remove_reference_t<U>&>(velem)) };
		};
	}

	std::any m_ptr{};
	maker_t m_maker{ nullptr };

	template<class U>
	requires std::is_constructible_v<_Ty, U> && !std::is_same_v<std::remove_cvref_t<U>, make>
		constexpr make(U&& u) : m_ptr{ std::make_any<U>(u) }, m_maker{ make_element_maker<U>() }{}

	template<class Tuple>
	requires !std::is_constructible_v<_Ty, Tuple> && !std::is_same_v<std::remove_cvref_t<Tuple>, make>
		&& (0 <= std::tuple_size_v<std::remove_reference_t<Tuple>>)
		constexpr make(Tuple&& tup) : m_ptr{ std::make_any<Tuple>(tup) }, m_maker{ make_tuple_maker<Tuple>() }{}


	constexpr _Ty operator()()const {
		return m_maker(m_ptr);
	}
};

#include "stdexcept"              // for range_error
#include <iostream>
#include <algorithm>
#include <vector>
#include <ranges>

#ifdef __clang__
#elif _MSC_VER
inline __declspec(noreturn) extern void fatal() {}
#endif

template<class _Ty>
class Container
{
public:

	////////////////////////////////////////////////////////////
	/// \brief vector of pair cause possibly more iteration on my elements than lookup or deletion
	////////////////////////////////////////////////////////////
	using my_vec = std::vector<std::pair<std::string, std::unique_ptr<_Ty >>>;

	explicit constexpr Container(_Ty* __p = nullptr) noexcept :m_parent{ __p } {}

	////////////////////////////////////////////////////////////
	/// \brief Adding a derived object to my container, constructed from a chunk of my derived class freshly created (not in the vector) and the parameter list                                                                                                    
	/// Example : Button base{};                                                                                                               
	/// Button& derived = base.add("my_id",parameters...);
	/// 
	/// \param everything is constructed in place with an id to access the button at runtime and a variadic parameter list
	/// 
	/// \return a reference to the last element of my container ( the freshly created object ) 
	////////////////////////////////////////////////////////////
	template<class ...Args>
	[[nodiscard]] constexpr _Ty& add(std::string_view, Args && ...args) noexcept;
	/*template<class ...Args>
	[[nodiscard]] constexpr _Ty& add(std::string_view id, Args&&...args) {
		return temp_func(id, std::forward_as_tuple(args...),
			std::make_index_sequence < sizeof...(args) - 1>{});
	}
	template<class Ts>
	[[nodiscard]] constexpr _Ty& add(std::string_view id, Ts&& args, Container<_Ty>* _this) {
		m_children.emplace_back(std::piecewise_construct, std::forward_as_tuple(id),
			std::forward_as_tuple(std::make_unique<_Ty>(std::forward<Ts>(args), static_cast<_Ty*>(_this))));
		return *m_children.back().second;
	}*/
	////////////////////////////////////////////////////////////
	/// \brief Give you the access to a derived object in your container
	/// 
	/// \param the id of your object
	/// 
	/// \return a reference to the object corresponding to the id you gave or throw an exception 
	////////////////////////////////////////////////////////////
	[[nodiscard]] constexpr _Ty& get(std::string_view) const;

	////////////////////////////////////////////////////////////
	/// \brief Remove the object corresponding to the id you gave
	/// 
	/// \param the id of your object
	////////////////////////////////////////////////////////////
	constexpr void remove(std::string_view) noexcept;

	////////////////////////////////////////////////////////////
	/// \brief Remove all elements of my container, the derived objects
	////////////////////////////////////////////////////////////
	constexpr void clear() noexcept;

	////////////////////////////////////////////////////////////
	/// \brief Apply a member function of my derived class to the base object and all his derived objects
	/// Example : base.apply_foreach(&Button::my_member_function, parameters...);
	///
	/// \param a variadic parameter list where you can put the parameters of the function applied
	////////////////////////////////////////////////////////////
	template<class _Ty2, class ...Args>
	constexpr void apply_foreach(_Ty2, Args &&... args) noexcept;

protected:

	constexpr ~Container() = default;

	////////////////////////////////////////////////////////////
	/// \return Access to an editable view of the container
	////////////////////////////////////////////////////////////
	[[nodiscard]] constexpr auto get_childs() const noexcept -> 
		std::span<std::pair<std::string,std::unique_ptr<_Ty>>const > { return m_children; }

	////////////////////////////////////////////////////////////
	/// \return Access to the button base
	////////////////////////////////////////////////////////////
	[[nodiscard]] constexpr _Ty* get_parent()noexcept { return m_parent; }

private:

	/*template<class Dummy = _Ty, class ...Args>
	[[nodiscard]] constexpr Dummy& temp_func(std::string_view id, Container<Dummy>* _this, Args const&... args) {
		m_children.emplace_back(std::piecewise_construct, std::forward_as_tuple(id),
			std::forward_as_tuple(std::make_unique<Dummy>(std::forward<Args>(args)..., static_cast<Dummy*>(_this))));
		return *m_children.back().second;
	}

	template<class...Args, std::size_t...Indexes>
	[[nodiscard]] constexpr _Ty& temp_func(std::string_view id, std::tuple<Args...> args_then_constructor, std::index_sequence<Indexes...>) {
		auto constexpr constructor_index = sizeof...(Args) - 1;
		return temp_func(id, std::get<constructor_index>(args_then_constructor),
			&std::get<Indexes>(args_then_constructor)...);
	}*/
	////////////////////////////////////////////////////////////
	/// Data Members
	////////////////////////////////////////////////////////////
	_Ty* m_parent;
	my_vec m_children{};
};

#endif // !Container_h

template<class _Ty>
template<class ...Args>
inline constexpr _Ty& Container<_Ty>::add(std::string_view id, Args && ...args) noexcept
{
	m_children.emplace_back(std::piecewise_construct, std::forward_as_tuple(id),
		std::forward_as_tuple(std::make_unique<_Ty>(std::forward<Args>(args)..., static_cast<_Ty*>(this))));
	return *m_children.back().second;
}

template<class _Ty>
inline constexpr _Ty& Container<_Ty>::get(std::string_view id) const
{
	const auto elem = std::views::keys(m_children);

	if (const auto it = std::ranges::find(elem, id, [](auto&& d) {return d; }); it != std::ranges::end(elem))
		return *it.base()->second;
	else
		throw std::range_error("didn't find the id of your object");

#ifdef __clang__
#pragma clang diagnostic ignored "-Wreturn-type"
#elif _MSC_VER
	fatal();
#endif
}

template<class _Ty>
inline constexpr void Container<_Ty>::remove(std::string_view id) noexcept
{
	const auto vec = std::views::all(m_children);
	const auto sub = std::ranges::remove(vec, id, &decltype(get_childs())::value_type::first);
	m_children.erase(sub.begin(), sub.end());
}

template<class _Ty>
inline constexpr void Container<_Ty>::clear() noexcept
{
	if (!m_children.empty())
		m_children.clear();
}

template<class _Ty>
template<class _Ty2, class ...Args>
inline constexpr void Container<_Ty>::apply_foreach(_Ty2 func, Args &&... args) noexcept
{
	std::invoke(func, static_cast<_Ty*>(this), std::forward<Args>(args)...);

	const auto pred = [&](auto&& d) { std::invoke(func, d, std::forward<Args>(args)...); };
	const auto elem = std::views::values(m_children);
	std::ranges::for_each(elem, pred);
}