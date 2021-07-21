#ifndef CONTAINER_H
#define CONTAINER_H

#include "stdexcept"              // for range_error
#include "utility"                // for pair
#include "vcruntime_exception.h"  // for exception
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

	explicit constexpr Container(_Ty* __p = nullptr) noexcept :m_parent(__p) {}

	////////////////////////////////////////////////////////////
	/// \brief Adding a button to my container, constructed from a chunk of my base Button and the parameter list                         
	/// Example : Button& derived = base.add("my_id",parameters...);
	/// 
	/// \param everything is constructed in place with an id to access the button at runtime and a variadic parameter list
	/// 
	/// \return a reference to the last element of my container ( the freshly created object ) 
	////////////////////////////////////////////////////////////
	template<class ...Args>
	[[nodiscard]] constexpr _Ty& add(std::string_view, Args &&...args) noexcept;

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
	/// \brief Remove the base button and all his derived buttons
	////////////////////////////////////////////////////////////
	constexpr void clear() noexcept;

	////////////////////////////////////////////////////////////
	/// \brief Apply a member function of my derived class to the base button and all his derived buttons
	/// Example : base.apply_foreach(&Button::my_member_function, parameters...);
	///
	/// \param a variadic parameter list where you can put the parameters of the function applied
	////////////////////////////////////////////////////////////
	template<class _Ty2, class ...Args>
	constexpr void apply_foreach(_Ty2, Args &&... args) noexcept;

protected:

	constexpr ~Container() = default;

	////////////////////////////////////////////////////////////
	/// \return Access to the container
	////////////////////////////////////////////////////////////
	[[nodiscard]] constexpr my_vec& get_childs()noexcept { return m_children; }

	////////////////////////////////////////////////////////////
	/// \return Access to the button base
	////////////////////////////////////////////////////////////
	[[nodiscard]] constexpr _Ty* get_parent()noexcept { return m_parent; }

private:

	////////////////////////////////////////////////////////////
	/// Data Members
	////////////////////////////////////////////////////////////
	_Ty* m_parent;
	my_vec m_children{};
	//my_vec::value_type my_pair;
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
	//const auto elem = std::views::keys(m_children);
	if (const auto it = std::ranges::find(m_children, id, &my_vec::value_type::first); it != std::ranges::end(m_children))
		return *it->second;
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
	//const auto elem = std::views::keys(m_children);
	const auto sub = std::ranges::remove(m_children, id, &my_vec::value_type::first);
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

	const auto pred = [&](auto&& __p) { std::invoke(func, __p, std::forward<Args>(args)...); };
	const auto elem = std::views::values(m_children);
	std::ranges::for_each(elem, pred);
}