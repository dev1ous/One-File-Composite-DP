#ifndef OBJECT_H
#define OBJECT_H

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <assert.h>
#include <concepts>
#include <functional>
#include <any>
#include <ranges>
#include <optional>
#include "SFML/Graphics/RenderWindow.hpp"

class ibase {
public:
	virtual void draw(std::any) const = 0;
protected:
	~ibase() = default;
};

template<typename ConcreteType>
constexpr ibase vtable_for{
	[](std::any& _storage) { std::any_cast<ConcreteType&>(_storage); },
	[](std::any& _storage) { std::any_cast<ConcreteType&>(_storage).~ConcreteType(); },
	[](std::any& _storage) { std::any_cast<ConcreteType&>(_storage).draw(std::add_lvalue_reference<sf::RenderWindow>{}); },
};

class make
{
public:
	template<typename ConcreteType>
	requires (!std::same_as<std::remove_cvref_t<ConcreteType>, make>)
		make(ConcreteType&& x) : m_storage{ std::forward<ConcreteType>(x) },
		m_guetter{ &vtable_for<ConcreteType> }{}

	[[nodiscard]] ibase& operator()()noexcept {
		return m_getter(m_storage);
	}

	[[nodiscard]] ibase* operator->()noexcept {
		return &m_getter(m_storage);
	}

private:

	std::any m_storage;
	std::function<ibase& (std::any&)> m_getter;
};

template<class T>
class composite : public std::ranges::view_interface<composite<T>> {
public:

	composite(std::ranges::range auto const& x) : m_begin{ std::ranges::cbegin(x) },
		m_end{ std::ranges::cend(x) }{}

	auto begin()const noexcept {
		return m_begin;
	}

	auto end()const noexcept {
		return m_end;
	}

	void draw(sf::RenderWindow& w) const{
		if constexpr (std::same_as < T, typename std::map::mapped_type>) {
			for (auto&& x : *this) {

			}
		}
	}
private:
	typename std::ranges::iterator_t<T> m_begin;
	typename std::ranges::sentinel_t<T> m_end;
};
#endif





