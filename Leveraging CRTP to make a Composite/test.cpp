#include "Button.h"

//TEST

int main()
{
	sf::RenderWindow App(sf::VideoMode(1920, 1080), "");

	sf::Font font;
	font.loadFromFile("../Ressources/LEMONMILK-Regular.otf");
	sf::Texture t;
	t.loadFromFile("../Ressources/golden.png");
	//parent
	Button button(sf::RectangleShape(sf::Vector2f(20, 20)));
	button.set_color_state(sf::Color::White, sf::Color::White, sf::Color::White);
	//child( my frankestein monster :) dont forget to put a reference if you don't want to slice the object)
	Button& MartinEstLeMeilleur = button.add("martin", sf::CircleShape(10), sf::Text("martin_2", font, 10));
	//you can switch color or texture if you hold, click or do nothing with the button. You can also just put 3 same colors to have no particular state switch
	MartinEstLeMeilleur.set_color_state(sf::Color::White, sf::Color::White, sf::Color::White).set_position({ 500.f,500.f });

	button.get("martin").set_color_state(sf::Color::Blue, sf::Color::Blue, sf::Color::Blue);

	while (App.isOpen())
	{
		for (auto e = sf::Event{}; App.pollEvent(e);) {
			button.apply_foreach<&Button::process_events>(e);
		}
		App.clear();
		button.apply_foreach<&Button::draw>(App);
		App.display();
	}

	return 0;
}