#ifndef BUTTON_H
#define BUTTON_H

#include "Container.h"
#include <functional>
#include <array>
#include "SFML/Graphics/Texture.hpp"  // for Texture (ptr only)
#include <variant>
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

template<class> 
inline constexpr bool always_false_v = false;

class Button : public Container<Button>// must be publicly inherited ! 
{
public:
	using shape_t = std::variant <sf::RectangleShape, sf::CircleShape, sf::ConvexShape, sf::Sprite>;//can be extended if you want
	using CI_button = Container<Button>;

	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	/// Create an empty button and my upcasting pointer with default argument.
	///
	////////////////////////////////////////////////////////////
	Button(Button* _parent = nullptr)noexcept :CI_button(_parent) {};

	////////////////////////////////////////////////////////////
	/// \brief Create a button from this list of arguments
	///
	/// \param i must initialize my upcast pointer firstly but i put the parameter at the end of the list with a default argument to don't have to be bothered with him when i instantiate my constructor
	/// \param Params left are just perfectly forwarded to let the compiler optimize as he wants
	///
	////////////////////////////////////////////////////////////
	template<class _Ty, class _Ty2 = sf::Text, class = std::enable_if_t<std::is_convertible_v<_Ty, shape_t>>>
	explicit Button(_Ty&& _shapes, _Ty2&& _text = sf::Text(), Button* _parent = nullptr)noexcept :
		CI_button(_parent), m_shapes{ std::forward<_Ty>(_shapes) }, m_text{ std::forward<_Ty2>(_text) } {};

	~Button() = default;

	void create_function_call(std::function<void()>)noexcept;
	void process_events(sf::Event const&)noexcept;
	void draw(sf::RenderWindow&)const noexcept;

	////////////////////////////////////////////////////////////
	/// \return True if the cursor of your mouse is in the button, False otherwise
	////////////////////////////////////////////////////////////
	constexpr bool mouse_in_button(sf::RenderWindow const& window) noexcept {
		return std::visit([&window](auto&& args) { 
			return args.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window))); }, m_shapes);
	}
	sf::Vector2f get_position()const noexcept;
	sf::FloatRect get_globalbounds()const noexcept;

	//Chaining functions for convenience

	Button& set_color_state(sf::Color const&, sf::Color const&, sf::Color const&)noexcept;
	Button& set_texture_state(sf::Texture const&, sf::Texture const&, sf::Texture const&)noexcept;
	Button& set_position(sf::Vector2f const&)noexcept;
	Button& set_origin()noexcept;
	Button& set_rotation(float)noexcept;
	Button& set_string(std::string const&)noexcept;

	Button& resize()noexcept;

	void center_text()noexcept;

	void force_hover()noexcept;
	void desactivate_hover()noexcept;
	void force_activation()noexcept;

private:

	void change_default_color(sf::Color const&)noexcept;
	void set_texture(sf::Texture const&)noexcept;

	void select()noexcept;
	void deselect()noexcept;
	void activate()noexcept;
	void desactivate()noexcept;

	shape_t m_shapes;
	sf::Text m_text;
	std::function<void()>m_click{};
	std::array<sf::Color, 3> m_col{};
	std::array<sf::Texture, 3> m_filepath{};
	sf::Vector2f m_current_texture{};

	bool m_toggle{ false };
	bool m_choose{ false };
};
#endif





