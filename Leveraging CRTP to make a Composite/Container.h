#ifndef CONTAINER_H
#define CONTAINER_H

#include "stdexcept"              // for range_error
#include "utility"                // for pair
#include "vcruntime_exception.h"  // for exception
#include <iostream>
#include <algorithm>
#include <vector>

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
	using my_vec = std::vector<std::pair<std::string, _Ty >>;

	////////////////////////////////////////////////////////////
	/// \brief alias for convenient access to my pair
	////////////////////////////////////////////////////////////
	using my_pair = std::pair<std::string, _Ty>;

	explicit constexpr Container(_Ty* __p = nullptr)noexcept :m_parent(__p) {}

	////////////////////////////////////////////////////////////
	/// \brief Adding a button to my container, constructed from a chunk of my base Button and the parameter list                         
	/// Exemple : Button& derived = base.add("my_id",parameters...);
	/// 
	/// \param everything is constructed in place with an id to access the button at runtime and a variadic parameter list
	/// 
	/// \return a reference to the last element of my container ( the freshly created object ) 
	////////////////////////////////////////////////////////////
	template<class ...Args>
	[[nodiscard]] _Ty& add(std::string_view, Args &&...args)noexcept;

	////////////////////////////////////////////////////////////
	/// \brief Give you the access, at runtime, to a derived object in your container
	/// 
	/// \param the id of your object
	/// 
	/// \return a reference to the object corresponding to the id you gave or throw an exception with a message in the console 
	////////////////////////////////////////////////////////////
	_Ty& get(std::string_view)noexcept;


	////////////////////////////////////////////////////////////
	/// \brief Remove, at runtime, the object corresponding to the id you gave
	/// 
	/// \param the id of your object
	////////////////////////////////////////////////////////////
	void remove(std::string_view)noexcept;

	////////////////////////////////////////////////////////////
	/// \brief Remove the base button and all his derived buttons
	////////////////////////////////////////////////////////////
	void clear()noexcept;

	////////////////////////////////////////////////////////////
	/// \brief Apply a member function of my derived class to the base button and all his derived buttons                                      
	/// Exemple : base.apply_foreach<&Button::my_member_function>(parameters...);
	///
	/// \param a variadic parameter list where you can put the parameters of the function applied
	////////////////////////////////////////////////////////////
	template<auto _Ty2, class ...Args>
	void apply_foreach(Args &&... args)noexcept;

protected:

	~Container() = default;

	////////////////////////////////////////////////////////////
	/// \return Access to the container
	////////////////////////////////////////////////////////////
	[[nodiscard]] my_vec& get_childs()noexcept { return m_children; }

	////////////////////////////////////////////////////////////
	/// \return Access to the button base
	////////////////////////////////////////////////////////////
	[[nodiscard]] _Ty* get_parent()noexcept { return m_parent; }

private:
	////////////////////////////////////////////////////////////
	/// Data Members
	////////////////////////////////////////////////////////////

	_Ty* m_parent;
	my_vec m_children = {};
};

#endif // !Container_h

template<class _Ty>
template<class ...Args>
inline _Ty& Container<_Ty>::add(std::string_view id, Args &&...args)noexcept
{
	m_children.emplace_back(std::piecewise_construct, std::forward_as_tuple(id),
		std::forward_as_tuple(std::forward<Args>(args)..., static_cast<_Ty*>(this)));
	return m_children.back().second;
}

template<class _Ty>
inline _Ty& Container<_Ty>::get(std::string_view id)noexcept
{
	try {
		if (auto x = std::find_if(std::begin(m_children), std::end(m_children), [id](my_pair const& b) {
			return b.first == id; }); x != std::end(m_children))
			return x->second;
		else
			throw std::range_error("didnt find the id of your object");
	}
	catch (std::exception const& e)
	{
		std::cout << "Exception : " << e.what() << "\n";
	}
#ifdef __clang__
#pragma clang diagnostic ignored "-Wreturn-type"
#elif _MSC_VER
	fatal();
#endif
}

template<class _Ty>
inline void Container<_Ty>::remove(std::string_view id)noexcept
{
	m_children.erase(std::remove_if(std::begin(m_children), std::end(m_children), [id](my_pair const& k) {
		return k.first == id; }), std::end(m_children));
}

template<class _Ty>
inline void Container<_Ty>::clear()noexcept
{
	m_children.clear();
}

template<class _Ty>
template<auto _Ty2, class ...Args>
inline void Container<_Ty>::apply_foreach(Args &&... args) noexcept
{
	(static_cast<_Ty*>(this)->*_Ty2)(std::forward<Args>(args)...);
	for (auto& [k, v] : m_children) {
		(v.*_Ty2)(std::forward<Args>(args)...);
	}
}








