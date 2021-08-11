#ifndef BUTTON_H
#define BUTTON_H

#include "Container.h"
#include <array>
#include <variant>
#include <functional>
#include "SFML/Graphics/Texture.hpp"  // for Texture (ptr only)
#include "SFML/Window/Event.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++17-extensions"
#elif _MSC_VER
#endif

class Button : public Container<Button>// must be publicly inherited ! 
{
public:
	using shape_t = std::variant <sf::RectangleShape, sf::CircleShape, sf::ConvexShape, sf::Sprite>;//can be extended if you want

	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	////////////////////////////////////////////////////////////
	Button() = default;

	////////////////////////////////////////////////////////////
	/// \brief Create a button from this list of arguments
	///
	////////////////////////////////////////////////////////////
	explicit Button(Button* _parent, shape_t const& _shape_in, sf::Text const& _text_in = {}) noexcept :
		Container<Button>(_parent), m_shapes{ _shape_in }, m_text{ _text_in }{}
	explicit Button(Button* _parent, shape_t&& _shape_in, sf::Text&& _text_in = {}) noexcept :
		Container<Button>(_parent), m_shapes{ std::move(_shape_in) }, m_text{ std::move(_text_in) }{}
	explicit Button(Button* _parent, sf::Text const& _text_in) noexcept :
		Container<Button>(_parent), m_text{ _text_in }{}
	explicit Button(Button* _parent, sf::Text&& _text_in) noexcept :
		Container<Button>(_parent), m_text{ std::move(_text_in) }{}
	////////////////////////////////////////////////////////////
	/// \brief Create a button from this list of arguments
	///
	////////////////////////////////////////////////////////////
	Button(shape_t const& _shape_in, sf::Text const& _text_in = {}) noexcept :
		m_shapes{ _shape_in }, m_text{ _text_in }{}
	Button(shape_t&& _shape_in, sf::Text&& _text_in = {}) noexcept :
		m_shapes{ std::move(_shape_in) }, m_text{ std::move(_text_in) }{}
	Button(sf::Text const& _text_in) noexcept : m_text{ _text_in } {}
	Button(sf::Text&& _text_in) noexcept : m_text{ std::move(_text_in) } {}

	void create_function_call(std::function<void()>)noexcept;
	void process_events(sf::Event const&);
	void draw(sf::RenderWindow&)const;

	////////////////////////////////////////////////////////////
	/// \return True if the cursor of your mouse is in the button, False otherwise
	////////////////////////////////////////////////////////////
	bool mouse_in_button(sf::RenderWindow const&)const;
	sf::Vector2f get_position()const noexcept;
	sf::FloatRect get_globalbounds()const noexcept;

	//Chaining functions for convenience

	Button& set_color_state(sf::Color const&, sf::Color const&, sf::Color const&);
	Button& set_texture_state(sf::Texture const&, sf::Texture const&, sf::Texture const&);
	Button& set_position(sf::Vector2f const&);
	Button& set_origin();
	Button& set_rotation(float);
	Button& set_string(std::string const&);

	Button& resize();

	void change_default_color(sf::Color const&);
	void set_texture(sf::Texture const&);

	void center_text() noexcept;

	void force_hover() noexcept;
	void desactivate_hover() noexcept;
	void force_activation() noexcept;

private:

	void select();
	void deselect();
	void activate();
	void desactivate();

	shape_t m_shapes;
	sf::Text m_text;
	std::function<void()>m_click{};
	std::array<sf::Color, 3> m_col{};
	std::array<sf::Texture, 3> m_filepath{};
	sf::Vector2f m_current_texture{};

	[[maybe_unused]] bool m_toggle{ false };
	bool m_choose{ false };
};
#endif