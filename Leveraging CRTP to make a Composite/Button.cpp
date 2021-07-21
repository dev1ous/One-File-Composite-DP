#include "Button.h"

void Button::create_function_call(std::function<void()> onClick)noexcept
{
	m_click = std::move(onClick);
}

void Button::process_events(sf::Event const& e)noexcept
{
	sf::FloatRect rect;

	std::visit([&rect](auto&& args) { rect = args.getGlobalBounds(); }, m_shapes);

	switch (e.type)
	{
	case sf::Event::MouseButtonPressed:
		if (rect.contains(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y)))
		{
			if (!m_choose)
				change_default_color(std::get<2>(m_col));
			else
				set_texture(std::get<2>(m_filepath));
		}
		break;
	case sf::Event::MouseButtonReleased:
		if (rect.contains(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y)))
		{
			if (m_click) {
				m_click();
			}

			desactivate();
		}
		break;
	case sf::Event::MouseMoved:
		if (rect.contains(static_cast<float>(e.mouseMove.x), static_cast<float>(e.mouseMove.y)))
			select();
		else
			deselect();
		break;
	case sf::Event::JoystickButtonPressed:
		switch (e.joystickButton.button) {
		case 0:
			if (rect.contains(sf::Vector2f(sf::Mouse::getPosition())))
			{
				if (!m_choose)
					change_default_color(std::get<2>(m_col));
				else
					set_texture(std::get<2>(m_filepath));
			}
			break;
		default:
			break;
		}
		break;
	case sf::Event::JoystickButtonReleased:
		switch (e.joystickButton.button)
		{
		case 0:
			if (rect.contains(sf::Vector2f(sf::Mouse::getPosition())))
			{
				if (m_click) {
					m_click();
				}

				desactivate();
			}
			break;
		}
		break;
	default:
		break;
	}

	//recursive call before the creation of the function "apply_foreach" in the base class
	//std::for_each(std::begin(get_childs()), std::end(get_childs()), [e](auto& h) {h.second->process_events(e); });
}

void Button::select()noexcept
{
	if (!m_choose)
		change_default_color(std::get<1>(m_col));
	else
		set_texture(std::get<1>(m_filepath));
}

void Button::deselect()noexcept
{
	if (!m_choose)
		change_default_color(std::get<0>(m_col));
	else
		set_texture(std::get<0>(m_filepath));
}

void Button::activate()noexcept
{
	if (!m_choose)
		change_default_color(std::get<2>(m_col));
	else
		set_texture(std::get<2>(m_filepath));
	if (m_click)
		m_click();
}

void Button::desactivate()noexcept
{
	if (!m_choose)
		change_default_color(std::get<1>(m_col));
	else
		set_texture(std::get<1>(m_filepath));
}

void Button::draw(sf::RenderWindow& window)const noexcept
{
	std::visit([&](auto&& args) { window.draw(args); }, m_shapes);
	window.draw(m_text);
}

void Button::set_texture(sf::Texture const& path)noexcept
{
	m_choose = true;

	std::visit([&path](auto&& args) {
		using _Ty = std::decay_t<decltype(args)>;
		if constexpr (std::is_same_v<_Ty, sf::RectangleShape> || std::is_same_v<_Ty, sf::CircleShape>
			|| std::is_same_v<_Ty, sf::ConvexShape>)
			args.setTexture(&path);
		else if constexpr (std::is_same_v<_Ty, sf::Sprite>) 
			args.setTexture(path);
		else
			static_assert(std::_Always_false<_Ty>, "Wrong type entered for setTexture of your Button");
		}, m_shapes);
}

Button& Button::set_color_state(sf::Color const& noMouseOn, sf::Color const& mouseMovedOn, sf::Color const& mouseClickedOn)noexcept
{
	m_choose = false;
	std::get<0>(m_col) = noMouseOn;
	std::get<1>(m_col) = mouseMovedOn;
	std::get<2>(m_col) = mouseClickedOn;
	return *this;
}

Button& Button::set_texture_state(sf::Texture const& noMouseOn, sf::Texture const& mouseMovedOn, sf::Texture const& mouseClickedOn) noexcept
{
	m_choose = true;
	std::get<0>(m_filepath) = noMouseOn;
	std::get<1>(m_filepath) = mouseMovedOn;
	std::get<2>(m_filepath) = mouseClickedOn;
	return *this;
}

Button& Button::resize() noexcept
{
	if (m_choose)
	{
		std::visit([this](auto&& args) {
			using _Ty = std::decay_t<decltype(args)>;
			if constexpr (std::is_same_v<_Ty, sf::RectangleShape>)
				args.setSize(m_current_texture);
			else if constexpr (std::is_same_v<_Ty, sf::CircleShape>)
				args.setRadius(m_current_texture.x);
			else if constexpr (std::is_same_v<_Ty, sf::ConvexShape> || std::is_same_v<_Ty, sf::Sprite>)
				args.setTextureRect(sf::IntRect(0, 0, static_cast<int>(m_current_texture.x), static_cast<int>(m_current_texture.y)));
			else
				static_assert(std::_Always_false<_Ty>, "Wrong type entered for resize of your Button");
			}, m_shapes);
	}

	return *this;
}

void Button::change_default_color(sf::Color const& col)noexcept
{
	m_choose = false;

	std::visit([&col](auto&& args) {
		using _Ty = std::decay_t<decltype(args)>;
		if constexpr (std::is_same_v<_Ty, sf::RectangleShape> || std::is_same_v<_Ty, sf::CircleShape>
			|| std::is_same_v<_Ty, sf::ConvexShape>)
			args.setFillColor(col);
		else if constexpr (std::is_same_v<_Ty, sf::Sprite>)
			args.setColor(col);
		else
			static_assert(std::_Always_false<_Ty>, "Wrong type entered for change_default_color of your Button");
		}, m_shapes);
}

Button& Button::set_position(sf::Vector2f const& pos)noexcept
{
	std::visit([&](auto&& args) { args.setPosition(pos); }, m_shapes);
	return *this;
}

Button& Button::set_origin() noexcept
{
	std::visit([](auto&& args) { args.setOrigin(args.getGlobalBounds().width / 2.f, args.getGlobalBounds().height / 2.f); }, m_shapes);

	return *this;
}

Button& Button::set_rotation(float rota) noexcept
{
	std::visit([=](auto&& args) { args.setRotation(rota); }, m_shapes);

	return *this;
}

Button& Button::set_string(std::string const& str) noexcept
{
	m_text.setString(str);

	return *this;
}

bool Button::mouse_in_button(sf::RenderWindow const& window) const noexcept
{
	return std::visit([&window](auto&& args) {
		return args.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window))); }, m_shapes);
}

sf::Vector2f Button::get_position()const noexcept
{
	return std::visit([](auto&& args) { return args.getPosition(); }, m_shapes);
}

sf::FloatRect Button::get_globalbounds()const noexcept
{
	return std::visit([](auto&& args) {return args.getGlobalBounds(); }, m_shapes);
}

void Button::force_hover()noexcept
{
	select();
}

void Button::desactivate_hover()noexcept
{
	deselect();
}

void Button::force_activation()noexcept
{
	if (m_click) {
		//play your music here
		m_click();
	}

	desactivate();
}

void Button::center_text()noexcept
{
	sf::FloatRect rect = m_text.getLocalBounds();

	std::visit([this, &rect](auto&& args) { m_text.setOrigin(rect.left + rect.width / 2.f, rect.top + rect.height / 2.f);
	m_text.setPosition(args.getPosition()); }, m_shapes);
}

