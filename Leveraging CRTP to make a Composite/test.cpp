#include "Button.h"
#include <iostream>
//TEST

int main()
{
	sf::RenderWindow App(sf::VideoMode(1920, 1080), "");

	sf::Font font;
	font.loadFromFile("../Ressources/LEMONMILK-Regular.otf");
	sf::Texture t;
	t.loadFromFile("../Ressources/golden.png");
	//parent
	Button button(sf::RectangleShape({ 20.f, 20.f }));
	button.change_default_color(sf::Color::White);
	//child( my frankestein monster :) dont forget to put a reference if you don't want to slice the object)
	Button& MartinEstLeMeilleur = button.add("martin", sf::CircleShape(10));
	MartinEstLeMeilleur.change_default_color(sf::Color::Blue);

	Button& JeSuisTropChaud = button.add("lebossquoi", sf::CircleShape(50));
	//you can switch color or texture if you hold, click or do nothing with the button. You can also just put 3 same colors to have no particular state switch
	JeSuisTropChaud.change_default_color(sf::Color::Red);
	JeSuisTropChaud.set_position({ 800.f,800.f });

	button.get("martin").set_position({900.f,900.f});
	button.get("lebossquoi").create_function_call([]() { std::cout << "aurelia" << "\n"; });

	while (App.isOpen())
	{
		for (auto e = sf::Event{}; App.pollEvent(e);) {
			button.apply_foreach(&Button::process_events, e);
		}

		button.set_color_state(sf::Color::Red, sf::Color::Cyan, sf::Color::Magenta);

		App.clear();
		button.apply_foreach(&Button::draw, App);
		App.display();
	}


	return 0;
}