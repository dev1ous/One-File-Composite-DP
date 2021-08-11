#ifndef CONTAINER_H
#define CONTAINER_H

#include <stdexcept>
#include <memory>
#include <algorithm>
#include <vector>
#include <ranges>

template<class _Ty>
class Container
{
public:

	explicit constexpr Container(_Ty* __p = nullptr) noexcept :m_parent(__p) {}

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
	[[nodiscard]] constexpr auto& add(std::string_view, Args && ...args) noexcept;
	
	////////////////////////////////////////////////////////////
	/// \brief Give you the access to a derived object in your container
	/// 
	/// \param the id of your object
	/// 
	/// \return a reference to the object corresponding to the id you gave or throw an exception 
	////////////////////////////////////////////////////////////
	[[nodiscard]] constexpr auto& get(std::string_view) const;

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
	/// Example : base.apply_foreach<&Button::my_member_function>(parameters...);
	///
	/// \param a variadic parameter list where you can put the parameters of the function applied
	////////////////////////////////////////////////////////////
	template<auto Func, class ...Args>
	constexpr void apply_foreach(Args &&... args) noexcept;

protected:

	~Container() = default;

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
	////////////////////////////////////////////////////////////
	/// Data Members
	////////////////////////////////////////////////////////////
	_Ty* m_parent{ nullptr };
	std::vector<std::pair<std::string, std::unique_ptr<_Ty >>> m_children{};
};

#endif // !Container_h

template<class _Ty>
template<class ...Args>
inline constexpr auto& Container<_Ty>::add(std::string_view id, Args && ...args) noexcept
{
	m_children.emplace_back(std::piecewise_construct, std::forward_as_tuple(id),
		std::forward_as_tuple(std::make_unique<_Ty>(static_cast<_Ty*>(this), std::forward<Args>(args)...)));
	return *m_children.back().second;
}

template<class _Ty>
inline constexpr auto& Container<_Ty>::get(std::string_view id) const
{
	const auto elem = std::views::keys(m_children);

	if (const auto it = std::ranges::find(elem, id, [](auto&& d) {return d; }); it != std::ranges::end(elem))
		return *it.base()->second;
	throw std::range_error("didn't find the id of your object");
}

template<class _Ty>
inline constexpr void Container<_Ty>::remove(std::string_view id) noexcept
{
	const auto pred = [&](auto const& p) {return p.first == id; };
	std::erase_if(m_children, pred);
}

template<class _Ty>
inline constexpr void Container<_Ty>::clear() noexcept
{
	if (!m_children.empty())
		m_children.clear();
}

template<class _Ty>
template<auto Func, class ...Args>
inline constexpr void Container<_Ty>::apply_foreach(Args &&... args) noexcept
{
	std::invoke(Func, static_cast<_Ty*>(this), std::forward<Args>(args)...);

	const auto pred = [&](auto&& d) { std::invoke(Func, d, std::forward<Args>(args)...); };
	std::ranges::for_each(std::views::values(m_children), pred);
}